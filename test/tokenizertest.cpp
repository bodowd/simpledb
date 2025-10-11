#include <gtest/gtest-death-test.h>
#include <iostream>

#include "parse/streamtokenizer.hpp"
#include "gtest/gtest.h"

namespace simpledb {
void print_current_token(const std::set<std::string> &keywords,
                         const std::set<std::string> &expected_ids,
                         const std::set<int> &expected_numbers,
                         StreamTokenizer &tok) {

  if (tok.tokenType == StreamTokenizer::TOKEN_NUMBER) {
    std::cout << "IntConstant " << tok.numVal << std::endl;
    ASSERT_TRUE(expected_numbers.find(tok.numVal) != expected_numbers.end());
  } else if (tok.tokenType == StreamTokenizer::TOKEN_WORD) {
    std::string word = tok.strVal;

    ASSERT_TRUE(keywords.find(word) != keywords.end() ||
                expected_ids.find(word) != expected_ids.end());
    if (keywords.find(word) != keywords.end()) {
      std::cout << "Keyword " << word << std::endl;
    } else {
      std::cout << "Id " << word << std::endl;
    }
  } else if (tok.tokenType == '\'') {
    std::cout << "StringConstant " << tok.strVal << std::endl;
  } else {
    std::cout << "Delimiter " << static_cast<char>(tok.tokenType) << std::endl;
  }
}

TEST(parse, tokenizer_test) {
  std::set<std::string> keywords = {
      "select", "from",    "where",  "and", "insert", "into",
      "values", "delete",  "update", "set", "create", "table",
      "int",    "varchar", "view",   "as",  "index",  "on"};

  std::set<std::string> expected_ids = {"a", "x", "z", "b", "c"};

  std::set<int> expected_numbers = {3};

  std::string s;
  std::stringstream ss;
  ss << "select a from x, z where b = 3 and c = 'foobar'";

  StreamTokenizer tok(ss);
  tok.OrdinaryChar('.');
  tok.WordChars('_', '_');
  tok.LowerCaseMode(true);
  while (tok.NextToken() != StreamTokenizer::TOKEN_EOF) {
    print_current_token(keywords, expected_ids, expected_numbers, tok);
  }
}
} // namespace simpledb
