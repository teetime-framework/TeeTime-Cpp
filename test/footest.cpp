#include <johl/foo.h>

//unit test framework
#include <gtest/gtest.h>

using namespace johl;

TEST( FooTest, foo )
{
  EXPECT_EQ( foo(), 42 );
}

int main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  int ret = RUN_ALL_TESTS();
  return ret;
}