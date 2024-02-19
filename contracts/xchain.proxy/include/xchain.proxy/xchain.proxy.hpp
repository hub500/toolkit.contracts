#include <set>
#include <chrono>
#include "xchain.proxydb.hpp"
#include "commons/utils.hpp"
#include <const.hpp>

using namespace std;
using namespace wasm::db;
namespace amax {

class [[eosio::contract("xchain.proxy")]] proxy: public eosio::contract {
private:
    global_t::tbl_t     _global;
    global_t            _gstate;
    
public:
    using contract::contract;

    proxy(eosio::name receiver, eosio::name code, datastream<const char*> ds):
        contract(receiver, code, ds)
        ,_global(get_self(), get_self().value)
    {
        _gstate = _global.exists() ? _global.get() : global_t{};
    }
    
    ~proxy() { _global.set( _gstate, get_self() ); }

    ACTION init(const name& admin,const name& submitter);

    ACTION activate(const name& account, 
                    const string& btc_pub_key,
                    const eosio::signature& signature,
                    const public_key temp_amc_pub);

    ACTION bind(const name& account, 
                    const string& btc_pub_key,
                    const eosio::signature& signature,
                    const public_key temp_amc_pub);
    
    ACTION submitaction(const name& account,const vector<char> packed_action,const eosio::signature& sign);

    ACTION proxyaction(const vector<action_t>& actions,const string& nonce){
        check(false,"Execution failed");
    }
    
}; //contract xchain.proxy

}