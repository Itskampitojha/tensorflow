/* Copyright 2023 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef XLA_SERVICE_GPU_MODEL_COALESCING_ANALYSIS_H_
#define XLA_SERVICE_GPU_MODEL_COALESCING_ANALYSIS_H_

#include "absl/container/flat_hash_map.h"
#include "mlir/IR/MLIRContext.h"  // from @llvm-project
#include "xla/hlo/ir/hlo_instruction.h"
#include "xla/service/gpu/fusions/fusion_emitter.h"
#include "xla/service/gpu/hlo_fusion_analysis.h"

namespace xla {
namespace gpu {

// Computes read coalescing for operands of an instruction or a
// producer-consumer fusion.
// Note, that later, after we migrate away from using the heuristic, we might
// want to use HloFusionAdaptor instead of having two different constructors.
class CoalescingAnalysis {
 public:
  // Computes read coalescing for operands of `instr`.
  CoalescingAnalysis(const HloInstruction* instr,
                     HloFusionAnalysis::EmitterFusionKind fusion_kind,
                     KernelFusionInterface* fusion_interface = nullptr,
                     mlir::MLIRContext* mlir_context = nullptr,
                     bool use_heuristic = true);

  // Computes read coalescing for operands of fused `producer` and `consumer`.
  CoalescingAnalysis(const HloInstruction* producer,
                     const HloInstruction* consumer,
                     HloFusionAnalysis::EmitterFusionKind fusion_kind,
                     KernelFusionInterface* fusion_interface = nullptr,
                     mlir::MLIRContext* mlir_context = nullptr,
                     bool use_heuristic = true);

  // Returns true if the operand is read coalesced.
  bool IsReadCoalesced(const HloInstruction* operand) const;

 private:
  bool ComputeCoalescingForAllOperands(
      const HloInstruction* instr, const HloInstruction* optional_producer,
      HloFusionAnalysis::EmitterFusionKind fusion_kind,
      KernelFusionInterface* fusion_interface, mlir::MLIRContext* mlir_context);

  absl::flat_hash_map<const HloInstruction*, bool> coalescing_per_operand_;
  bool is_coalesced_computed_by_heuristic_ = false;
};

// Returns true if all input reads are coalesced. If consumer is not nullptr,
// producer and consumer are considered as one fusion, otherwise it's only the
// producer.
bool IsReadCoalescedHeuristic(HloFusionAnalysis::EmitterFusionKind fusion_kind,
                              const HloInstruction* producer,
                              const HloInstruction* consumer = nullptr);

}  // namespace gpu
}  // namespace xla

#endif  // XLA_SERVICE_GPU_MODEL_COALESCING_ANALYSIS_H_
