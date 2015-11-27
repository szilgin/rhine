#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Module.h"
#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Transform/LambdaLifting.h"
#include "rhine/Transform/Scope2Block.h"

using namespace rhine;

TEST(Scope2Block, NumberOfBBs)
{
  auto SourcePrg =
    "def main() do\n"
    "  if false do X = 3; else Y = 4; end\n"
    "end";
  ParseFacade Pf(SourcePrg);
  Scope2Block Flatten;
  auto Mod = Pf.parseToIR(ParseSource::STRING, { &Flatten });
  auto MainF = Mod->front();
  auto NumberOfBBs = std::distance(MainF->begin(), MainF->end());
  ASSERT_EQ(NumberOfBBs, 4);
}

TEST(Scope2Block, PredSucc)
{
  auto SourcePrg =
    "def main() do\n"
    "  if false do X = 3; else Y = 4; end\n"
    "end";
  ParseFacade Pf(SourcePrg);
  Scope2Block Flatten;
  auto Mod = Pf.parseToIR(ParseSource::STRING, { &Flatten });
  auto MainF = Mod->front();
  std::vector<int> NumPreds = {0, 1, 1, 2}, NumSuccs = {2, 1, 1, 0};
  auto NumPredsIt = NumPreds.begin();
  auto NumSuccsIt = NumSuccs.begin();
  for (auto BB : *MainF) {
    ASSERT_EQ(std::distance(BB->pred_begin(), BB->pred_end()), *NumPredsIt++);
    ASSERT_EQ(std::distance(BB->succ_begin(), BB->succ_end()), *NumSuccsIt++);
  }
}

TEST(Scope2Block, SetParent)
{
  auto SourcePrg =
    "def main() do\n"
    "  ret 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  Scope2Block Flatten;
  auto Mod = Pf.parseToIR(ParseSource::STRING, { &Flatten });
  auto MainF = Mod->front();
  auto EntryBlock = MainF->getEntryBlock();
  ASSERT_EQ(EntryBlock->getParent(), MainF);
}

TEST(Scope2Block, SetIfParent)
{
  auto SourcePrg =
    "def main() do\n"
    "  if false do X = 3; else Y = 4; end\n"
    "end";
  ParseFacade Pf(SourcePrg);
  Scope2Block Flatten;
  auto Mod = Pf.parseToIR(ParseSource::STRING, { &Flatten });
  auto MainF = Mod->front();
  for (auto BB : *MainF) {
    ASSERT_EQ(BB->getParent(), MainF);
  }
}

TEST(Scope2Block, SetLambdaParent)
{
  auto SourcePrg =
    "def foo() do\n"
    "  Bfunc = fn x ~Int -> ret x; end\n"
    "end";
  ParseFacade Pf(SourcePrg);
  LambdaLifting LambLift;
  Scope2Block Flatten;
  auto Mod = Pf.parseToIR(ParseSource::STRING, { &LambLift, &Flatten });
  for (auto F : *Mod)
    for (auto BB : *F)
      ASSERT_EQ(BB->getParent(), F);
}