#include "LLVMOptimizationToolkit/Passes.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Transforms/Utils/Local.h"

using namespace llvm;

PreservedAnalyses
DeadCodeEliminationPass::run(Function &F, FunctionAnalysisManager &) {
  bool Changed = false;
  bool RemovedInstruction = true;

  while (RemovedInstruction) {
    RemovedInstruction = false;
    for (BasicBlock &BB : F) {
      for (auto It = BB.begin(); It != BB.end();) {
        Instruction &I = *It++;
        if (!isInstructionTriviallyDead(&I))
          continue;

        I.eraseFromParent();
        RemovedInstruction = true;
        Changed = true;
      }
    }
  }

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
