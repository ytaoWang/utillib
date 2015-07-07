//#include <gtest/gtest.h>
#include <string>
#include <iterator>
#include "tree.hpp"

using namespace tree;

/*
TEST (TreeTest, InitTree) {
	
	Tree<int> t;
	
	t.addChild(1);
	t.addChild(2);

	EXPECT_EQ(2UL,t.size());

	Tree<int>::iterator iter = t.begin();
	
	for(Tree<int>::iterator iter = t.begin(); iter != t.end(); ++iter)
		std::cout << "-------child:" << *iter << "----" << std::endl;
}
*/
int main(int argc, char *argv[]) {
	
	//::testing::InitGoogleTest(&argc,argv);
	//::testing::GTEST_FLAG(filter) = "Tree.initTree";

	Tree<int> tree;

	tree.addChild(1);
	tree.addChild(1,2);
	tree.addChild(2,3);
	tree.removeChild(2,3);

	std::cout << tree  << ",count:" << tree.size() << std::endl;
	return 0;//RUN_ALL_TESTS();
}
