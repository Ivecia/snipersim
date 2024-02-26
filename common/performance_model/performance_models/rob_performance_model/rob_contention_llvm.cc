#include "rob_contention_llvm.h"
#include "core_model.h"
#include "dynamic_micro_op.h"
#include "core.h"
#include "config.hpp"
#include "simulator.h"
#include "memory_manager_base.h"

RobContentionLLVM::RobContentionLLVM(const Core *core, const CoreModel *core_model)
   : m_core_model(core_model)
   , m_cache_block_mask(~(core->getMemoryManager()->getCacheBlockSize() - 1))
   , m_now(core->getDvfsDomain())
   , fu_used_until(DynamicMicroOpLLVM::UOP_FU_SIZE, SubsecondTime::Zero())
{
}

void RobContentionLLVM::initCycle(SubsecondTime now)
{
   m_now.setElapsedTime(now);
   memset(ports, 0, sizeof(bool) * DynamicMicroOpLLVM::UOP_PORT_SIZE);
}

bool RobContentionLLVM::tryIssue(const DynamicMicroOp &uop)
{
   std::cerr << "trying " << std::endl;
   // Port contention
   // TODO: Maybe the scheduler is more intelligent and doesn't just assing the first uop in the ROB
   //       that fits a particular free port. We could give precedence to uops that have dependants, etc.
   // NOTE: mixes canIssue and doIssue, in the sense that ports* are incremented immediately.
   //       This works as long as, if we return true, this microop is indeed issued

   const DynamicMicroOpLLVM *core_uop_info = uop.getCoreSpecificInfo<DynamicMicroOpLLVM>();
   DynamicMicroOpLLVM::uop_port_t uop_port = core_uop_info->getPort();

   if (uop_port == DynamicMicroOpLLVM::UOP_ALL) {
      if (ports_all >= 3) {
         return false;
      } else {
         ports_all++;
      }
   } else {
      if (ports[uop_port]) {
         return false;
      } else if (ports_all >= 3) {
         return false;
      } else {
         ports[uop_port] = true;
         ports_all++;
      }
   }

   // Functional Unit contention
   if (DynamicMicroOpLLVM::uop_fu_t fu = core_uop_info->getFU())
   {
      if (fu && fu_used_until[fu] > m_now)
         return false;
   }

   return true;
}

void RobContentionLLVM::doIssue(DynamicMicroOp &uop)
{
   const DynamicMicroOpLLVM *core_uop_info = uop.getCoreSpecificInfo<DynamicMicroOpLLVM>();
   DynamicMicroOpLLVM::uop_fu_t fu = core_uop_info->getFU();
   if (fu)
      fu_used_until[fu] = m_now + m_core_model->getAluLatency(uop.getMicroOp());
}

bool RobContentionLLVM::noMore()
{
   // When we issued something to all ports in this cycle, stop walking the rest of the ROB
   return ports_all >= 3;
}
