#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Pass.h"
#include <regex>
#include <system_error>
#include <string>

using namespace llvm;

namespace {
    // Global variable to name all basic blocks.
    // This is not thread safe.
    int BBCounter = 0;

    struct CFGModulePass : public PassInfoMixin<CFGModulePass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
        void nameBBs(Module &M);
        void linkBBs(Module &M);
        static bool isRequired() { return true; }
    };

    void CFGModulePass::nameBBs(Module &M) {
        for (Function &F : M) {
            for (BasicBlock &BB : F) {
                BB.setName("BB"+std::to_string(BBCounter));
                ++BBCounter;
            } 
        }
    }

    void CFGModulePass::linkBBs(Module &M) {
        std::regex pattern("(benchmarks\\/Stanford\\/|\\.c)");
        std::string fileName = std::regex_replace(M.getSourceFileName(), pattern, "");
        fileName += ".dot";

        std::error_code EC;
        raw_fd_ostream File(fileName, EC, sys::fs::OF_Append);
        File << "digraph CFG {\n";
        for (Function &F : M) {
            for (BasicBlock &BB : F) {
                  File << "\t" << BB.getName()
                       << "[shape=record,\n\t\tlabel=\"{" << BB.getName()
                       << ":\\l\\l";
                for (Instruction &I : BB) {
                    File << "\n\t\t\t" << I << "\\l";
                    if (auto* brInst = dyn_cast<BranchInst>(&I)) {
                        File << "}\"];\n";
                        int succs = brInst->getNumSuccessors();
                        for (int i=0; i < succs; ++i) {
                            BasicBlock* targetBB = brInst->getSuccessor(i);
                            File << "\t" << BB.getName()
                                 << " -> " << targetBB->getName() << ";\n";
                        }
                    }
                    if (auto* retInst = dyn_cast<ReturnInst>(&I)){
                        File << "}\"];\n";
                    }
                }
            } 
        }
        File << "}";
        File.close();
    }

    PreservedAnalyses CFGModulePass::run(Module &M,
                                         ModuleAnalysisManager &) {
        nameBBs(M);
        linkBBs(M);
        return PreservedAnalyses::all();
    }
}

char CFG
static RegisterPass<DeadCodeElimination> X(
	"dead-code-elimination",
    "Eliminates dead code based on Range Analysis (github.com/vhscampos/range-analysis)"
);
