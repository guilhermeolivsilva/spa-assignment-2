
#include "DeadCodeElimination.h"

#define STATS
#define DEBUG_TYPE "DeadCodeElimination"

STATISTIC(EliminatedBBs, "Total amount of thoroughly eliminated Basic Blocks.");
STATISTIC(EliminatedInstructions, "Total amount of eliminated individual Instructions.");

using namespace llvm;


bool DeadCodeElimination::solveICmpInstruction(BranchInst* branchInst) {
	bool codeHasBeenEliminated = false;
	ICmpInst* I = dyn_cast<ICmpInst>(branchInst->getCondition());

	InterProceduralRA < Cousot >* ra = &getAnalysis < InterProceduralRA < Cousot > >();
	Range range1 = ra->getRange(I->getOperand(0));
	Range range2 = ra->getRange(I->getOperand(1));

	switch (I->getPredicate()) {
		// if(x < y)
		case CmpInst::ICMP_SLT:
			if (range1.getUpper().slt(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().sge(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x < y) (unsigned)
		case CmpInst::ICMP_ULT:
			if (range1.getUpper().ult(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().uge(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x <= y)
		case CmpInst::ICMP_SLE:
			if (range1.getUpper().sle(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().sgt(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x <= y) (unsigned)
		case CmpInst::ICMP_ULE:
			if (range1.getUpper().ule(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().ugt(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x > y)
		case CmpInst::ICMP_SGT:
			if (range1.getUpper().sgt(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().sle(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x > y) (unsigned)
		case CmpInst::ICMP_UGT:
			if (range1.getUpper().ugt(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().ule(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x >= y)
		case CmpInst::ICMP_SGE:
			if (range1.getUpper().sge(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().slt(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x >= y) (unsigned)
		case CmpInst::ICMP_UGE:
			if (range1.getUpper().uge(range2.getLower())) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
				codeHasBeenEliminated = true;
			} else if(range1.getLower().ult(range2.getUpper())) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
				codeHasBeenEliminated = true;
			}
			break;

		// if(x == y)
		case CmpInst::ICMP_EQ:
			// Range class implements == operator. It's true if both bounds
			// of the objects is exactly the same.
			if (range1 == range2) {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
			} else {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
			}
			codeHasBeenEliminated = true;
			break;

		// if(x != y)
		case CmpInst::ICMP_NE:
			if (range1 == range2) {
				eliminateCondition(branchInst, FALSE_BRANCH, I);
			} else {
				eliminateCondition(branchInst, TRUE_BRANCH, I);
			}
			codeHasBeenEliminated = true;
			break;

		default:
			break;
	}

	return codeHasBeenEliminated;
}

void DeadCodeElimination::eliminateCondition(BranchInst* branchInst, int successor, ICmpInst* condition) {
	BranchInst* replacementBranch = BranchInst::Create(branchInst->getSuccessor(successor));
	ReplaceInstWithInst(branchInst, replacementBranch);
	RecursivelyDeleteTriviallyDeadInstructions(condition);
}

bool DeadCodeElimination::runOnFunction(Function &F) {
	BranchInst* branchInst;
	bool localCmpInstructionEliminatedCode, globalCmpInstructionEliminatedCode;
	globalCmpInstructionEliminatedCode = false;

	EliminatedBBs = F.size();
	EliminatedInstructions = 0;

	for (BasicBlock &bb : F) {
		localCmpInstructionEliminatedCode = false;
		branchInst = dyn_cast<BranchInst>(--(bb.end()));

		if(branchInst && branchInst->isConditional()) {
			localCmpInstructionEliminatedCode = solveICmpInstruction(branchInst);
		}

		if(localCmpInstructionEliminatedCode) {
			globalCmpInstructionEliminatedCode = true;
			EliminatedInstructions += bb.size();
		}
	}

	// Eliminate blocks affected by eliminateCondition
	removeUnreachableBlocks(F);
	EliminatedBBs -= F.size();

	return globalCmpInstructionEliminatedCode;
}

void DeadCodeElimination::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
	AU.addRequired<InterProceduralRA<Cousot>>();
}


char DeadCodeElimination::ID = 0;
static RegisterPass<DeadCodeElimination> X(
	"dead-code-elimination",
    "Eliminates dead code based on Range Analysis (github.com/vhscampos/range-analysis)"
);
