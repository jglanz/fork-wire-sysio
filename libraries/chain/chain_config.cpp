#include <sysio/chain/chain_config.hpp>
#include <sysio/chain/exceptions.hpp>
#include <sysio/chain/controller.hpp>

namespace sysio { namespace chain {

   void chain_config_v0::validate() const {
      SYS_ASSERT( target_block_net_usage_pct <= config::percent_100, action_validate_exception,
                  "target block net usage percentage cannot exceed 100%" );
      SYS_ASSERT( target_block_net_usage_pct >= config::percent_1/10, action_validate_exception,
                  "target block net usage percentage must be at least 0.1%" );
      SYS_ASSERT( target_block_cpu_usage_pct <= config::percent_100, action_validate_exception,
                  "target block cpu usage percentage cannot exceed 100%" );
      SYS_ASSERT( target_block_cpu_usage_pct >= config::percent_1/10, action_validate_exception,
                  "target block cpu usage percentage must be at least 0.1%" );

      SYS_ASSERT( max_transaction_net_usage < max_block_net_usage, action_validate_exception,
                  "max transaction net usage must be less than max block net usage" );
      SYS_ASSERT( max_transaction_cpu_usage < max_block_cpu_usage, action_validate_exception,
                  "max transaction cpu usage must be less than max block cpu usage" );

      SYS_ASSERT( base_per_transaction_net_usage < max_transaction_net_usage, action_validate_exception,
                  "base net usage per transaction must be less than the max transaction net usage" );
      SYS_ASSERT( (max_transaction_net_usage - base_per_transaction_net_usage) >= config::min_net_usage_delta_between_base_and_max_for_trx,
                  action_validate_exception,
                  "max transaction net usage must be at least ${delta} bytes larger than base net usage per transaction",
                  ("delta", config::min_net_usage_delta_between_base_and_max_for_trx) );
      SYS_ASSERT( context_free_discount_net_usage_den > 0, action_validate_exception,
                  "net usage discount ratio for context free data cannot have a 0 denominator" );
      SYS_ASSERT( context_free_discount_net_usage_num <= context_free_discount_net_usage_den, action_validate_exception,
                  "net usage discount ratio for context free data cannot exceed 1" );

      SYS_ASSERT( min_transaction_cpu_usage <= max_transaction_cpu_usage, action_validate_exception,
                  "min transaction cpu usage cannot exceed max transaction cpu usage" );
      SYS_ASSERT( max_transaction_cpu_usage < (max_block_cpu_usage - min_transaction_cpu_usage), action_validate_exception,
                  "max transaction cpu usage must be at less than the difference between the max block cpu usage and the min transaction cpu usage" );

      SYS_ASSERT( 1 <= max_authority_depth, action_validate_exception,
                  "max authority depth should be at least 1" );
}

void chain_config_v1::validate() const {
   chain_config_v0::validate();
   SYS_ASSERT( max_action_return_value_size <= MAX_SIZE_OF_BYTE_ARRAYS, action_validate_exception,
               "max action return value size should be less than MAX_SIZE_OF_BYTE_ARRAYS" );
}

bool config_entry_validator::operator()(uint32_t id) const {
   bool allowed = true;
   switch(id){
      case chain_config_v1::max_action_return_value_size_id:
      {
         allowed = control.is_builtin_activated(builtin_protocol_feature_t::action_return_value);
         if (!allowed){
            wlog("action_return_value protocol feature is not active, max_action_return_value_size config is not allowed");
         }
      }
      break;
   }

   return allowed;
}

} } // namespace sysio::chain
