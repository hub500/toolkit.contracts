#include "xchain.proxy/xchain.proxy.hpp"
#include <chrono>
#include <const.hpp>
#include <vector>
#include <string>


const std::string MESSAGE_MAGIC = "Bitcoin Signed Message:\n";
const std::string BIND_MSG = "hello Armonia";
static constexpr eosio::name active_permission{"active"_n};
using namespace amax;
using namespace wasm;

using namespace std;

std::string to_hex( const char* d, uint32_t s ) 
{
    std::string r;
    const char* to_hex="0123456789abcdef";
    uint8_t* c = (uint8_t*)d;
    for( uint32_t i = 0; i < s; ++i )
        (r += to_hex[(c[i]>>4)]) += to_hex[(c[i] &0x0f)];
    return r;
}
std::vector<char> encode(const int val,const int minlen) {
    std::vector<char> result;
    // 0-255 的ASCII码值
    int base = 256;
    std::vector<char> chars(base);
    for (int i = 0; i < 256; ++i) {
        chars[i] = static_cast<char>(i);
    }
    //使用模运算符 % 和除法运算符 /，通过将 val 除以 base 来迭代编码过程。每次迭代，我们都会得到一个余数，该余数对应于 chars 向量中的一个字符
    int value = val;
    while (value > 0) {
        auto curcode = chars[value % base];
        result.insert(result.begin(), curcode);
        value /= base;
    }
    // 检查是否需要用零进行填充以达到最小长度 minlen
    int pad_size = minlen - result.size();
    if (pad_size > 0) {
        for (int i = 0; i < pad_size; i++) {
            result.insert(result.begin(), 0);
        }
    }
    return result;
}

std::vector<char> num_to_var_int(const uint64_t x) {
    std::vector<char> result;
    if (x < 253) {
        result.push_back(x);
    } else if (x < 65536) {
        result.push_back((char)253);
        auto encode_bytes = encode(x, 2);
        reverse(encode_bytes.begin(),encode_bytes.end());
        result.insert(result.end(), encode_bytes.begin(), encode_bytes.end());
    } else if (x < 4294967296) {
        result.push_back((char)254);
        auto encode_bytes = encode(x, 4);
        reverse(encode_bytes.begin(),encode_bytes.end());
        result.insert(result.end(), encode_bytes.begin(), encode_bytes.end());
    } else {
        result.push_back((char)255);
        auto encode_bytes = encode(x, 8);
        reverse(encode_bytes.begin(),encode_bytes.end());
        result.insert(result.end(), encode_bytes.begin(), encode_bytes.end());
    }
    
    return result;
}


eosio::checksum256 sha256sha256(const vector<char> data){

    auto sha_1 = sha256(data.data(),data.size());
    auto byte_array = sha_1.extract_as_byte_array();
    auto chars = reinterpret_cast<const char*>(byte_array.data());
    return sha256(chars,byte_array.size());
}
void proxy::init(const name& admin,const name& submitter){
    require_auth(_self);
    CHECKC( is_account(admin), err::ACCOUNT_INVALID,"admin account invalid");
    CHECKC( is_account(submitter), err::ACCOUNT_INVALID,"submitter account invalid");
    _gstate.admin = admin; 
    _gstate.submitter = submitter;

}
void proxy::activate( const name& account, 
                    const string& btc_pub_key,
                    const eosio::signature& signature,
                    const public_key temp_amc_pub){
    require_auth(_gstate.admin);

    amax_system::newaccount_action act(SYS_CONTRACT, { {get_self(), ACTIVE_PERM} }) ;
    authority owner_auth  = { 1, {}, {{{get_self(), ACTIVE_PERM}, 1}}, {} }; 
    authority active_auth  = { 1, {}, {{{get_self(), ACTIVE_PERM}, 1},{{get_self(), AMAX_CODE_PERM}, 1}}, {} }; 
    act.send( _self, account, owner_auth, active_auth);

    amax_system::buyrambytes_action buy_ram_act(SYS_CONTRACT, { {get_self(), ACTIVE_PERM} });
    buy_ram_act.send( get_self(), account, _gstate.ram_bytes );

    amax_system::delegatebw_action delegatebw_act(SYS_CONTRACT, { {get_self(), ACTIVE_PERM} });
    delegatebw_act.send( get_self(), account,  _gstate.stake_net_quantity,  _gstate.stake_cpu_quantity, false );

    auto keys          = public_key_t::tbl_t( _self, _self.value );
    auto itr           = keys.find(account.value);
    CHECKC( itr == keys.end(), err::RECORD_EXISTING,"account already bound")
    
    keys.emplace( _self, [&]( auto& row ) {
        row.owner = account;
        row.temp_amc_pub = temp_amc_pub;
        row.signature =  signature;
        row.btc_pub_key = btc_pub_key;
        row.status = BindStatus::UNBOUND;
        row.created_at = current_time_point();
        row.updated_at = current_time_point();
    });
}


void proxy::submitaction(const name& account, const vector<char> packed_action,const eosio::signature& sign){
    
    require_auth(account);
    string msg = to_hex(packed_action.data(),packed_action.size());
    // msg_packed_t msg_packed = {MESSAGE_MAGIC,msg};
    // auto last_packed_data = pack(msg_packed);
    
    vector<char> packed = pack(MESSAGE_MAGIC);
    // auto size_bytes = num_to_var_int(msg.size());
    // char_data.insert(char_data.end(),size_bytes.begin(),size_bytes.end());
    vector<char> msg_packed = num_to_var_int(msg.size());;
    for (auto it = msg.begin(); it != msg.end(); ++it) {
        msg_packed.push_back(*it);
    }
    packed.insert(packed.end(),msg_packed.begin(),msg_packed.end());
    // auto t = pack(msg);
    // check(false,to_hex(char_data.data(),char_data.size()));
    auto recover_pub_key =  recover_key( sha256sha256(packed), sign);
    auto keys          = public_key_t::tbl_t( _self, _self.value );
    auto itr            = keys.find(account.value);
    CHECKC( itr != keys.end(), err::RECORD_NOT_FOUND,"[proxy] account not found")
    CHECKC( itr-> recovered_public_key == recover_pub_key, err::DATA_MISMATCH,"Public key mismatch")

    // auto idx             = keys.get_index<"pubkeyidx"_n>();
    // auto hash = pack(recover_pub_key);
    // auto idx_itr           = idx.find(sha256(hash.data(),hash.size()));
    // CHECKC( idx_itr != idx.end(), err::RECORD_NOT_FOUND,"Public key unbound")
    
    unpacked_action_t unpacked_action = unpack<unpacked_action_t>(packed_action.data(),packed_action.size());
    CHECKC( unpacked_action.nonce == to_string(itr -> next_nonce), err::DATA_MISMATCH,"nonce does not match")
    CHECKC( unpacked_action.actions.size() > 0 , err::OVERSIZED,"There are no executable actions")
    for ( auto at : unpacked_action.actions){
        eosio::action send_action;
        send_action.account = at.account;
        send_action.name = at.name;
        send_action.data = at.data;
        send_action.authorization.emplace_back(permission_level{ account, active_permission });
        send_action.send();
    }
    keys.modify( itr,same_payer, [&]( auto& row ) {
        row.next_nonce += 1;
        row.updated_at = current_time_point();
        row.recovered_public_key = recover_pub_key;
    });
}

void proxy::bind(const name& account, 
                    const string& btc_pub_key,
                    const eosio::signature& signature,
                    const public_key temp_amc_pub){
    require_auth(_gstate.admin);
    CHECKC( is_account(account), err::ACCOUNT_INVALID,"account invalid");
    
    authority auth = { 1, {{temp_amc_pub, 1}}, {}, {} };
    amax_system::updateauth_action updateauth_act(SYS_CONTRACT, { {account, OWNER_PERM} });
    updateauth_act.send( account, "submitaction"_n, ACTIVE_PERM, auth);
    amax_system::linkauth_action linkauth_act(SYS_CONTRACT, { {account, ACTIVE_PERM} });
    linkauth_act.send( account, _self, "submitaction"_n, "submitaction"_n);

    auto packed_data = pack(msg_packed_t(MESSAGE_MAGIC,BIND_MSG));
    auto public_key = recover_key(sha256sha256(packed_data),signature);

    auto keys          = public_key_t::tbl_t( _self, _self.value );
    auto itr           = keys.find(account.value);
    CHECKC( itr != keys.end(), err::RECORD_EXISTING,"account not found")

    auto idx             = keys.get_index<"pubkeyidx"_n>();
    auto hash = pack(public_key);
    auto idx_itr           = idx.find(sha256(hash.data(),hash.size()));
    
    CHECKC( idx_itr == idx.end() || idx_itr -> owner == account, err::RECORD_EXISTING,"Public key already bound")

    keys.modify( itr, same_payer, [&]( auto& row ) {
        row.owner = account;
        row.recovered_public_key = public_key;
        row.temp_amc_pub = temp_amc_pub;
        row.signature =  signature;
        row.btc_pub_key = btc_pub_key;
        row.updated_at = current_time_point();
        row.status = BindStatus::BOUND;
    });
}

