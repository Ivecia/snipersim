#ifndef __DYNAMIC_MICRO_OP_LLVM_H
#define __DYNAMIC_MICRO_OP_LLVM_H

#include "dynamic_micro_op.h"

class MicroOp;

class DynamicMicroOpLLVM : public DynamicMicroOp
{
   public:
      enum uop_port_t {
         UOP_NONE = 0,
         UOP_INT,
         UOP_FLOAT,
         UOP_MEM,
         UOP_ALL,
         UOP_PORT_SIZE,
      };
      uop_port_t uop_port;

      enum uop_fu_t {
         UOP_FU_NONE = 0,
         UOP_FU_ALU,          // add, minus, bit operation
         UOP_FU_MDU,          // mult & div
         UOP_FU_FALU,         // float add & minus
         UOP_FU_FMDU,         // float mult & div
         UOP_FU_LSU,          // load & store
         UOP_FU_SIZE,
      };
      uop_fu_t uop_fu;

      static uop_port_t getPort(const MicroOp *uop);
      static uop_fu_t getFU(const MicroOp *uop);

      virtual const char* getType() const;

      DynamicMicroOpLLVM(const MicroOp *uop, const CoreModel *core_model, ComponentPeriod period);

      uop_port_t getPort(void) const { return uop_port; }
      uop_fu_t getFU(void) const { return uop_fu; }

      static const char * PortTypeString(DynamicMicroOpLLVM::uop_port_t port);
};

#endif // __DYNAMIC_MICRO_OP_LLVM_H
