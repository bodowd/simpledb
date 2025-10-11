#include <istream>
#include <vector>
namespace simpledb {
class PushBackReader {
public:
  PushBackReader(std::istream &is);
  /// Advance the current position in the input stream that is being read from
  int Read();
  /// Decrements the current position in the buffer and stores the specified
  /// character at the new position.
  ///
  /// This method ensures that the buffer is updated in reverse order, placing
  /// the given character 'ch' at the position one step before the current
  /// position. It is assumed that '_currentPos' is always within the valid
  /// range of the buffer.
  ///
  /// Side Effects:
  ///   - Modifies the '_buffer' array at the position determined by
  ///   '_currentPos'.
  ///   - Decreases '_currentPos' by 1.
  void Unread(int ch);

private:
  int _currentPos;

  /// buffer that the input stream will be read into
  std::vector<char> _buffer;
};
} // namespace simpledb
