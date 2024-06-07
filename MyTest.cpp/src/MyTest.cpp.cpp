//============================================================================
// Name        : cpp.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <gtest/gtest.h>
TEST(MyFirst, ThisTestPasses){
	SUCCEED();
}

TEST(MyFirstTestClass,ThisTestFails){
	FAIL();
}
