#include <gtest/gtest.h>
#include "core/func.hpp"

// Test literal swapping transformation
TEST(FormulaTransformTests, SwapLiteralsTest) {
    FormulaType originalFormula = {{"1", "2", "0"}, {"-3", "4", "0"}};
    FormulaType transformedFormula = swapLiterals(originalFormula);
    
    // Check if literals were shuffled (transformation should not affect size)
    EXPECT_EQ(originalFormula.size(), transformedFormula.size());
    EXPECT_EQ(originalFormula[0].size(), transformedFormula[0].size());
    EXPECT_EQ(originalFormula[1].size(), transformedFormula[1].size());
}

// Test clause negation transformation
TEST(FormulaTransformTests, ClauseNegationTest) {
    FormulaType originalFormula = {{"1", "-2", "0"}, {"3", "4", "0"}};
    FormulaType negatedFormula = clauseNegation(originalFormula);

    // Check if literals were negated correctly
    EXPECT_EQ(negatedFormula[0][0], "-1");
    EXPECT_EQ(negatedFormula[0][1], "2");
    EXPECT_EQ(negatedFormula[1][0], "-3");
    EXPECT_EQ(negatedFormula[1][1], "-4");
}

// Test adding a tautology
TEST(FormulaTransformTests, AddTautologyTest) {
    FormulaType originalFormula = {{"1", "2", "0"}};
    FormulaType newFormula = addTautology(originalFormula);

    // Check if the tautology was added
    EXPECT_EQ(newFormula.size(), 2);
    EXPECT_EQ(newFormula[1][0], "1");
    EXPECT_EQ(newFormula[1][1], "-1");
    EXPECT_EQ(newFormula[1][2], "0");
}
