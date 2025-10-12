#include "parse/lexer.hpp"
#include <sstream>
#include <stdexcept>

namespace simpledb {
Lexer::Lexer(const std::string &s) {
  initKeywords();
  std::stringstream sstream(s);
  _tok = std::make_unique<StreamTokenizer>(sstream);
  _tok->OrdinaryChar('.');
  _tok->WordChars('_', '_');
  _tok->LowerCaseMode(true);
  nextToken();
}

bool Lexer::MatchDelim(const char &d) {
  return d == static_cast<char>(_tok->tokenType);
}

bool Lexer::MatchIntConstant() {
  return _tok->tokenType == StreamTokenizer::TOKEN_NUMBER;
}

bool Lexer::MatchStringConstant() {
  return '\'' == static_cast<char>(_tok->tokenType);
}

bool Lexer::MatchKeyword(const std::string &w) {
  return (_tok->tokenType == StreamTokenizer::TOKEN_WORD) &&
         (_tok->strVal == w);
}

bool Lexer::MatchId() {
  return (_tok->tokenType == StreamTokenizer::TOKEN_WORD) &&
         (_keywords.find(_tok->strVal) == _keywords.end());
}

void Lexer::EatDelim(const char &d) {
  if (!MatchDelim(d)) {
    throw std::runtime_error("bad syntax exception");
  }
  nextToken();
}

int Lexer::EatIntConstant() {
  if (!MatchIntConstant()) {
    throw std::runtime_error("bad syntax exception");
  }
  int i = static_cast<int>(_tok->numVal);
  nextToken();
  return i;
}

std::string Lexer::EatStringConstant() {
  if (!MatchStringConstant()) {
    throw std::runtime_error("bad syntax exception");
  }
  std::string s = _tok->strVal;
  nextToken();
  return s;
}

void Lexer::EatKeyword(const std::string &w) {
  if (!MatchKeyword(w)) {
    throw std::runtime_error("bad syntax exception");
  }
  nextToken();
}

std::string Lexer::EatId() {
  if (!MatchId()) {
    throw std::runtime_error("bad syntax exception");
  }
  std::string s = _tok->strVal;
  nextToken();
  return s;
}

void Lexer::nextToken() {
  try {
    _tok->NextToken();
  } catch (std::exception &e) {
    throw std::runtime_error("bad syntax exception");
  }
}

void Lexer::initKeywords() {
  _keywords = std::set<std::string>{
      "select",  "from",   "where",  "and", "insert", "into",
      "values",  "delete", "update", "set", "create", "table",
      "varchar", "int",    "view",   "as",  "index",  "on"};
}

} // namespace simpledb
