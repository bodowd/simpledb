#include "streamtokenizer.hpp"
#include <memory>
#include <set>
#include <string>

namespace simpledb {

/// The stream tokenizer is a general purpose lexical analyzer and the Lexer
/// class here provides an easier way for the parser to access the token stream
/// (p. 241 in the book)
///
/// There are two kinds of methods the parser can call: match and eat methods.
/// Match methods allow the parser to ask about the current token, and the eat
/// methods returning its value and move to the next token
class Lexer {
public:
  Lexer(const std::string &s);

  /// match methods
  ///
  /// Returns true if the current token has the specified value, otherwise
  /// returns false
  bool MatchDelim(const char &ch);
  bool MatchIntConstant();
  bool MatchStringConstant();
  bool MatchKeyword(const std::string &kw);
  bool MatchId();

  /// "eat" methods
  ///
  /// These methods will advance to the next token if the call is successful.
  /// If not successful, will throw an exception
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

  /// Advance to the next token using the underlying tokenizer
  void nextToken();
  void initKeywords();
};
} // namespace simpledb
