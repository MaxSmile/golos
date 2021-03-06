#pragma once

#include <steemit/protocol/asset.hpp>
#include <steemit/protocol/base.hpp>
#include <steemit/protocol/block_header.hpp>

#include <fc/utf8.hpp>

namespace steemit {
    namespace protocol {

        struct author_reward_operation : public virtual_operation {
            author_reward_operation() {
            }

            author_reward_operation(const account_name_type &a, const string &p, const asset &s, const asset &st, const asset &v)
                    : author(a), permlink(p), sbd_payout(s), steem_payout(st),
                      vesting_payout(v) {
            }

            account_name_type author;
            string permlink;
            asset sbd_payout;
            asset steem_payout;
            asset vesting_payout;
        };


        struct curation_reward_operation : public virtual_operation {
            curation_reward_operation() {
            }

            curation_reward_operation(const string &c, const asset &r, const string &a, const string &p)
                    : curator(c), reward(r), comment_author(a),
                      comment_permlink(p) {
            }

            account_name_type curator;
            asset reward;
            account_name_type comment_author;
            string comment_permlink;
        };


        struct comment_reward_operation : public virtual_operation {
            comment_reward_operation() {
            }

            comment_reward_operation(const account_name_type &a, const string &pl, const asset &p)
                    : author(a), permlink(pl), payout(p) {
            }

            account_name_type author;
            string permlink;
            asset payout;
        };


        struct liquidity_reward_operation : public virtual_operation {
            liquidity_reward_operation(string o = string(), asset p = asset())
                    : owner(o), payout(p) {
            }

            account_name_type owner;
            asset payout;
        };


        struct interest_operation : public virtual_operation {
            interest_operation(const string &o = "", const asset &i = asset(0, SBD_SYMBOL))
                    : owner(o), interest(i) {
            }

            account_name_type owner;
            asset interest;
        };


        struct fill_convert_request_operation : public virtual_operation {
            fill_convert_request_operation() {
            }

            fill_convert_request_operation(const string &o, const uint32_t id, const asset &in, const asset &out)
                    : owner(o), requestid(id), amount_in(in), amount_out(out) {
            }

            account_name_type owner;
            uint32_t requestid = 0;
            asset amount_in;
            asset amount_out;
        };


        struct fill_vesting_withdraw_operation : public virtual_operation {
            fill_vesting_withdraw_operation() {
            }

            fill_vesting_withdraw_operation(const string &f, const string &t, const asset &w, const asset &d)
                    : from_account(f), to_account(t), withdrawn(w),
                      deposited(d) {
            }

            account_name_type from_account;
            account_name_type to_account;
            asset withdrawn;
            asset deposited;
        };


        struct shutdown_witness_operation : public virtual_operation {
            shutdown_witness_operation() {
            }

            shutdown_witness_operation(const string &o) : owner(o) {
            }

            account_name_type owner;
        };

        struct fill_transfer_from_savings_operation : public virtual_operation {
            fill_transfer_from_savings_operation() {
            }

            fill_transfer_from_savings_operation(const account_name_type &f, const account_name_type &t, const asset &a, const uint32_t r, const string &m)
                    : from(f), to(t), amount(a), request_id(r), memo(m) {
            }

            account_name_type from;
            account_name_type to;
            asset amount;
            uint32_t request_id = 0;
            string memo;
        };

        struct hardfork_operation : public virtual_operation {
            hardfork_operation() {
            }

            hardfork_operation(uint32_t hf_id) : hardfork_id(hf_id) {
            }

            uint32_t hardfork_id = 0;
        };

        struct comment_payout_update_operation : public virtual_operation {
            comment_payout_update_operation() {
            }

            comment_payout_update_operation(const account_name_type &a, const string &p)
                    : author(a), permlink(p) {
            }

            account_name_type author;
            string permlink;
        };

        struct comment_benefactor_reward_operation : public virtual_operation {
            comment_benefactor_reward_operation() {
            }

            comment_benefactor_reward_operation(const account_name_type &b, const account_name_type &a, const string &p, const asset &r)
                    : benefactor(b), author(a), permlink(p), reward(r) {
            }

            account_name_type benefactor;
            account_name_type author;
            string permlink;
            asset reward;
        };

        struct return_vesting_delegation_operation : public virtual_operation {
            return_vesting_delegation_operation() {
            }

            return_vesting_delegation_operation(const account_name_type &a, const asset &v)
                    : account(a), vesting_shares(v) {
            }

            account_name_type account;
            asset vesting_shares;
        };
    }
} //steemit::protocol

FC_REFLECT(steemit::protocol::author_reward_operation, (author)(permlink)(sbd_payout)(steem_payout)(vesting_payout))
FC_REFLECT(steemit::protocol::curation_reward_operation, (curator)(reward)(comment_author)(comment_permlink))
FC_REFLECT(steemit::protocol::comment_reward_operation, (author)(permlink)(payout))
FC_REFLECT(steemit::protocol::fill_convert_request_operation, (owner)(requestid)(amount_in)(amount_out))
FC_REFLECT(steemit::protocol::liquidity_reward_operation, (owner)(payout))
FC_REFLECT(steemit::protocol::interest_operation, (owner)(interest))
FC_REFLECT(steemit::protocol::fill_vesting_withdraw_operation, (from_account)(to_account)(withdrawn)(deposited))
FC_REFLECT(steemit::protocol::shutdown_witness_operation, (owner))
FC_REFLECT(steemit::protocol::fill_transfer_from_savings_operation, (from)(to)(amount)(request_id)(memo))
FC_REFLECT(steemit::protocol::hardfork_operation, (hardfork_id))
FC_REFLECT(steemit::protocol::comment_payout_update_operation, (author)(permlink))
FC_REFLECT(steemit::protocol::comment_benefactor_reward_operation, (benefactor)(author)(permlink)(reward))
FC_REFLECT(steemit::protocol::return_vesting_delegation_operation, (account)(vesting_shares))