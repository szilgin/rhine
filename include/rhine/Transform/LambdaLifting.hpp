#pragma once

#include "rhine/Pass/FunctionPass.hpp"

namespace rhine {
class Function;

class LambdaLifting : public FunctionPass {
public:
  virtual ~LambdaLifting() = default;
  void runOnFunction(Function *F) override;
};
}
