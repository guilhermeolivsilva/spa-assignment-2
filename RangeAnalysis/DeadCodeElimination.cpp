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

using namespace llvm;

namespace {
class DeadCodeElimination : public llvm::FunctionPass {
	private:
		bool solveICmpInstruction(ICmpInst* I) {
			InterProceduralRA < Cousot >* ra = &getAnalysis < InterProceduralRA < Cousot > >();
			Range range1 = ra->getRange(I->getOperand(0));
			Range range2 = ra->getRange(I->getOperand(1));
            errs() << range1.getLower() << ' ';
            errs() << range2.getLower() << '\n';
			switch (I->getPredicate()) {
				case CmpInst::ICMP_SLT:
					if (range1.getUpper().slt(range2.getLower())) {
						errs() << "hello from solveCmpInstruction";
						llvm::outs() << I << '\n';
					}
					break;
				default:
					break;
			}

			return true;
		}

	public:
		static char ID;
		DeadCodeElimination() : FunctionPass(ID) {}
		~DeadCodeElimination() = default;

		virtual bool runOnFunction(Function &F) {
			errs() << "hello from runOnFunction\n";
            llvm::ICmpInst* cmp;
            for(Function::iterator bb = F.begin(), bbEnd = F.end();
                bb != bbEnd; ++bb) {
                for(BasicBlock::iterator I = bb->begin(), IEnd = bb->end(); 
                    I != IEnd; ++I) {
                    // if I is a conditional
                    cmp = llvm::dyn_cast<llvm::ICmpInst>(&*I);
                    if(cmp) {
                        solveICmpInstruction(cmp);
                    } 
                }
            }

			return true;
		}

		virtual void getAnalysisUsage(AnalysisUsage &AU) const {
			AU.setPreservesAll();
			AU.addRequired<InterProceduralRA<Cousot>>();
		}
};
}

char DeadCodeElimination::ID = 0;
static RegisterPass<DeadCodeElimination> X(
	"dead-code-elimination",
    "Eliminates dead code based on Range Analysis (github.com/vhscampos/range-analysis)"
);
