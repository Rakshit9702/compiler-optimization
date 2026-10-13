#include "LLVMOptimizationToolkit/Passes.h"

#include "llvm/Config/llvm-config.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Compiler.h"

#include <utility>

using namespace llvm;

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LLVMOptimizationToolkit",
          LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "toolkit-const-fold") {
                    FPM.addPass(ConstantFoldingPass());
                    return true;
                  }
                  if (Name == "toolkit-local-cse") {
                    FPM.addPass(LocalCSEPass());
                    return true;
                  }
                  if (Name == "toolkit-dce") {
                    FPM.addPass(DeadCodeEliminationPass());
                    return true;
                  }
                  return false;
                });

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name != "llvm-opt-toolkit")
                    return false;

                  FunctionPassManager FPM;
                  FPM.addPass(ConstantFoldingPass());
                  FPM.addPass(LocalCSEPass());
                  FPM.addPass(DeadCodeEliminationPass());
                  MPM.addPass(
                      createModuleToFunctionPassAdaptor(std::move(FPM)));
                  return true;
                });
          }};
}
