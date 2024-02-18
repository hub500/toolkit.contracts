#pragma once

#include <eosio/crypto.hpp>
#include <eosio/datastream.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/permission.hpp>
#include <eosio/privileged.hpp>
#include <eosio/serialize.hpp>
#include <eosio/singleton.hpp>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/privileged.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>
#include <eosio/binary_extension.hpp>
#include <cmath>
#include "commons/utils.hpp"
#include <map>
#include <optional>
#include <set>
#include <chrono>
#include <algorithm>
#include <string>
#include <type_traits>
#include "amax_system.hpp"
using namespace eosio;
using namespace std;
using std::string;

// using namespace wasm;

enum class err: uint8_t {
   NONE                 = 0,
   RECORD_NOT_FOUND     = 1,
   RECORD_EXISTING      = 2,
   SYMBOL_MISMATCH      = 4,
   PARAM_ERROR          = 5,
   MEMO_FORMAT_ERROR    = 6,
   PAUSED               = 7,
   NO_AUTH              = 8,
   NOT_POSITIVE         = 9,
   NOT_STARTED          = 10,
   OVERSIZED            = 11,
   TIME_EXPIRED         = 12,
   NOTIFY_UNRELATED     = 13,
   ACTION_REDUNDANT     = 14,
   ACCOUNT_INVALID      = 15,
   FEE_INSUFFICIENT     = 16,
   FIRST_CREATOR        = 17,
   STATUS_ERROR         = 18,
   RATE_OVERLOAD        = 19,
   DATA_MISMATCH        = 20,
   MISC                 = 255,

   DEX_ORDER_ERROR      = 21,
   POOL_ORDER_ERROR     = 22,
   INSUFFICIENT_QUOTA   = 23,
   LOWER_LIMIT_EXCEEDED = 24,
   UPPER_LIMIT_EXCEEDED = 25,
   TIMES_EXCEEDED        = 26
};


namespace wasm { namespace db {

#define PROXY_TBL struct [[eosio::table, eosio::contract("xchain.proxy")]]
#define PROXY_TBL_NAME(name) struct [[eosio::table(name), eosio::contract("xchain.proxy")]]
#define HASH256(str) sha256(const_cast<char*>(str.c_str()), str.size())


namespace BindStatus {
    static constexpr eosio::name UNBOUND    { "unbound"_n     };
    static constexpr eosio::name BOUND      { "bound"_n     };
}
PROXY_TBL_NAME("global") global_t {
   name              admin;
   name              submitter;
   uint64_t    ram_bytes               = 5000;
   asset       stake_net_quantity      = asset(10000, SYS_SYMB); //TODO: set params in global table
   asset       stake_cpu_quantity      = asset(40000, SYS_SYMB);
   typedef eosio::singleton< "global"_n, global_t > tbl_t;
};

struct [[eosio::action]] action_t{
   name           account;
   name           name;
   vector<char>   data; 
   EOSLIB_SERIALIZE(action_t,(account)(name)(data))
};


struct [[eosio::action]] unpacked_action_t{
   vector<action_t>          actions;
   string                           nonce;
   EOSLIB_SERIALIZE(unpacked_action_t,(actions)(nonce))
};

struct [[eosio::action]] msg_packed_t{
   string                  message_magic;
   string                  msg;

   msg_packed_t() {};
   msg_packed_t( const string& s1, const string& s2): message_magic(s1),msg(s2) {};

   EOSLIB_SERIALIZE(msg_packed_t,  (message_magic)(msg))
};
// _self
PROXY_TBL public_key_t{
   name                 owner;
   public_key           recovered_public_key;
   public_key           temp_amc_pub;
   string               btc_pub_key;
   eosio::signature     signature;
   uint64_t             next_nonce = 1;
   name                 status;
   time_point_sec       created_at;
   time_point_sec       updated_at;

   uint64_t primary_key() const { return owner.value;}
   checksum256 by_public_key() const { 
      auto packed_data = pack(recovered_public_key); 
      return sha256(packed_data.data(),packed_data.size());
   };
   EOSLIB_SERIALIZE(public_key_t,  (owner)(recovered_public_key)(temp_amc_pub)(btc_pub_key)(signature)(next_nonce)(status)(created_at)(updated_at))
   typedef eosio::multi_index
   < "publickeys"_n,  public_key_t,
   indexed_by<"pubkeyidx"_n,     const_mem_fun<public_key_t, checksum256, &public_key_t::by_public_key> >
   > tbl_t; 
};

}}