#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
#define SYMBOL(sym_code, precision) symbol(symbol_code(sym_code), precision)
// #define TEST_ENVIRONMENT = 1

#ifdef TEST_ENVIRONMENT
static constexpr uint32_t SECONDS_PER_DAY       = 600;
static constexpr name      BOOSTER_ACCOUNT      { "ido.create"_n };
#else
static constexpr uint32_t SECONDS_PER_DAY       = 86400;
static constexpr name      BOOSTER_ACCOUNT      { "amax2booster"_n };
#endif

static constexpr symbol    MUSDT_SYMBOL       = symbol(symbol_code("MUSDT"), 6);
static constexpr name      MTOKEN               { "amax.mtoken"_n };
static constexpr name      SPLIT_ACCOUNT        { "amax.split"_n };


static constexpr uint32_t  MAX_TITLE_SIZE        = 256;
static constexpr int128_t  RATIO_PRECISION       = 10000;
static constexpr eosio::name SYS_CONTRACT       = "amax"_n;
static constexpr eosio::name OWNER_PERM         = "owner"_n;
static constexpr eosio::name ACTIVE_PERM        = "active"_n;
static constexpr eosio::name AMAX_CODE_PERM        = "amax.code"_n;
static constexpr eosio::symbol SYS_SYMB         = SYMBOL("AMAX", 8);