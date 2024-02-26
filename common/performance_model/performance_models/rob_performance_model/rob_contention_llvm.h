#ifndef __ROB_CONTENTION_LLVM_H
#define __ROB_CONTENTION_LLVM_H

#include "rob_contention.h"
#include "contention_model.h"
#include "core_model_llvm.h"
#include "dynamic_micro_op_llvm.h"

#include <vector>

class RobContentionLLVM : public RobContention {
   private:
      const CoreModel *m_core_model;
      uint64_t m_cache_block_mask;
      ComponentTime m_now;

      // port contention
      bool ports[DynamicMicroOpLLVM::UOP_PORT_SIZE];
      int ports_all;

      std::vector<SubsecondTime> fu_used_until;

   public:
      RobContentionLLVM(const Core *core, const CoreModel *core_model);

      void initCycle(SubsecondTime now);
      bool tryIssue(const DynamicMicroOp &uop);
      bool noMore();
      void doIssue(DynamicMicroOp &uop);
};

#endif // __ROB_CONTENTION_BOOM_V1_H
