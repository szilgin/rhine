//-*- C++ -*-

#ifndef RHINE_CONSTANT_H
#define RHINE_CONSTANT_H

#include "llvm/IR/Constants.h"
#include "llvm/ADT/iterator_range.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/User.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/BasicBlock.h"

using namespace std;
using namespace llvm;

namespace rhine {
class Module;

class Constant : public User {
public:
  Constant(Type *Ty, RTValue ID);
  virtual ~Constant() {}
  static bool classof(const Value *V);
  virtual llvm::Constant *toLL(llvm::Module *M) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class ConstantInt : public Constant {
  int Val;
public:
  ConstantInt(int Val, unsigned Bitwidth, Context *K);
  static ConstantInt *get(int Val, unsigned Bitwidth, Context *K);
  static bool classof(const Value *V);
  int getVal() const;
  unsigned getBitwidth() const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class ConstantBool : public Constant {
  bool Val;
public:
  ConstantBool(bool Val, Context *K);
  virtual ~ConstantBool() {}
  static ConstantBool *get(bool Val, Context *K);
  static bool classof(const Value *V);
  float getVal() const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class ConstantFloat : public Constant {
public:
  float Val;
  ConstantFloat(float Val, Context *K);
  virtual ~ConstantFloat() {}
  static ConstantFloat *get(float Val, Context *K);
  static bool classof(const Value *V);
  float getVal() const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class Prototype : public User {
private:
  Module *ParentModule;
  std::vector<Argument *> ArgumentList;
  Argument *VariadicRestLoadInst;
public:
  Prototype(FunctionType *FTy, RTValue RTy = RT_Prototype);
  virtual ~Prototype();
  static Prototype *get(FunctionType *FTy);
  static bool classof(const Value *V);
  void setParent(Module *Parent);
  Module *getParent() const;
  void setName(std::string N);
  std::string getName() const;
  std::string getMangledName() const;
  void setArguments(std::vector<Argument *> L);
  void setVariadicRest(Argument *Rest);
  std::vector<Argument *> getArguments() const;
  typedef std::vector<Argument *>::iterator arg_iterator;
  arg_iterator arg_begin();
  arg_iterator arg_end();
  iterator_range<arg_iterator> args();
  virtual llvm::Constant *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
  void emitArguments(std::ostream &Stream) const;
};

class Function : public Prototype {
  BasicBlock *Val;
public:
  Function(FunctionType *FTy);
  virtual ~Function();
  static Function *get(FunctionType *FTy);
  static bool classof(const Value *V);
  void setBody(BasicBlock *Body);
  BasicBlock *getVal() const;
  BasicBlock *getEntryBlock() const;
  BasicBlock::iterator begin();
  BasicBlock::iterator end();
  virtual llvm::Constant *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class Pointer : public User {
  Value *Val;
public:
  Pointer(Value *V, Type *Ty);
  virtual ~Pointer();
  static Pointer *get(Value *V);
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *getVal() const;
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
