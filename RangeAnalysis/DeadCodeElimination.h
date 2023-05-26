#include "llvm/ADT/Statistic.h"
#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"

#include "RangeAnalysis.h"

#define TRUE_BRANCH 0
#define FALSE_BRANCH 1

using namespace llvm;

class DeadCodeElimination : public llvm::FunctionPass {
	private:
		bool solveICmpInstruction(BranchInst*);
		void eliminateCondition(BranchInst*, int, ICmpInst*);

	public:
		static char ID;
		DeadCodeElimination() : FunctionPass(ID) {}
		~DeadCodeElimination() = default;

		bool runOnFunction(Function &F);
		void getAnalysisUsage(AnalysisUsage &AU) const;
};