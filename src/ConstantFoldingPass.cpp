#include "LLVMOptimizationToolkit/Passes.h"

#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

PreservedAnalyses ConstantFoldingPass::run(Function &F,
                                           FunctionAnalysisManager &) {
  bool Changed = false;
  const DataLayout &DL = F.getParent()->getDataLayout();

  for (BasicBlock &BB : F) {
    for (auto It = BB.begin(); It != BB.end();) {
      Instruction &I = *It++;
      if (I.getType()->isVoidTy())
        continue;

      if (Constant *Folded = ConstantFoldInstruction(&I, DL)) {
        I.replaceAllUsesWith(Folded);
        I.eraseFromParent();
        Changed = true;
      }
    }
  }

  return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
