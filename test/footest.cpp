#include <gtest/gtest.h>

#include <teetime/AbstractStage.h>
#include <teetime/AbstractConsumerStage.h>
#include <teetime/AbstractProducerStage.h>
#include <teetime/Port.h>
#include <vector>

using namespace teetime;

namespace
{
  class TestProducerStage : public AbstractProducerStage<int>
  {
  public:

  private:
    virtual void execute() override
    {
      for(int i=0; i<3; ++i)
      {
        getOutputPort().send(i);
      }
    }
  };

  class TestConsumerStage : public AbstractConsumerStage<int>
  {
  public:
    std::vector<int> valuesProcessed;

    TestConsumerStage()
    {      
    }

  private:
    virtual void execute(const int& value)
    {
      valuesProcessed.push_back(value);
    }
  };
}


TEST( AbstractConsumerStageTest, simpleExecute )
{
  TestProducerStage producer;
  TestConsumerStage consumer;   
  
  connect(producer.getOutputPort(), consumer.getInputPort());

  producer.executeStage();

  EXPECT_EQ(size_t(3), consumer.valuesProcessed.size());
  EXPECT_EQ(0, consumer.valuesProcessed[0]);
  EXPECT_EQ(1, consumer.valuesProcessed[1]);
  EXPECT_EQ(2, consumer.valuesProcessed[2]);
}

int main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  int ret = RUN_ALL_TESTS();
  return ret;
}