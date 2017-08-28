#include <steemit/protocol/asset.hpp>

/*

The bounds on asset serialization are as follows:

index : field
0     : decimals
1..6  : symbol
   7  : \0
*/

namespace steemit {
    namespace protocol {
        typedef boost::multiprecision::int128_t int128_t;

        asset::asset() : amount(0), symbol(STEEM_SYMBOL) {

        }

        asset::asset(share_type a, asset_symbol_type id) : amount(a), symbol(id) {
        }

        asset::asset(share_type a, asset_name_type id) : amount(a) {
            string s = fc::trim(id);

            symbol = uint64_t(3);
            char *sy = (char *) &symbol;

            size_t symbol_size = id.size();

            if (symbol_size > 0) {
                FC_ASSERT(symbol_size <= 6);

                std::string symbol_string(id);

                FC_ASSERT(std::find_if(symbol_string.begin(), symbol_string.end(), [&](const char &c) -> bool {
                    return std::isdigit(c);
                }) == symbol_string.end());
                memcpy(sy + 1, symbol_string.c_str(), symbol_size);
            }
        }

        uint8_t asset::decimals() const {
            auto a = (const char *) &symbol;
            uint8_t result = uint8_t(a[0]);
            FC_ASSERT(result < 15);
            return result;
        }

        void asset::set_decimals(uint8_t d) {
            FC_ASSERT(d < 15);
            auto a = (char *) &symbol;
            a[0] = d;
        }

        asset_name_type asset::symbol_name() const {
            auto a = (const char *) &symbol;
            FC_ASSERT(a[7] == 0);
            return &a[1];
        }

        int64_t asset::precision() const {
            static int64_t table[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000ll, 1000000000ll,
                                      10000000000ll, 100000000000ll, 1000000000000ll, 10000000000000ll,
                                      100000000000000ll};
            uint8_t d = decimals();
            return table[d];
        }

        string asset::to_string() const {
            int64_t prec = precision();
            string result = fc::to_string(amount.value / prec);
            if (prec > 1) {
                auto fract = amount.value % prec;
                // prec is a power of ten, so for example when working with
                // 7.005 we have fract = 5, prec = 1000.  So prec+fract=1005
                // has the correct number of zeros and we can simply trim the
                // leading 1.
                result += "." + fc::to_string(prec + fract).erase(0, 1);
            }
            return result + " " + symbol_name();
        }

        asset asset::from_string(const string &from) {
            try {
                string s = fc::trim(from);
                auto space_pos = s.find(' ');
                auto dot_pos = s.find('.');

                asset result;
                result.symbol = uint64_t(3);
                auto sy = (char *) &result.symbol;

                if (space_pos == std::string::npos && dot_pos == std::string::npos &&
                    std::find_if(from.begin(), from.end(), [&](const std::string::value_type &c) -> bool {
                        return std::isdigit(c);
                    }) == from.end()) {
                    result.amount = 0;
                } else if (dot_pos != std::string::npos) {
                    FC_ASSERT(space_pos > dot_pos);

                    auto intpart = s.substr(0, dot_pos);
                    auto fractpart = "1" + s.substr(dot_pos + 1, space_pos - dot_pos - 1);
                    result.set_decimals(fractpart.size() - 1);

                    result.amount = fc::to_int64(intpart);
                    result.amount.value *= result.precision();
                    result.amount.value += fc::to_int64(fractpart);
                    result.amount.value -= result.precision();
                } else {
                    auto intpart = s.substr(0, space_pos);
                    result.amount = fc::to_int64(intpart);
                    result.set_decimals(0);
                }
                auto symbol = s.substr(space_pos + 1);
                size_t symbol_size = symbol.size();

                if (symbol_size > 0) {
                    FC_ASSERT(symbol_size <= 6);
                    memcpy(sy + 1, symbol.c_str(), symbol_size);
                }

                return result;
            } FC_CAPTURE_AND_RETHROW((from))
        }

        bool operator==(const price &a, const price &b) {
            if (std::tie(a.base.symbol, a.quote.symbol) != std::tie(b.base.symbol, b.quote.symbol)) {
                return false;
            }

            const auto amult = uint128_t(b.quote.amount.value) * a.base.amount.value;
            const auto bmult = uint128_t(a.quote.amount.value) * b.base.amount.value;

            return amult == bmult;
        }

        bool operator<(const price &a, const price &b) {
            if (a.base.symbol < b.base.symbol) {
                return true;
            }
            if (a.base.symbol > b.base.symbol) {
                return false;
            }
            if (a.quote.symbol < b.quote.symbol) {
                return true;
            }
            if (a.quote.symbol > b.quote.symbol) {
                return false;
            }

            const auto amult = uint128_t(b.quote.amount.value) * a.base.amount.value;
            const auto bmult = uint128_t(a.quote.amount.value) * b.base.amount.value;

            return amult < bmult;
        }

        bool operator<=(const price &a, const price &b) {
            return (a == b) || (a < b);
        }

        bool operator!=(const price &a, const price &b) {
            return !(a == b);
        }

        bool operator>(const price &a, const price &b) {
            return !(a <= b);
        }

        bool operator>=(const price &a, const price &b) {
            return !(a < b);
        }

        asset operator*(const asset &a, const price &b) {
            if (a.symbol_name() == b.base.symbol_name()) {
                FC_ASSERT(b.base.amount.value > 0);
                uint128_t result = (uint128_t(a.amount.value) * b.quote.amount.value) / b.base.amount.value;
                FC_ASSERT(result.hi == 0);
                return asset(result.to_uint64(), b.quote.symbol);
            } else if (a.symbol_name() == b.quote.symbol_name()) {
                FC_ASSERT(b.quote.amount.value > 0);
                uint128_t result = (uint128_t(a.amount.value) * b.base.amount.value) / b.quote.amount.value;
                FC_ASSERT(result.hi == 0);
                return asset(result.to_uint64(), b.base.symbol);
            }
            FC_THROW_EXCEPTION(fc::assert_exception, "invalid asset * price", ("asset", a)("price", b));
        }

        price operator/(const asset &base, const asset &quote) {
            try {
                FC_ASSERT(base.symbol_name() != quote.symbol_name());
                return price{base, quote};
            } FC_CAPTURE_AND_RETHROW((base)(quote))
        }

        price price::max(asset_symbol_type base, asset_symbol_type quote) {
            return asset(share_type(STEEMIT_MAX_SHARE_SUPPLY), base) / asset(share_type(1), quote);
        }

        price price::min(asset_symbol_type base, asset_symbol_type quote) {
            return asset(1, base) / asset(STEEMIT_MAX_SHARE_SUPPLY, quote);
        }

        price price::max(asset_name_type base, asset_name_type quote) {
            return asset(share_type(STEEMIT_MAX_SHARE_SUPPLY), base) / asset(share_type(1), quote);
        }

        price price::min(asset_name_type base, asset_name_type quote) {
            return asset(1, base) / asset(STEEMIT_MAX_SHARE_SUPPLY, quote);
        }

        bool price::is_null() const {
            return *this == price();
        }

        void price::validate() const {
            try {
                FC_ASSERT(base.amount > share_type(0));
                FC_ASSERT(quote.amount > share_type(0));
                FC_ASSERT(base.symbol_name() != quote.symbol_name());
            } FC_CAPTURE_AND_RETHROW((base)(quote))
        }

        price price::call_price(const asset &debt, const asset &collateral, uint16_t collateral_ratio) {
            try {
                //wdump((debt)(collateral)(collateral_ratio));
                boost::rational<int128_t> swan(debt.amount.value, collateral.amount.value);
                boost::rational<int128_t> ratio(collateral_ratio, STEEMIT_COLLATERAL_RATIO_DENOM);
                auto cp = swan * ratio;

                while (cp.numerator() > STEEMIT_MAX_SHARE_SUPPLY || cp.denominator() > STEEMIT_MAX_SHARE_SUPPLY) {
                    cp = boost::rational<int128_t>((cp.numerator() >> 1) + 1, (cp.denominator() >> 1) + 1);
                }

                return ~(asset(cp.numerator().convert_to<int64_t>(), debt.symbol) /
                         asset(cp.denominator().convert_to<int64_t>(), collateral.symbol));
            } FC_CAPTURE_AND_RETHROW((debt)(collateral)(collateral_ratio))
        }

        void price_feed::validate() const {
            try {
                if (!settlement_price.is_null()) {
                    settlement_price.validate();
                }
                FC_ASSERT(maximum_short_squeeze_ratio >= STEEMIT_MIN_COLLATERAL_RATIO);
                FC_ASSERT(maximum_short_squeeze_ratio <= STEEMIT_MAX_COLLATERAL_RATIO);
                FC_ASSERT(maintenance_collateral_ratio >= STEEMIT_MIN_COLLATERAL_RATIO);
                FC_ASSERT(maintenance_collateral_ratio <= STEEMIT_MAX_COLLATERAL_RATIO);
                max_short_squeeze_price(); // make sure that it doesn't overflow

                //FC_ASSERT( maintenance_collateral_ratio >= maximum_short_squeeze_ratio );
            } FC_CAPTURE_AND_RETHROW((*this))
        }

        bool price_feed::is_for(asset_name_type asset_name) const {
            try {
                if (!settlement_price.is_null()) {
                    return (settlement_price.base.symbol_name() == asset_name);
                }
                if (!core_exchange_rate.is_null()) {
                    return (core_exchange_rate.base.symbol_name() == asset_name);
                }
                // (null, null) is valid for any feed
                return true;
            } FC_CAPTURE_AND_RETHROW((*this))
        }

        price price_feed::max_short_squeeze_price() const {
            boost::rational<int128_t> sp(settlement_price.base.amount.value,
                                         settlement_price.quote.amount.value); //debt.amount.value,collateral.amount.value);
            boost::rational<int128_t> ratio(STEEMIT_COLLATERAL_RATIO_DENOM, maximum_short_squeeze_ratio);
            auto cp = sp * ratio;

            while (cp.numerator() > STEEMIT_MAX_SHARE_SUPPLY || cp.denominator() > STEEMIT_MAX_SHARE_SUPPLY) {
                cp = boost::rational<int128_t>((cp.numerator() >> 1) + (cp.numerator() & 1),
                                               (cp.denominator() >> 1) + (cp.denominator() & 1));
            }

            return (asset(cp.numerator().convert_to<int64_t>(), settlement_price.base.symbol) /
                    asset(cp.denominator().convert_to<int64_t>(), settlement_price.quote.symbol));
        }

        // compile-time table of powers of 10 using template metaprogramming

        template<int N>
        struct p10 {
            static const int64_t v = 10 * p10<N - 1>::v;
        };

        template<>
        struct p10<0> {
            static const int64_t v = 1;
        };

        const int64_t scaled_precision_lut[19] = {p10<0>::v, p10<1>::v, p10<2>::v, p10<3>::v, p10<4>::v, p10<5>::v,
                                                  p10<6>::v, p10<7>::v, p10<8>::v, p10<9>::v, p10<10>::v, p10<11>::v,
                                                  p10<12>::v, p10<13>::v, p10<14>::v, p10<15>::v, p10<16>::v,
                                                  p10<17>::v, p10<18>::v};
    }
} // steemit::protocol
