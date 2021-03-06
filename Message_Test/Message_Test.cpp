// Message_Test.cpp : Defines the entry point for the console application.
//

#include "../Message/Message.h"
#include "../Utilities/Utilities.h"
#include <functional>

class Test
{
public:
  using TestType = std::function<bool()>;
  bool demo() { return true; }
  void execute(TestType t);
private:
  Utilities::Tester<TestType> tester;
};

void Test::execute(TestType t)
{
  tester.execute(t, "Test::demo");
}

int main()
{
  Test test;
  //Test::TestType fun = [&]() { return test.demo(); };
  test.execute([&](){ return test.demo(); });
  return 0;
}

