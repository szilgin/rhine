//-*- C++ -*-
#ifndef RHINE_TYPEINFER_H
#define RHINE_TYPEINFER_H

#include "rhine/Transform/ValueVisitor.h"
#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class TypeInfer : public ValueVisitor<Type *>, public FunctionPass {
  Context *K;
public:
  TypeInfer();
  virtual ~TypeInfer() {}
  void runOnFunction(Function *F) override;
  template <typename T>
  Type *typeInferValueList(std::vector<T> V);
  Type *typeInferBB(BasicBlock *BB);
  using ValueVisitor<Type *>::visit;
  Type *visit(ConstantInt *I) override;
  Type *visit(ConstantBool *B) override;
  Type *visit(ConstantFloat *F) override;
  Type *visit(GlobalString *G) override;
  Type *visit(Prototype *P) override;
  Type *visit(Function *F) override;
  Type *visit(Pointer *P) override;
  Type *visit(AddInst *A) override;
  Type *visit(IfInst *F) override;
  Type *visit(Argument *A) override;
  Type *visit(MallocInst *B) override;
  Type *visit(LoadInst *S) override;
  Type *visit(CallInst *C) override;
  Type *visit(ReturnInst *C) override;
};
}

#endif
