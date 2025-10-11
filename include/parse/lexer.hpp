#include "streamtokenizer.hpp"
#include <memory>
#include <set>
#include <string>

namespace simpledb {
class Lexer {
public:
  Lexer(const std::string &s);

  /// match methods
  ///
  /// Returns true if the current token has the specified value, otherwise
  /// an exception is thrown
  bool MatchDelim(const char &ch);
  bool MatchIntConstant();
  bool MatchStringConstant();
  bool MatchKeyword(const std::string &kw);
  bool MatchId();

  /// "eat" methods
  ///
  /// These methods will advance to the next token if the call is successful.
  void EatDelim(const char &ch);
  /// returns the value of the token
  int EatIntConstant();
  /// returns the value of the token
  std::string EatStringConstant();
  void EatKeyword(const std::string &kw);
  /// returns the value of the token
  std::string EatId();

private:
  std::set<std::string> _keywords;
  std::unique_ptr<StreamTokenizer> _tok;

  void nextToken();
  void initKeywords();
};
} // namespace simpledb
