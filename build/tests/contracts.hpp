#pragma once
#include <eosio/testing/tester.hpp>

namespace eosio { namespace testing {

struct contracts {
   
   static std::vector<uint8_t> recover_wasm() { return read_wasm("/Users/richardchen/work/wasm.contracts/toolkit.contracts/build/tests/../contracts/realme.dao/realme.dao.wasm"); }
   static std::vector<char>    recover_abi() { return read_abi("/Users/richardchen/work/wasm.contracts/toolkit.contracts/build/tests/../contracts/realme.dao/realme.dao.abi"); }

   static std::vector<uint8_t> proxy_wasm() { return read_wasm("/Users/richardchen/work/wasm.contracts/toolkit.contracts/build/tests/../contracts/realme.owner/realme.owner.wasm"); }
   static std::vector<char>    proxy_abi() { return read_abi("/Users/richardchen/work/wasm.contracts/toolkit.contracts/build/tests/../contracts/realme.owner/realme.owner.abi"); }

   static std::vector<uint8_t> auth_wasm() { return read_wasm("/Users/richardchen/work/wasm.contracts/toolkit.contracts/build/tests/../contracts/realme.auth/realme.auth.wasm"); }
   static std::vector<char>    auth_abi() { return read_abi("/Users/richardchen/work/wasm.contracts/toolkit.contracts/build/tests/../contracts/realme.auth/realme.auth.abi"); }

};
}} //ns eosio::testing
