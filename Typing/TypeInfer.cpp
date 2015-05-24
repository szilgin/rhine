#include "rhine/IR.h"
#include "rhine/Externals.h"

namespace rhine {
Type *ConstantInt::typeInfer(Context *K) {
  return getType();
}

Type *ConstantBool::typeInfer(Context *K) {
  return getType();
}

Type *ConstantFloat::typeInfer(Context *K) {
  return getType();
}

Type *GlobalString::typeInfer(Context *K) {
  return getType();
}

Type *Function::typeInfer(Context *K) {
  // Argument list transform
  auto V = getArgumentList();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Symbol *S) -> Symbol * {
                   S->setType(S->typeInfer(K));
                   return S;
                 });
  // FunctionType and Body transform
  auto FType = dyn_cast<FunctionType>(getType());
  Type *LastTy;
  for (auto I: getVal()) {
    LastTy = I->typeInfer(K);
    I->setType(LastTy);
  }
  FType->setRTy(LastTy);
  K->addMapping(getName(), LastTy);
  return FType;
}

Type *AddInst::typeInfer(Context *K) {
  auto LType = getOperand(0)->typeInfer();
  if (getOperand(1)->typeInfer() != LType)
    assert(0 && "AddInst with operands of different types");
  return getType();
}

Type *Symbol::typeInfer(Context *K) {
  auto Ty = getType();
  auto Name = getName();
  if (Ty != UnType::get(K)) {
    ;
  } else if (auto Result = K->getMappingTy(Name)) {
    Ty = Result;
  } else if (auto Result = Externals::get(K)->getMappingTy(Name)) {
    Ty = Result->getRTy();
  } else {
    K->DiagPrinter->errorReport(
        SourceLoc, "untyped symbol " + Name);
    exit(1);
  }
  K->addMapping(Name, Ty);
  return Ty;
}

Type *CallInst::typeInfer(Context *K) {
  Type *Ty = getType();
  auto Name = getName();
  if (Ty != UnType::get(K))
    return Ty;
  if (auto Result = K->getMappingTy(Name)) {
    return Result;
  } else if (auto Result = Externals::get(K)->getMappingTy(Name)) {
    return Result->getRTy();
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "untyped function " + Name);
  exit(1);
}

Type *BindInst::typeInfer(Context *K) {
  Type *Ty = getVal()->typeInfer();
  assert (Ty != UnType::get(K) &&
          "Unable to type infer BindInst");
  K->addMapping(getName(), Ty);
  return getType();
}

void Module::typeInfer(Context *K) {
  auto V = getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Function *F) -> Function * {
                   F->setType(F->typeInfer(K));
                   return F;
                 });
}
}
