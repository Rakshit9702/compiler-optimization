#ifndef LLVM_OPTIMIZATION_TOOLKIT_PASSES_H
#define LLVM_OPTIMIZATION_TOOLKIT_PASSES_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class ConstantFoldingPass : public PassInfoMixin<ConstantFoldingPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

class DeadCodeEliminationPass
    : public PassInfoMixin<DeadCodeEliminationPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

class LocalCSEPass : public PassInfoMixin<LocalCSEPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif
