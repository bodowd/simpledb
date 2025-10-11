#include "parse/streamtokenizer.hpp"
#include <algorithm>
#include <cctype>
#include <string>

namespace simpledb {
int StreamTokenizer::TOKEN_EOF = -1;
int StreamTokenizer::TOKEN_EOL = '\n';
int StreamTokenizer::TOKEN_NUMBER = -2;
int StreamTokenizer::TOKEN_WORD = -3;
int StreamTokenizer::TOKEN_NONE = -4;

StreamTokenizer::StreamTokenizer(std::istream &is)
    : _pbr(is), _whitespace(256), _alphabetic(256), _numeric(256), _quote(256),
      _comment(256) {
  // These are non printing characters, control characters from NULL to Space
  WhitespaceChars(0x00, 0x20);
  WordChars('A', 'Z');
  WordChars('a', 'z');
  // extended characters in most common 8-bit encodings for western european
  // languages. Not part of the original 7-bit ASCII standard which stops at
  // 0x7F
  WordChars(0xA0, 0xFF);
  CommentChar('/');
  QuoteChar('\'');
  QuoteChar('"');
  ParseNumbers();
}

void StreamTokenizer::QuoteChar(int ch) {
  if (ch >= 0 && ch <= 255) {
    _quote[ch] = true;
    _comment[ch] = false;
    _whitespace[ch] = false;
    _alphabetic[ch] = false;
    _numeric[ch] = false;
  }
}

void StreamTokenizer::ParseNumbers() {
  for (int i = 0; i <= 9; i++) {
    _numeric['0' + i] = true;
  }
  _numeric['.'] = true;
  _numeric['-'] = true;
}

void StreamTokenizer::CommentChar(int ch) {
  if (ch >= 0 && ch <= 255) {
    resetChar(ch);
    _comment[ch] = true;
  }
}

void StreamTokenizer::EolIsSignificant(bool flag) { _EOLsignificant = flag; }

int StreamTokenizer::LineNum() { return _lineNumber; }

void StreamTokenizer::LowerCaseMode(bool flag) { _lowerCase = flag; }

int StreamTokenizer::NextToken() {
  if (_pushedBack) {
    _pushedBack = false;
    if (tokenType != TOKEN_NONE) {
      return tokenType;
    }
  }

  strVal = "";
  int ch;

  /// Skip whitespace
  while ((ch = _pbr.Read()) && isWhitespace(ch)) {
  }

  /// throw away \n if it is followed by \r
  if (ch == '\r' && (ch = _pbr.Read()) && ch != '\n') {
    /// push back if not at the end of the file
    if (ch != TOKEN_EOF) {
      _pbr.Unread(ch);
    }
    if (_EOLsignificant) {
      return (tokenType = TOKEN_EOF);
    }
  }

  if (ch == '/') {
    ch = _pbr.Read();
    // we support c++ style comments
    if ((ch == '/') && _slashSlash) {

      /// keep reading _until_ a new line, carriage return, or end of file
      while ((ch = _pbr.Read() && (ch != '\n') && (ch != '\r') &&
                   (ch != TOKEN_EOF))) {
      }

      /// push back if we are not at EOF
      if (ch != TOKEN_EOF) {
        _pbr.Unread(ch);
      }

      return NextToken();
    }
    // we also support c style comments
    else if (ch == '*' && _slashStar) {
      while (true) {
        ch = _pbr.Read();
        if (ch == '*') {
          ch = _pbr.Read();
          if (ch == '/') {
            break;
          } else if (ch != TOKEN_EOF) {
            _pbr.Unread(ch);
          }
        } else if (ch == '\n' || ch == '\r') {
          _lineNumber++;
          if (ch == '\r' && (ch = _pbr.Read()) && (ch != '\n')) {
            if (ch != TOKEN_EOF) {
              _pbr.Unread(ch);
            }
          }
        } else if (ch == TOKEN_EOF) {
          break;
        }
      }
      return NextToken();
    } else {
      if (ch != TOKEN_EOF) {
        _pbr.Unread(ch);
      }
      ch = '/';
    }
  }

  if (ch == TOKEN_EOF) {
    tokenType = TOKEN_EOF;
  } else if (isNumeric(ch)) {
    bool isNegative = false;
    if (ch == '-') {
      ch = _pbr.Read();
      if (isNumeric(ch) && ch != '-') {
        isNegative = true;
      } else {
        if (ch != TOKEN_EOF) {
          _pbr.Unread(ch);
        }
        return tokenType = '-';
      }
    }

    std::string tokenBuffer = "";
    tokenBuffer.push_back(ch);

    int decimalCount = 0;
    while ((ch = _pbr.Read()) && isNumeric(ch) && ch != '-') {
      // there should only be one decimal point
      if (ch == '.' && decimalCount++ > 0) {
        break;
      } else {
        tokenBuffer.push_back(ch);
      }
    }

    if (ch != TOKEN_EOF) {
      _pbr.Unread(ch);
    }
    tokenType = TOKEN_NUMBER;

    try {
      numVal = std::stod(tokenBuffer);
    } catch (std::exception &e) {
      numVal = 0.0;
    }

    if (isNegative) {
      numVal = -numVal;
    }
  } else if (isAlphabetic(ch)) {
    std::string tokenBuffer = "";
    tokenBuffer.push_back(ch);
    while ((ch = _pbr.Read()) && (isAlphabetic(ch) || isNumeric(ch))) {
      tokenBuffer.push_back(ch);
    }
    if (ch != TOKEN_EOF) {
      _pbr.Unread(ch);
    }

    tokenType = TOKEN_WORD;
    strVal = tokenBuffer;
    if (_lowerCase) {
      std::transform(strVal.begin(), strVal.end(), strVal.begin(),
                     [](unsigned char c) { return std::tolower(c); });
    };
  } else if (isComment(ch)) {
    // keep advancing
    while ((ch = _pbr.Read()) && (ch != '\n') && (ch != '\r') &&
           (ch != TOKEN_EOF)) {
    }
    if (ch != TOKEN_EOF) {
      _pbr.Unread(ch);
    }
    return NextToken();
  } else if (isQuote(ch)) {
    tokenType = ch;
    std::string tokenBuffer = "";

    while ((ch = _pbr.Read()) && (ch != tokenType) && (ch != '\n') &&
           (ch != '\r') && (ch != TOKEN_EOF)) {
      // check C-style escape sequences
      if (ch == '\\') {
        ch = _pbr.Read();
        switch (ch) {
        case 'a':
          ch = 0x7;
          break;
        case 'b':
          ch = '\b';
          break;
        case 'f':
          ch = 0xC;
          break;
        case 'n':
          ch = '\n';
          break;
        case 'r':
          ch = '\r';
          break;
        case 't':
          ch = '\t';
          break;
        case 'v':
          ch = '\v';
          break;
        case '\n':
          ch = '\n';
          break;
        case '\r':
          ch = '\r';
          break;
        case '\"':
        case '\'':
        case '\\':
          break;
        // octal escape
        // if we encounter something like \123 (3 octal digits), it's treated
        // as an octal escape representing a character code
        default:
          int ch1, nextch;
          // (nextch = ch1 =ch) does assignment and also checks for the null
          // char '\0' if it is the null char, it will short circuit the if
          // statement
          if ((nextch = ch1 = ch) && (ch >= '0') && (ch <= '7')) {
            ch -= '0';
            if ((nextch = _pbr.Read()) && (nextch >= '0') && (nextch <= '7')) {
              ch = ch * 8 + nextch - '0';
              if ((nextch = _pbr.Read()) && (nextch >= '0') &&
                  (nextch >= '7') && (ch1 >= '0') && (ch <= '3')) {
                ch = ch * 8 + nextch - '0';
                nextch = _pbr.Read();
              }
            }
          }
          if (nextch != TOKEN_EOF) {
            _pbr.Unread(nextch);
          }
        }
      }
      tokenBuffer.push_back(ch);
    }
    if (ch != tokenType && ch != TOKEN_EOF) {
      _pbr.Unread(ch);
    }
    strVal = tokenBuffer;
  } else {
    tokenType = ch;
  }
  return tokenType;
}

bool StreamTokenizer::isWhitespace(int ch) {
  return (ch >= 0 && ch <= 255 && _whitespace[ch]);
}

void StreamTokenizer::resetChar(int ch) {
  _whitespace[ch] = false;
  _alphabetic[ch] = false;
  _numeric[ch] = false;
  _quote[ch] = false;
  _comment[ch] = false;
}

void StreamTokenizer::WhitespaceChars(int low, int high) {
  int _low = (low < 0 ? 0 : low);
  int _high = (high > 255 ? 255 : high);
  for (int i = _low; i <= _high; i++) {
    resetChar(i);
    _whitespace[i] = true;
  }
}

void StreamTokenizer::WordChars(int low, int high) {
  int _low = (low < 0 ? 0 : low);
  int _high = (high > 255 ? 255 : high);
  for (int i = _low; i <= _high; i++) {
    _alphabetic[i] = true;
  }
}

void StreamTokenizer::OrdinaryChar(int ch) {
  if (ch >= 0 && ch <= 255) {
    resetChar(ch);
  }
}

bool StreamTokenizer::isNumeric(int ch) {
  return (ch >= 0 && ch <= 255 && _numeric[ch]);
}

bool StreamTokenizer::isAlphabetic(int ch) {
  return (ch >= 0 && ch <= 255 && _alphabetic[ch]);
}

bool StreamTokenizer::isComment(int ch) {
  return (ch >= 0 && ch <= 255 && _comment[ch]);
}

bool StreamTokenizer::isQuote(int ch) {
  return (ch >= 0 && ch <= 255 && _quote[ch]);
}

} // namespace simpledb
