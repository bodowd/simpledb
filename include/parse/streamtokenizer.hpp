#include "pushbackreader.hpp"
#include <istream>
#include <string>
namespace simpledb {

class StreamTokenizer {
public:
  static int TOKEN_EOF;
  static int TOKEN_EOL;
  static int TOKEN_NUMBER;
  static int TOKEN_WORD;
  static int TOKEN_NONE;

  int tokenType = TOKEN_NONE;
  std::string strVal;
  double numVal;

  StreamTokenizer(std::istream &is);
  /// Designate which character is used for comments
  void CommentChar(int ch);
  void EolIsSignificant(bool flag);
  int LineNum();
  void LowerCaseMode(bool flag);
  int NextToken();
  // check if a char is within the standard 8-bit ASCII character range
  // This should be treated as a literal single character, and it has no special
  // meaning. If it is an ordinary character this will reset the characters
  // metadata
  void OrdinaryChar(int ch);
  void OrdinaryChars(int low, int high);
  // Designate the numeric characters
  void ParseNumbers();
  void PushBack();
  // Designate the quote characters
  void QuoteChar(int ch);
  void ResetSyntax();
  void SlashSlashComments(bool flag);
  void SlashStarComments(bool flag);
  std::string ToString();
  /// Designate which ASCII numbers are whitespace characters
  /// For example from 0x00 to 0x20
  void WhitespaceChars(int low, int high);
  /// Designat which characters are letters between an
  /// lower and upper ASCII range. For example, 'A' to 'Z' will be
  /// marked as "letters" in private datastructure
  void WordChars(int low, int high);

private:
  bool _EOLsignificant = false;
  bool _lowerCase = false;
  bool _slashSlash = false;
  bool _slashStar = false;
  bool _pushedBack = false;
  int _lineNumber = 1;

  PushBackReader _pbr;
  // These vectors are used to mark the type of a character. Each index
  // specifies a character in the 8-bit range of extended ASCII.
  // If the slot is true, it is a character of the type denoted by the vector's
  // name
  std::vector<bool> _whitespace;
  std::vector<bool> _alphabetic;
  std::vector<bool> _numeric;
  std::vector<bool> _quote;
  std::vector<bool> _comment;

  bool isWhitespace(int ch);
  bool isAlphabetic(int ch);
  bool isNumeric(int ch);
  bool isQuote(int ch);
  bool isComment(int ch);

  /// reset metadata of the character at index "ch"
  void resetChar(int ch);
};
} // namespace simpledb
