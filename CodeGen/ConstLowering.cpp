#include "rhine/Context.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Constant *ConstantInt::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantFP::get(LLTy, getVal());
}

llvm::Constant *Prototype::toLL(llvm::Module *M) {
  return Externals::get(getContext())->getMappingVal(Name, M);
}

llvm::Constant *Function::toLL(llvm::Module *M) {
  auto K = getContext();
  auto RhFnTy = cast<FunctionType>(getType());
  auto FnTy = cast<llvm::FunctionType>(RhFnTy->toLL(M));
  if (auto FunctionCandidate =
      dyn_cast<llvm::Function>(M->getOrInsertFunction(Name, FnTy)))
    K->CurrentFunction = FunctionCandidate;
  else {
    K->DiagPrinter->errorReport(
        SourceLoc, Name + " was declared with different signature earlier");
    exit(1);
  }
  K->CurrentFunction->setGC("rhgc");

  // Bind argument symbols to function argument values in symbol table
  auto ArgList = getArguments();
  auto S = ArgList.begin();
  for (auto &Arg : K->CurrentFunction->args()) {
    K->Map.add(*S, nullptr, &Arg);
    ++S;
  }

  // Add function symbol to symbol table
  K->Map.add(this, nullptr, K->CurrentFunction);

  llvm::BasicBlock *BB =
    llvm::BasicBlock::Create(K->Builder->getContext(),
                             "entry", K->CurrentFunction);
  K->Builder->SetInsertPoint(BB);
  auto Block = getVal();
  Block->toLL(M);
  if (!isa<ReturnInst>(Block->back()))
    K->Builder->CreateRet(nullptr);
  return K->CurrentFunction;
}

llvm::Value *Pointer::toLL(llvm::Module *M) {
  auto K = getContext();
  return K->Map.getl(getVal());
}
}
