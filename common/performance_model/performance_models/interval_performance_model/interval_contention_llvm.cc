#include "interval_contention_llvm.h"
#include "core.h"
#include "stats.h"

IntervalContentionLLVM::IntervalContentionLLVM(const Core *core, const CoreModel *core_model)
   : m_core_model(core_model)
{
   for(unsigned int i = 0; i < DynamicMicroOpLLVM::UOP_PORT_SIZE; ++i)
   {
      m_cpContrByPort[i] = 0;
      String name = String("cpContr_") + DynamicMicroOpLLVM::PortTypeString((DynamicMicroOpLLVM::uop_port_t)i);
      registerStatsMetric("interval_timer", core->getId(), name, &(m_cpContrByPort[i]));
   }
}

void IntervalContentionLLVM::clearFunctionalUnitStats()
{
   for(unsigned int i = 0; i < (unsigned int)DynamicMicroOpLLVM::UOP_PORT_SIZE; ++i)
   {
      m_count_byport[i] = 0;
   }
}

void IntervalContentionLLVM::addFunctionalUnitStats(const DynamicMicroOp *uop)
{
   m_count_byport[uop->getCoreSpecificInfo<DynamicMicroOpLLVM>()->getPort()]++;
}

void IntervalContentionLLVM::removeFunctionalUnitStats(const DynamicMicroOp *uop)
{
   m_count_byport[uop->getCoreSpecificInfo<DynamicMicroOpLLVM>()->getPort()]--;
}

uint64_t IntervalContentionLLVM::getEffectiveCriticalPathLength(uint64_t critical_path_length, bool update_reason)
{
   DynamicMicroOpLLVM::uop_port_t reason = DynamicMicroOpLLVM::UOP_PORT_SIZE;
   uint64_t effective_cp_length = critical_path_length;

   // For the standard ports, check if we have exceeded our execution limit
   for (unsigned int i = 0 ; i < DynamicMicroOpLLVM::UOP_PORT_SIZE ; i++)
   {
      // Skip shared ports
      if (i != DynamicMicroOpLLVM::UOP_ALL && effective_cp_length < m_count_byport[i]
      )
      {
         effective_cp_length = m_count_byport[i];
         reason = (DynamicMicroOpLLVM::uop_port_t)i;
      }
   }
   // Check shared port usage
   uint64_t shared_use = m_count_byport[DynamicMicroOpLLVM::UOP_INT] + m_count_byport[DynamicMicroOpLLVM::UOP_FLOAT]
                        + m_count_byport[DynamicMicroOpLLVM::UOP_MEM]
                        + m_count_byport[DynamicMicroOpLLVM::UOP_ALL];
   if (shared_use > (3*effective_cp_length))
   {
      effective_cp_length = (shared_use+2) / 3; // +3 to round up to the next cycle
      reason = DynamicMicroOpLLVM::UOP_ALL;
   }

   if (update_reason && effective_cp_length > critical_path_length)
   {
      LOG_ASSERT_ERROR(reason != DynamicMicroOpLLVM::UOP_PORT_SIZE, "There should be a reason for the cp extention, but there isn't");
      m_cpContrByPort[reason] += 1000000 * (effective_cp_length - critical_path_length) / effective_cp_length;
   }

   return effective_cp_length;
}
