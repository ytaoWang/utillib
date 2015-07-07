#include <gtest/gtest.h>
#include <string>
#include "util.hpp"

using namespace libutil;

typedef unsigned int uint;


struct IntHash {
	size_t getHash(const size_t & key)
	{
		return key;
	}

	size_t operator()(const uint& key) const
	{
		return key;
	}
};

TEST(HashTableTest, IntKey)  {
	
	HashTable<uint,uint,IntHash> ht;
	
	EXPECT_EQ(0UL,ht.count());
	EXPECT_FALSE(ht.containsKey(1));
	ht.add(1,1);
	EXPECT_EQ(1UL,ht.count());
	EXPECT_TRUE(ht.containsKey(1));
	EXPECT_TRUE(ht.contains(1,1));
	ht[1] = 0;
	EXPECT_TRUE(ht[1] == 0U);
	ht.remove(1);
	EXPECT_EQ(0UL,ht.count());
}

TEST(HashTableTest, expandKey) {
	
	HashTable<std::string,std::string> ht;
	char buf[10];
	std::string c;

	for(int i = 0; i < 100; i ++) 
	{
		sprintf(buf,"a%d",i);
		//ht.add(buf, "ab");
		ht[buf] = "ab";
		ht[buf] = "ac";
		c = ht[buf];
		std::cout << "buf:" << buf << ",value:" << ht[buf] << ",c:" << c << std::endl;
	}

	//std::cout << "11:" << ht["11"]<< std::endl;
	
	EXPECT_EQ(100UL,ht.count());
	
	ht.remove("a8");

	EXPECT_EQ(99UL,ht.count());
	//EXPECT_TRUE(ht["a13"] == "ac");
	EXPECT_TRUE(ht.containsKey("a13"));
	EXPECT_TRUE(ht.contains("a13","ac"));
	EXPECT_FALSE(ht.containsKey("a1012"));
	EXPECT_FALSE(ht.contains("a1123","ac"));
	EXPECT_FALSE(ht.contains("a12","ab"));

	for(int i = 0; i < 1000; i ++) 
	{
		sprintf(buf,"a%d",i);
		//ht.add(buf, "ab");
		ht[buf] = "ab";
		ht[buf] = "ac";
		c = ht[buf];
		std::cout << "buf:" << buf << ",value:" << ht[buf] << ",c:" << c << std::endl;
	}

	ht.remove("a18");

	EXPECT_EQ(999UL,ht.count());
	//EXPECT_TRUE(ht["a13"] == "ac");
	EXPECT_TRUE(ht.containsKey("a13"));
	EXPECT_TRUE(ht.contains("a13","ac"));
	EXPECT_TRUE(ht.containsKey("a112"));
	EXPECT_FALSE(ht.contains("a1203","ac"));
	
}


void expandKey()
{

	HashTable<std::string,std::string> ht;
	char buf[10];
	std::string c;

	for(int i = 0; i < 100; i ++) {
		sprintf(buf,"a%d",i);
		//ht.add(buf, "ab");
		ht[buf] = "ab";
		ht[buf] = "ac";
		c = ht[buf];
		std::cout << "buf:" << buf << ",value:" << ht[buf] << ",c:" << c << std::endl;
	}
	
	ht.remove("a8");

	for(int i = 0; i < 1000; i ++) 
	{
		sprintf(buf,"a%d",i);
		//ht.add(buf, "ab");
		ht[buf] = "ab";
		ht[buf] = "ac";
		c = ht[buf];
		std::cout << "buf:" << buf << ",value:" << ht[buf] << ",c:" << c << std::endl;
	}

	ht.remove("a18");
}

int test_main(int argc, char *argv[]) 
{
	::testing::InitGoogleTest(&argc,argv);
	::testing::GTEST_FLAG(filter) = "HashTableTest.expandKey"; // run specific test case
	HashTable<uint,std::string> ht;
	ht[0] = "ac";
	
	for(int i = 0; i < 100; i ++) {
		ht[i] = "ab";
	}

	return RUN_ALL_TESTS();
}
/*
void test_remove(void) {

	HashTable<std::string,std::string,StringHash()> ht;
	char buf[10];
	std::string c;

	for(int i = 0; i < 100; i ++) 
	{
		sprintf(buf,"a%d",i);
		//ht.add(buf, "ab");
		ht[buf] = "ab";
		ht[buf] = "ac";
		c = ht[buf];
		std::cout << "buf:" << buf << ",value:" << ht[buf] << ",c:" << c << std::endl;
	}
	ht.remove("a8");
}
*/
int main(int argc,char *argv[]) {

	test_main(argc,argv);
	return 0;
}
