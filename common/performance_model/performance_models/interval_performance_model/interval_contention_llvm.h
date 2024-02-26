#ifndef __INTERVAL_CONTENTION_LLVM_H
#define __INTERVAL_CONTENTION_LLVM_H

#include "interval_contention.h"
#include "dynamic_micro_op_llvm.h"

class IntervalContentionLLVM : public IntervalContention {
   private:
      const CoreModel *m_core_model;

      uint64_t m_count_byport[8 /*MicroOp::UOP_PORT_SIZE*/];
      uint64_t m_cpContrByPort[DynamicMicroOpLLVM::UOP_PORT_SIZE];

   public:
      IntervalContentionLLVM(const Core *core, const CoreModel *core_model);

      virtual void clearFunctionalUnitStats();
      virtual void addFunctionalUnitStats(const DynamicMicroOp *uop);
      virtual void removeFunctionalUnitStats(const DynamicMicroOp *uop);
      virtual uint64_t getEffectiveCriticalPathLength(uint64_t critical_path_length, bool update_reason);
};

#endif // __INTERVAL_CONTENTION_LLVM_H
