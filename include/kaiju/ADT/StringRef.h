
#ifndef KAIJU_ADT_STRINGREF_H
#define KAIJU_ADT_STRINGREF_H

#include "kaiju/ADT/iterator_range.h"
#include "kaiju/Support/Compiler.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace kaiju {
  class StringRef;

  /// Helper functions for StringRef::getAsInteger.
  bool getAsUnsignedInteger(StringRef Str, unsigned Radix,
                            unsigned long long &Result);

  bool getAsSignedInteger(StringRef Str, unsigned Radix, long long &Result);

  bool consumeUnsignedInteger(StringRef &Str, unsigned Radix,
                              unsigned long long &Result);
  bool consumeSignedInteger(StringRef &Str, unsigned Radix, long long &Result);

  /// StringRef - Represent a constant reference to a string, i.e. a character
  /// array and a length, which need not be null terminated.
  ///
  /// This class does not own the string data, it is expected to be used in
  /// situations where the character data resides in some other buffer, whose
  /// lifetime extends past that of the StringRef. For this reason, it is not in
  /// general safe to store a StringRef.
  class StringRef {
  public:
    static const size_t npos = ~size_t(0);

    using iterator = const char *;
    using const_iterator = const char *;
    using size_type = size_t;

  private:
    /// The start of the string, in an external buffer.
    const char *Data = nullptr;

    /// The length of the string.
    size_t Length = 0;

    // Workaround memcmp issue with null pointers (undefined behavior)
    // by providing a specialized version
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    static int compareMemory(const char *Lhs, const char *Rhs, size_t Length) {
      if (Length == 0) { return 0; }
      return ::memcmp(Lhs,Rhs,Length);
    }

  public:
    /// @name Constructors
    /// @{

    /// Construct an empty string ref.
    /*implicit*/ StringRef() = default;

    /// Disable conversion from nullptr.  This prevents things like
    /// if (S == nullptr)
    StringRef(std::nullptr_t) = delete;

    /// Construct a string ref from a cstring.
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    /*implicit*/ StringRef(const char *Str)
        : Data(Str), Length(Str ? ::strlen(Str) : 0) {}

    /// Construct a string ref from a pointer and length.
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    /*implicit*/ constexpr StringRef(const char *data, size_t length)
        : Data(data), Length(length) {}

    /// Construct a string ref from an std::string.
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    /*implicit*/ StringRef(const std::string &Str)
      : Data(Str.data()), Length(Str.length()) {}

    static StringRef withNullAsEmpty(const char *data) {
      return StringRef(data ? data : "");
    }

    /// @}
    /// @name Iterators
    /// @{

    iterator begin() const { return Data; }

    iterator end() const { return Data + Length; }

    const unsigned char *bytes_begin() const {
      return reinterpret_cast<const unsigned char *>(begin());
    }
    const unsigned char *bytes_end() const {
      return reinterpret_cast<const unsigned char *>(end());
    }
    iterator_range<const unsigned char *> bytes() const {
      return make_range(bytes_begin(), bytes_end());
    }

    /// @}
    /// @name String Operations
    /// @{

    /// data - Get a pointer to the start of the string (which may not be null
    /// terminated).
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    const char *data() const { return Data; }

    /// empty - Check if the string is empty.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool empty() const { return Length == 0; }

    /// size - Get the string size.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    size_t size() const { return Length; }

    /// front - Get the first character in the string.
    KAIJU_NODISCARD
    char front() const {
      assert(!empty());
      return Data[0];
    }

    /// back - Get the last character in the string.
    KAIJU_NODISCARD
    char back() const {
      assert(!empty());
      return Data[Length-1];
    }

    // copy - Allocate copy in Allocator and return StringRef to it.
    template <typename Allocator>
    KAIJU_NODISCARD StringRef copy(Allocator &A) const {
      // Don't request a length 0 copy from the allocator.
      if (empty())
        return StringRef();
      char *S = A.template Allocate<char>(Length);
      std::copy(begin(), end(), S);
      return StringRef(S, Length);
    }

    /// equals - Check for string equality, this is more efficient than
    /// compare() when the relative ordering of inequal strings isn't needed.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool equals(StringRef RHS) const {
      return (Length == RHS.Length &&
              compareMemory(Data, RHS.Data, RHS.Length) == 0);
    }

    /// equals_lower - Check for string equality, ignoring case.
    KAIJU_NODISCARD
    bool equals_lower(StringRef RHS) const {
      return Length == RHS.Length && compare_lower(RHS) == 0;
    }

    /// compare - Compare two strings; the result is -1, 0, or 1 if this string
    /// is lexicographically less than, equal to, or greater than the \p RHS.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    int compare(StringRef RHS) const {
      // Check the prefix for a mismatch.
      if (int Res = compareMemory(Data, RHS.Data, std::min(Length, RHS.Length)))
        return Res < 0 ? -1 : 1;

      // Otherwise the prefixes match, so we only need to check the lengths.
      if (Length == RHS.Length)
        return 0;
      return Length < RHS.Length ? -1 : 1;
    }

    /// compare_lower - Compare two strings, ignoring case.
    KAIJU_NODISCARD
    int compare_lower(StringRef RHS) const;

    /// compare_numeric - Compare two strings, treating sequences of digits as
    /// numbers.
    KAIJU_NODISCARD
    int compare_numeric(StringRef RHS) const;

    /// \brief Determine the edit distance between this string and another
    /// string.
    ///
    /// \param Other the string to compare this string against.
    ///
    /// \param AllowReplacements whether to allow character
    /// replacements (change one character into another) as a single
    /// operation, rather than as two operations (an insertion and a
    /// removal).
    ///
    /// \param MaxEditDistance If non-zero, the maximum edit distance that
    /// this routine is allowed to compute. If the edit distance will exceed
    /// that maximum, returns \c MaxEditDistance+1.
    ///
    /// \returns the minimum number of character insertions, removals,
    /// or (if \p AllowReplacements is \c true) replacements needed to
    /// transform one of the given strings into the other. If zero,
    /// the strings are identical.
    KAIJU_NODISCARD
    unsigned edit_distance(StringRef Other, bool AllowReplacements = true,
                           unsigned MaxEditDistance = 0) const;

    /// str - Get the contents as an std::string.
    KAIJU_NODISCARD
    std::string str() const {
      if (!Data) return std::string();
      return std::string(Data, Length);
    }

    /// @}
    /// @name Operator Overloads
    /// @{

    KAIJU_NODISCARD
    char operator[](size_t Index) const {
      assert(Index < Length && "Invalid index!");
      return Data[Index];
    }

    /// Disallow accidental assignment from a temporary std::string.
    ///
    /// The declaration here is extra complicated so that `stringRef = {}`
    /// and `stringRef = "abc"` continue to select the move assignment operator.
    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value,
                            StringRef>::type &
    operator=(T &&Str) = delete;

    /// @}
    /// @name Type Conversions
    /// @{

    operator std::string() const {
      return str();
    }

    /// @}
    /// @name String Predicates
    /// @{

    /// Check if this string starts with the given \p Prefix.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool startswith(StringRef Prefix) const {
      return Length >= Prefix.Length &&
             compareMemory(Data, Prefix.Data, Prefix.Length) == 0;
    }

    /// Check if this string starts with the given \p Prefix, ignoring case.
    KAIJU_NODISCARD
    bool startswith_lower(StringRef Prefix) const;

    /// Check if this string ends with the given \p Suffix.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool endswith(StringRef Suffix) const {
      return Length >= Suffix.Length &&
        compareMemory(end() - Suffix.Length, Suffix.Data, Suffix.Length) == 0;
    }

    /// Check if this string ends with the given \p Suffix, ignoring case.
    KAIJU_NODISCARD
    bool endswith_lower(StringRef Suffix) const;

    /// @}
    /// @name String Searching
    /// @{

    /// Search for the first character \p C in the string.
    ///
    /// \returns The index of the first occurrence of \p C, or npos if not
    /// found.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    size_t find(char C, size_t From = 0) const {
      size_t FindBegin = std::min(From, Length);
      if (FindBegin < Length) { // Avoid calling memchr with nullptr.
        // Just forward to memchr, which is faster than a hand-rolled loop.
        if (const void *P = ::memchr(Data + FindBegin, C, Length - FindBegin))
          return static_cast<const char *>(P) - Data;
      }
      return npos;
    }

    /// Search for the first character \p C in the string, ignoring case.
    ///
    /// \returns The index of the first occurrence of \p C, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t find_lower(char C, size_t From = 0) const;

    /// Search for the first string \p Str in the string.
    ///
    /// \returns The index of the first occurrence of \p Str, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t find(StringRef Str, size_t From = 0) const;

    /// Search for the first string \p Str in the string, ignoring case.
    ///
    /// \returns The index of the first occurrence of \p Str, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t find_lower(StringRef Str, size_t From = 0) const;

    /// Search for the last character \p C in the string.
    ///
    /// \returns The index of the last occurrence of \p C, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t rfind(char C, size_t From = npos) const {
      From = std::min(From, Length);
      size_t i = From;
      while (i != 0) {
        --i;
        if (Data[i] == C)
          return i;
      }
      return npos;
    }

    /// Search for the last character \p C in the string, ignoring case.
    ///
    /// \returns The index of the last occurrence of \p C, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t rfind_lower(char C, size_t From = npos) const;

    /// Search for the last string \p Str in the string.
    ///
    /// \returns The index of the last occurrence of \p Str, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t rfind(StringRef Str) const;

    /// Search for the last string \p Str in the string, ignoring case.
    ///
    /// \returns The index of the last occurrence of \p Str, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t rfind_lower(StringRef Str) const;

    /// Find the first character in the string that is \p C, or npos if not
    /// found. Same as find.
    KAIJU_NODISCARD
    size_t find_first_of(char C, size_t From = 0) const {
      return find(C, From);
    }

    /// Find the first character in the string that is in \p Chars, or npos if
    /// not found.
    ///
    /// Complexity: O(size() + Chars.size())
    KAIJU_NODISCARD
    size_t find_first_of(StringRef Chars, size_t From = 0) const;

    /// Find the first character in the string that is not \p C or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t find_first_not_of(char C, size_t From = 0) const;

    /// Find the first character in the string that is not in the string
    /// \p Chars, or npos if not found.
    ///
    /// Complexity: O(size() + Chars.size())
    KAIJU_NODISCARD
    size_t find_first_not_of(StringRef Chars, size_t From = 0) const;

    /// Find the last character in the string that is \p C, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t find_last_of(char C, size_t From = npos) const {
      return rfind(C, From);
    }

    /// Find the last character in the string that is in \p C, or npos if not
    /// found.
    ///
    /// Complexity: O(size() + Chars.size())
    KAIJU_NODISCARD
    size_t find_last_of(StringRef Chars, size_t From = npos) const;

    /// Find the last character in the string that is not \p C, or npos if not
    /// found.
    KAIJU_NODISCARD
    size_t find_last_not_of(char C, size_t From = npos) const;

    /// Find the last character in the string that is not in \p Chars, or
    /// npos if not found.
    ///
    /// Complexity: O(size() + Chars.size())
    KAIJU_NODISCARD
    size_t find_last_not_of(StringRef Chars, size_t From = npos) const;

    /// Return true if the given string is a substring of *this, and false
    /// otherwise.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool contains(StringRef Other) const { return find(Other) != npos; }

    /// Return true if the given character is contained in *this, and false
    /// otherwise.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool contains(char C) const { return find_first_of(C) != npos; }

    /// Return true if the given string is a substring of *this, and false
    /// otherwise.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool contains_lower(StringRef Other) const {
      return find_lower(Other) != npos;
    }

    /// Return true if the given character is contained in *this, and false
    /// otherwise.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool contains_lower(char C) const { return find_lower(C) != npos; }

    /// @}
    /// @name Helpful Algorithms
    /// @{

    /// Return the number of occurrences of \p C in the string.
    KAIJU_NODISCARD
    size_t count(char C) const {
      size_t Count = 0;
      for (size_t i = 0, e = Length; i != e; ++i)
        if (Data[i] == C)
          ++Count;
      return Count;
    }

    /// Return the number of non-overlapped occurrences of \p Str in
    /// the string.
    size_t count(StringRef Str) const;

    /// Parse the current string as an integer of the specified radix.  If
    /// \p Radix is specified as zero, this does radix autosensing using
    /// extended C rules: 0 is octal, 0x is hex, 0b is binary.
    ///
    /// If the string is invalid or if only a subset of the string is valid,
    /// this returns true to signify the error.  The string is considered
    /// erroneous if empty or if it overflows T.
    template <typename T>
    typename std::enable_if<std::numeric_limits<T>::is_signed, bool>::type
    getAsInteger(unsigned Radix, T &Result) const {
      long long LLVal;
      if (getAsSignedInteger(*this, Radix, LLVal) ||
            static_cast<T>(LLVal) != LLVal)
        return true;
      Result = LLVal;
      return false;
    }

    template <typename T>
    typename std::enable_if<!std::numeric_limits<T>::is_signed, bool>::type
    getAsInteger(unsigned Radix, T &Result) const {
      unsigned long long ULLVal;
      // The additional cast to unsigned long long is required to avoid the
      // Visual C++ warning C4805: '!=' : unsafe mix of type 'bool' and type
      // 'unsigned __int64' when instantiating getAsInteger with T = bool.
      if (getAsUnsignedInteger(*this, Radix, ULLVal) ||
          static_cast<unsigned long long>(static_cast<T>(ULLVal)) != ULLVal)
        return true;
      Result = ULLVal;
      return false;
    }

    /// Parse the current string as an integer of the specified radix.  If
    /// \p Radix is specified as zero, this does radix autosensing using
    /// extended C rules: 0 is octal, 0x is hex, 0b is binary.
    ///
    /// If the string does not begin with a number of the specified radix,
    /// this returns true to signify the error. The string is considered
    /// erroneous if empty or if it overflows T.
    /// The portion of the string representing the discovered numeric value
    /// is removed from the beginning of the string.
    template <typename T>
    typename std::enable_if<std::numeric_limits<T>::is_signed, bool>::type
    consumeInteger(unsigned Radix, T &Result) {
      long long LLVal;
      if (consumeSignedInteger(*this, Radix, LLVal) ||
          static_cast<long long>(static_cast<T>(LLVal)) != LLVal)
        return true;
      Result = LLVal;
      return false;
    }

    template <typename T>
    typename std::enable_if<!std::numeric_limits<T>::is_signed, bool>::type
    consumeInteger(unsigned Radix, T &Result) {
      unsigned long long ULLVal;
      if (consumeUnsignedInteger(*this, Radix, ULLVal) ||
          static_cast<unsigned long long>(static_cast<T>(ULLVal)) != ULLVal)
        return true;
      Result = ULLVal;
      return false;
    }

    /// Parse the current string as an IEEE double-precision floating
    /// point value.  The string must be a well-formed double.
    ///
    /// If \p AllowInexact is false, the function will fail if the string
    /// cannot be represented exactly.  Otherwise, the function only fails
    /// in case of an overflow or underflow.
    bool getAsDouble(double &Result, bool AllowInexact = true) const;

    /// @}
    /// @name String Operations
    /// @{

    // Convert the given ASCII string to lowercase.
    KAIJU_NODISCARD
    std::string lower() const;

    /// Convert the given ASCII string to uppercase.
    KAIJU_NODISCARD
    std::string upper() const;

    /// @}
    /// @name Substring Operations
    /// @{

    /// Return a reference to the substring from [Start, Start + N).
    ///
    /// \param Start The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param N The number of characters to included in the substring. If N
    /// exceeds the number of characters remaining in the string, the string
    /// suffix (starting with \p Start) will be returned.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    StringRef substr(size_t Start, size_t N = npos) const {
      Start = std::min(Start, Length);
      return StringRef(Data + Start, std::min(N, Length - Start));
    }

    /// Return a StringRef equal to 'this' but with only the first \p N
    /// elements remaining.  If \p N is greater than the length of the
    /// string, the entire string is returned.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    StringRef take_front(size_t N = 1) const {
      if (N >= size())
        return *this;
      return drop_back(size() - N);
    }

    /// Return a StringRef equal to 'this' but with only the last \p N
    /// elements remaining.  If \p N is greater than the length of the
    /// string, the entire string is returned.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    StringRef take_back(size_t N = 1) const {
      if (N >= size())
        return *this;
      return drop_front(size() - N);
    }

    /// Return a StringRef equal to 'this' but with the first \p N elements
    /// dropped.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    StringRef drop_front(size_t N = 1) const {
      assert(size() >= N && "Dropping more elements than exist");
      return substr(N);
    }

    /// Return a StringRef equal to 'this' but with the last \p N elements
    /// dropped.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    StringRef drop_back(size_t N = 1) const {
      assert(size() >= N && "Dropping more elements than exist");
      return substr(0, size()-N);
    }

    /// Returns true if this StringRef has the given prefix and removes that
    /// prefix.
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool consume_front(StringRef Prefix) {
      if (!startswith(Prefix))
        return false;

      *this = drop_front(Prefix.size());
      return true;
    }

    /// Returns true if this StringRef has the given suffix and removes that
    /// suffix.
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    bool consume_back(StringRef Suffix) {
      if (!endswith(Suffix))
        return false;

      *this = drop_back(Suffix.size());
      return true;
    }

    /// Return a reference to the substring from [Start, End).
    ///
    /// \param Start The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param End The index following the last character to include in the
    /// substring. If this is npos or exceeds the number of characters
    /// remaining in the string, the string suffix (starting with \p Start)
    /// will be returned. If this is less than \p Start, an empty string will
    /// be returned.
    KAIJU_NODISCARD
    KAIJU_ATTRIBUTE_ALWAYS_INLINE
    StringRef slice(size_t Start, size_t End) const {
      Start = std::min(Start, Length);
      End = std::min(std::max(Start, End), Length);
      return StringRef(Data + Start, End - Start);
    }

    /// Split into two substrings around the first occurrence of a separator
    /// character.
    ///
    /// If \p Separator is in the string, then the result is a pair (LHS, RHS)
    /// such that (*this == LHS + Separator + RHS) is true and RHS is
    /// maximal. If \p Separator is not in the string, then the result is a
    /// pair (LHS, RHS) where (*this == LHS) and (RHS == "").
    ///
    /// \param Separator The character to split on.
    /// \returns The split substrings.
    KAIJU_NODISCARD
    std::pair<StringRef, StringRef> split(char Separator) const {
      size_t Idx = find(Separator);
      if (Idx == npos)
        return std::make_pair(*this, StringRef());
      return std::make_pair(slice(0, Idx), slice(Idx+1, npos));
    }

    /// Split into two substrings around the first occurrence of a separator
    /// string.
    ///
    /// If \p Separator is in the string, then the result is a pair (LHS, RHS)
    /// such that (*this == LHS + Separator + RHS) is true and RHS is
    /// maximal. If \p Separator is not in the string, then the result is a
    /// pair (LHS, RHS) where (*this == LHS) and (RHS == "").
    ///
    /// \param Separator - The string to split on.
    /// \return - The split substrings.
    KAIJU_NODISCARD
    std::pair<StringRef, StringRef> split(StringRef Separator) const {
      size_t Idx = find(Separator);
      if (Idx == npos)
        return std::make_pair(*this, StringRef());
      return std::make_pair(slice(0, Idx), slice(Idx + Separator.size(), npos));
    }

    /// Split into two substrings around the last occurrence of a separator
    /// character.
    ///
    /// If \p Separator is in the string, then the result is a pair (LHS, RHS)
    /// such that (*this == LHS + Separator + RHS) is true and RHS is
    /// minimal. If \p Separator is not in the string, then the result is a
    /// pair (LHS, RHS) where (*this == LHS) and (RHS == "").
    ///
    /// \param Separator - The character to split on.
    /// \return - The split substrings.
    KAIJU_NODISCARD
    std::pair<StringRef, StringRef> rsplit(char Separator) const {
      size_t Idx = rfind(Separator);
      if (Idx == npos)
        return std::make_pair(*this, StringRef());
      return std::make_pair(slice(0, Idx), slice(Idx+1, npos));
    }

    /// Return string with consecutive \p Char characters starting from the
    /// the left removed.
    KAIJU_NODISCARD
    StringRef ltrim(char Char) const {
      return drop_front(std::min(Length, find_first_not_of(Char)));
    }

    /// Return string with consecutive characters in \p Chars starting from
    /// the left removed.
    KAIJU_NODISCARD
    StringRef ltrim(StringRef Chars = " \t\n\v\f\r") const {
      return drop_front(std::min(Length, find_first_not_of(Chars)));
    }

    /// Return string with consecutive \p Char characters starting from the
    /// right removed.
    KAIJU_NODISCARD
    StringRef rtrim(char Char) const {
      return drop_back(Length - std::min(Length, find_last_not_of(Char) + 1));
    }

    /// Return string with consecutive characters in \p Chars starting from
    /// the right removed.
    KAIJU_NODISCARD
    StringRef rtrim(StringRef Chars = " \t\n\v\f\r") const {
      return drop_back(Length - std::min(Length, find_last_not_of(Chars) + 1));
    }

    /// Return string with consecutive \p Char characters starting from the
    /// left and right removed.
    KAIJU_NODISCARD
    StringRef trim(char Char) const {
      return ltrim(Char).rtrim(Char);
    }

    /// Return string with consecutive characters in \p Chars starting from
    /// the left and right removed.
    KAIJU_NODISCARD
    StringRef trim(StringRef Chars = " \t\n\v\f\r") const {
      return ltrim(Chars).rtrim(Chars);
    }

    /// @}
  };

  /// A wrapper around a string literal that serves as a proxy for constructing
  /// global tables of StringRefs with the length computed at compile time.
  /// In order to avoid the invocation of a global constructor, StringLiteral
  /// should *only* be used in a constexpr context, as such:
  ///
  /// constexpr StringLiteral S("test");
  ///
  class StringLiteral : public StringRef {
  private:
    constexpr StringLiteral(const char *Str, size_t N) : StringRef(Str, N) {
    }

  public:
    template <size_t N>
    constexpr StringLiteral(const char (&Str)[N])
#if defined(__clang__) && __has_attribute(enable_if)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgcc-compat"
        __attribute((enable_if(__builtin_strlen(Str) == N - 1,
                               "invalid string literal")))
#pragma clang diagnostic pop
#endif
        : StringRef(Str, N - 1) {
    }

    // Explicit construction for strings like "foo\0bar".
    template <size_t N>
    static constexpr StringLiteral withInnerNUL(const char (&Str)[N]) {
      return StringLiteral(Str, N - 1);
    }
  };

  /// @name StringRef Comparison Operators
  /// @{

  KAIJU_ATTRIBUTE_ALWAYS_INLINE
  inline bool operator==(StringRef LHS, StringRef RHS) {
    return LHS.equals(RHS);
  }

  KAIJU_ATTRIBUTE_ALWAYS_INLINE
  inline bool operator!=(StringRef LHS, StringRef RHS) { return !(LHS == RHS); }

  inline bool operator<(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) == -1;
  }

  inline bool operator<=(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) != 1;
  }

  inline bool operator>(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) == 1;
  }

  inline bool operator>=(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) != -1;
  }

  inline std::string &operator+=(std::string &buffer, StringRef string) {
    return buffer.append(string.data(), string.size());
  }

  /// @}

  // StringRefs can be treated like a POD type.
  template <typename T> struct isPodLike;
  template <> struct isPodLike<StringRef> { static const bool value = true; };

} // end namespace kaiju

#endif // KAIJU_ADT_STRINGREF_H
