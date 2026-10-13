#include "LLVMOptimizationToolkit/Passes.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include <algorithm>
#include <vector>

using namespace llvm;

namespace {

bool isCSEEligible(const Instruction &I) {
  return isa<BinaryOperator>(I) || isa<UnaryOperator>(I) ||
         isa<CmpInst>(I) || isa<CastInst>(I) || isa<SelectInst>(I) ||
         isa<GetElementPtrInst>(I) || isa<ExtractElementInst>(I) ||
         isa<InsertElementInst>(I) || isa<ShuffleVectorInst>(I) ||
         isa<ExtractValueInst>(I) || isa<InsertValueInst>(I);
}

} // namespace

PreservedAnalyses LocalCSEPass::run(Function &F, FunctionAnalysisManager &) {
  bool Changed = false;

  for (BasicBlock &BB : F) {
    std::vector<Instruction *> AvailableExpressions;

    for (auto It = BB.begin(); It != BB.end();) {
      Instruction &I = *It++;
      if (!isCSEEligible(I) || I.getType()->isVoidTy() || I.isTerminator() ||
          I.mayReadOrWriteMemory() || I.mayHaveSideEffects())
        continue;

      auto Match = std::find_if(
          AvailableExpressions.begin(), AvailableExpressions.end(),
          [&I](const Instruction *Candidate) {
            return I.isIdenticalTo(Candidate);
          });

      if (Match == AvailableExpressions.end()) {
        AvailableExpressions.push_back(&I);
        continue;
      }

      I.replaceAllUsesWith(*Match);
      I.eraseFromParent();
      Changed = true;
    }
  }

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
