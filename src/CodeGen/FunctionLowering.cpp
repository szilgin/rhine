#include "rhine/IR/Context.h"
#include "rhine/IR/Function.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"
#include "rhine/Diagnostic/Diagnostic.h"

namespace rhine {
llvm::Function *Prototype::getOrInsert(llvm::Module *M) {
  auto K = getContext();
  auto FnTy = cast<llvm::FunctionType>(getType()->toLL(M));
  auto MangledName = getMangledName();

  /// getOrInsertFunction::
  ///
  /// Look up the specified function in the module symbol table.
  /// Four possibilities:
  /// 1. If it does not exist, add a prototype for the function and return it.
  /// 2. If it exists, and has a local linkage, the existing function is
  /// renamed and a new one is inserted.
  /// 3. Otherwise, if the existing function has the correct prototype, return
  /// the existing function.
  /// 4. Finally, the function exists but has the wrong prototype: return the
  /// function with a constantexpr cast to the right prototype.
  auto Const = M->getOrInsertFunction(MangledName, FnTy);

  if (auto FunctionCandidate = dyn_cast<llvm::Function>(Const))
    return FunctionCandidate;
  auto Error = MangledName + " was declared with different signature earlier";
  K->DiagPrinter->errorReport(SourceLoc, Error);
  exit(1);
}

llvm::Constant *Prototype::toLL(llvm::Module *M) { return getOrInsert(M); }

void Function::codegenAllBlocks(llvm::Module *M) const {
  auto EntryBlock = getEntryBlock();
  EntryBlock->toContainerLL(M);
  for (auto Block = EntryBlock; Block; Block = Block->getMergeBlock()) {
    Block->toValuesLL(M);
  }
}

llvm::Constant *Function::toLL(llvm::Module *M) {
  auto K = getContext();
  auto CurrentFunction = getOrInsert(M);

  /// Bind argument symbols to function argument values in symbol table
  auto ArgList = getArguments();
  auto S = ArgList.begin();
  for (auto &Arg : CurrentFunction->args()) {
    assert(
        K->Map.add(*S, &Arg) &&
        ("argument " + (*S)->getName() + " conflicts with existing symbol name")
            .c_str());
    ++S;
  }

  // Add function symbol to symbol table, global scope
  K->Map.add(this, CurrentFunction);

  /// Codegen all blocks
  codegenAllBlocks(M);

  auto ExitBlock = getExitBlock();
  if (!ExitBlock->size() || !isa<ReturnInst>(ExitBlock->back()))
    K->Builder->CreateRet(nullptr);
  return CurrentFunction;
}
}