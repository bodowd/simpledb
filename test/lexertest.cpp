#include "parse/lexer.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
namespace simpledb {
TEST(parse, lexer_test) {
  std::string s = "";
  /// The test expects a line to be of the form "A=c" or "c=A" where A is
  /// an identifier and c is an int constant. An input line in any other form
  /// generates an exception
  ///
  /// If the wrong methods are used "EatIntConstant" instead of EatId() for
  /// example, the test will fail because an exception will be thrown
  std::stringstream stringStream("A=1 \n 3=A");
  std::string x = "";
  int y = 0;
  while (std::getline(stringStream, s, '\n')) {
    Lexer lex(s);
    if (lex.MatchId()) {
      x = lex.EatId();
      lex.EatDelim('=');
      y = lex.EatIntConstant();
    } else {
      y = lex.EatIntConstant();
      lex.EatDelim('=');
      x = lex.EatId();
    }
    std::cout << x << " equals " << y << std::endl;
  }
}
} // namespace simpledb
