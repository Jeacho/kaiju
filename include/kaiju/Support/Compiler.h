
#ifndef KAIJU_SUPPORT_COMPILER_H
#define KAIJU_SUPPORT_COMPILER_H

#if defined(_MSC_VER)
#include <sal.h>
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#ifndef __has_extension
# define __has_extension(x) 0
#endif

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

#ifndef __has_cpp_attribute
# define __has_cpp_attribute(x) 0
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

/// \macro KAIJU_GNUC_PREREQ
/// \brief Extend the default __GNUC_PREREQ even if glibc's features.h isn't
/// available.
#ifndef KAIJU_GNUC_PREREQ
# if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#  define KAIJU_GNUC_PREREQ(maj, min, patch) \
    ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >= \
     ((maj) << 20) + ((min) << 10) + (patch))
# elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define KAIJU_GNUC_PREREQ(maj, min, patch) \
    ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))
# else
#  define KAIJU_GNUC_PREREQ(maj, min, patch) 0
# endif
#endif

/// \macro KAIJU_MSC_PREREQ
/// \brief Is the compiler MSVC of at least the specified version?
/// The common \param version values to check for are:
///  * 1900: Microsoft Visual Studio 2015 / 14.0
#ifdef _MSC_VER
#define KAIJU_MSC_PREREQ(version) (_MSC_VER >= (version))

// We require at least MSVC 2015.
#if !KAIJU_MSC_PREREQ(1900)
#error KAIJU requires at least MSVC 2015.
#endif

#else
#define KAIJU_MSC_PREREQ(version) 0
#endif

/// \brief Does the compiler support ref-qualifiers for *this?
///
/// Sadly, this is separate from just rvalue reference support because GCC
/// and MSVC implemented this later than everything else.
#if __has_feature(cxx_rvalue_references) || KAIJU_GNUC_PREREQ(4, 8, 1)
#define KAIJU_HAS_RVALUE_REFERENCE_THIS 1
#else
#define KAIJU_HAS_RVALUE_REFERENCE_THIS 0
#endif

/// Expands to '&' if ref-qualifiers for *this are supported.
///
/// This can be used to provide lvalue/rvalue overrides of member functions.
/// The rvalue override should be guarded by KAIJU_HAS_RVALUE_REFERENCE_THIS
#if KAIJU_HAS_RVALUE_REFERENCE_THIS
#define KAIJU_LVALUE_FUNCTION &
#else
#define KAIJU_LVALUE_FUNCTION
#endif

/// KAIJU_LIBRARY_VISIBILITY - If a class marked with this attribute is linked
/// into a shared library, then the class should be private to the library and
/// not accessible from outside it.  Can also be used to mark variables and
/// functions, making them private to any shared library they are linked into.
/// On PE/COFF targets, library visibility is the default, so this isn't needed.
#if (__has_attribute(visibility) || KAIJU_GNUC_PREREQ(4, 0, 0)) &&              \
    !defined(__MINGW32__) && !defined(__CYGWIN__) && !defined(KAIJU_ON_WIN32)
#define KAIJU_LIBRARY_VISIBILITY __attribute__ ((visibility("hidden")))
#else
#define KAIJU_LIBRARY_VISIBILITY
#endif

#if defined(__GNUC__)
#define KAIJU_PREFETCH(addr, rw, locality) __builtin_prefetch(addr, rw, locality)
#else
#define KAIJU_PREFETCH(addr, rw, locality)
#endif

#if __has_attribute(used) || KAIJU_GNUC_PREREQ(3, 1, 0)
#define KAIJU_ATTRIBUTE_USED __attribute__((__used__))
#else
#define KAIJU_ATTRIBUTE_USED
#endif

/// KAIJU_NODISCARD - Warn if a type or return value is discarded.
#if __cplusplus > 201402L && __has_cpp_attribute(nodiscard)
#define KAIJU_NODISCARD [[nodiscard]]
#elif !__cplusplus
// Workaround for kaiju.org/PR23435, since clang 3.6 and below emit a spurious
// error when __has_cpp_attribute is given a scoped attribute in C mode.
#define KAIJU_NODISCARD
#elif __has_cpp_attribute(clang::warn_unused_result)
#define KAIJU_NODISCARD [[clang::warn_unused_result]]
#else
#define KAIJU_NODISCARD
#endif

// Some compilers warn about unused functions. When a function is sometimes
// used or not depending on build settings (e.g. a function only called from
// within "assert"), this attribute can be used to suppress such warnings.
//
// However, it shouldn't be used for unused *variables*, as those have a much
// more portable solution:
//   (void)unused_var_name;
// Prefer cast-to-void wherever it is sufficient.
#if __has_attribute(unused) || KAIJU_GNUC_PREREQ(3, 1, 0)
#define KAIJU_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#define KAIJU_ATTRIBUTE_UNUSED
#endif

// FIXME: Provide this for PE/COFF targets.
#if (__has_attribute(weak) || KAIJU_GNUC_PREREQ(4, 0, 0)) &&                    \
    (!defined(__MINGW32__) && !defined(__CYGWIN__) && !defined(KAIJU_ON_WIN32))
#define KAIJU_ATTRIBUTE_WEAK __attribute__((__weak__))
#else
#define KAIJU_ATTRIBUTE_WEAK
#endif

// Prior to clang 3.2, clang did not accept any spelling of
// __has_attribute(const), so assume it is supported.
#if defined(__clang__) || defined(__GNUC__)
// aka 'CONST' but following KAIJU Conventions.
#define KAIJU_READNONE __attribute__((__const__))
#else
#define KAIJU_READNONE
#endif

#if __has_attribute(pure) || defined(__GNUC__)
// aka 'PURE' but following KAIJU Conventions.
#define KAIJU_READONLY __attribute__((__pure__))
#else
#define KAIJU_READONLY
#endif

#if __has_builtin(__builtin_expect) || KAIJU_GNUC_PREREQ(4, 0, 0)
#define KAIJU_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
#define KAIJU_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
#else
#define KAIJU_LIKELY(EXPR) (EXPR)
#define KAIJU_UNLIKELY(EXPR) (EXPR)
#endif

/// KAIJU_ATTRIBUTE_NOINLINE - On compilers where we have a directive to do so,
/// mark a method "not for inlining".
#if __has_attribute(noinline) || KAIJU_GNUC_PREREQ(3, 4, 0)
#define KAIJU_ATTRIBUTE_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define KAIJU_ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define KAIJU_ATTRIBUTE_NOINLINE
#endif

/// KAIJU_ATTRIBUTE_ALWAYS_INLINE - On compilers where we have a directive to do
/// so, mark a method "always inline" because it is performance sensitive. GCC
/// 3.4 supported this but is buggy in various cases and produces unimplemented
/// errors, just use it in GCC 4.0 and later.
#if __has_attribute(always_inline) || KAIJU_GNUC_PREREQ(4, 0, 0)
#define KAIJU_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define KAIJU_ATTRIBUTE_ALWAYS_INLINE __forceinline
#else
#define KAIJU_ATTRIBUTE_ALWAYS_INLINE
#endif

#ifdef __GNUC__
#define KAIJU_ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define KAIJU_ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define KAIJU_ATTRIBUTE_NORETURN
#endif

#if __has_attribute(returns_nonnull) || KAIJU_GNUC_PREREQ(4, 9, 0)
#define KAIJU_ATTRIBUTE_RETURNS_NONNULL __attribute__((returns_nonnull))
#elif defined(_MSC_VER)
#define KAIJU_ATTRIBUTE_RETURNS_NONNULL _Ret_notnull_
#else
#define KAIJU_ATTRIBUTE_RETURNS_NONNULL
#endif

/// \macro KAIJU_ATTRIBUTE_RETURNS_NOALIAS Used to mark a function as returning a
/// pointer that does not alias any other valid pointer.
#ifdef __GNUC__
#define KAIJU_ATTRIBUTE_RETURNS_NOALIAS __attribute__((__malloc__))
#elif defined(_MSC_VER)
#define KAIJU_ATTRIBUTE_RETURNS_NOALIAS __declspec(restrict)
#else
#define KAIJU_ATTRIBUTE_RETURNS_NOALIAS
#endif

/// KAIJU_FALLTHROUGH - Mark fallthrough cases in switch statements.
#if __cplusplus > 201402L && __has_cpp_attribute(fallthrough)
#define KAIJU_FALLTHROUGH [[fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
#define KAIJU_FALLTHROUGH [[gnu::fallthrough]]
#elif !__cplusplus
// Workaround for kaiju.org/PR23435, since clang 3.6 and below emit a spurious
// error when __has_cpp_attribute is given a scoped attribute in C mode.
#define KAIJU_FALLTHROUGH
#elif __has_cpp_attribute(clang::fallthrough)
#define KAIJU_FALLTHROUGH [[clang::fallthrough]]
#else
#define KAIJU_FALLTHROUGH
#endif

/// KAIJU_EXTENSION - Support compilers where we have a keyword to suppress
/// pedantic diagnostics.
#ifdef __GNUC__
#define KAIJU_EXTENSION __extension__
#else
#define KAIJU_EXTENSION
#endif

// KAIJU_ATTRIBUTE_DEPRECATED(decl, "message")
#if __has_feature(attribute_deprecated_with_message)
# define KAIJU_ATTRIBUTE_DEPRECATED(decl, message) \
  decl __attribute__((deprecated(message)))
#elif defined(__GNUC__)
# define KAIJU_ATTRIBUTE_DEPRECATED(decl, message) \
  decl __attribute__((deprecated))
#elif defined(_MSC_VER)
# define KAIJU_ATTRIBUTE_DEPRECATED(decl, message) \
  __declspec(deprecated(message)) decl
#else
# define KAIJU_ATTRIBUTE_DEPRECATED(decl, message) \
  decl
#endif

/// KAIJU_BUILTIN_UNREACHABLE - On compilers which support it, expands
/// to an expression which states that it is undefined behavior for the
/// compiler to reach this point.  Otherwise is not defined.
#if __has_builtin(__builtin_unreachable) || KAIJU_GNUC_PREREQ(4, 5, 0)
# define KAIJU_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
# define KAIJU_BUILTIN_UNREACHABLE __assume(false)
#endif

/// KAIJU_BUILTIN_TRAP - On compilers which support it, expands to an expression
/// which causes the program to exit abnormally.
#if __has_builtin(__builtin_trap) || KAIJU_GNUC_PREREQ(4, 3, 0)
# define KAIJU_BUILTIN_TRAP __builtin_trap()
#elif defined(_MSC_VER)
// The __debugbreak intrinsic is supported by MSVC, does not require forward
// declarations involving platform-specific typedefs (unlike RaiseException),
// results in a call to vectored exception handlers, and encodes to a short
// instruction that still causes the trapping behavior we want.
# define KAIJU_BUILTIN_TRAP __debugbreak()
#else
# define KAIJU_BUILTIN_TRAP *(volatile int*)0x11 = 0
#endif

/// KAIJU_BUILTIN_DEBUGTRAP - On compilers which support it, expands to
/// an expression which causes the program to break while running
/// under a debugger.
#if __has_builtin(__builtin_debugtrap)
# define KAIJU_BUILTIN_DEBUGTRAP __builtin_debugtrap()
#elif defined(_MSC_VER)
// The __debugbreak intrinsic is supported by MSVC and breaks while
// running under the debugger, and also supports invoking a debugger
// when the OS is configured appropriately.
# define KAIJU_BUILTIN_DEBUGTRAP __debugbreak()
#else
// Just continue execution when built with compilers that have no
// support. This is a debugging aid and not intended to force the
// program to abort if encountered.
# define KAIJU_BUILTIN_DEBUGTRAP
#endif

/// \macro KAIJU_ASSUME_ALIGNED
/// \brief Returns a pointer with an assumed alignment.
#if __has_builtin(__builtin_assume_aligned) || KAIJU_GNUC_PREREQ(4, 7, 0)
# define KAIJU_ASSUME_ALIGNED(p, a) __builtin_assume_aligned(p, a)
#elif defined(KAIJU_BUILTIN_UNREACHABLE)
// As of today, clang does not support __builtin_assume_aligned.
# define KAIJU_ASSUME_ALIGNED(p, a) \
           (((uintptr_t(p) % (a)) == 0) ? (p) : (KAIJU_BUILTIN_UNREACHABLE, (p)))
#else
# define KAIJU_ASSUME_ALIGNED(p, a) (p)
#endif

/// \macro KAIJU_ALIGNAS
/// \brief Used to specify a minimum alignment for a structure or variable.
#if __GNUC__ && !__has_feature(cxx_alignas) && !KAIJU_GNUC_PREREQ(4, 8, 1)
# define KAIJU_ALIGNAS(x) __attribute__((aligned(x)))
#else
# define KAIJU_ALIGNAS(x) alignas(x)
#endif

/// \macro KAIJU_PACKED
/// \brief Used to specify a packed structure.
/// KAIJU_PACKED(
///    struct A {
///      int i;
///      int j;
///      int k;
///      long long l;
///   });
///
/// KAIJU_PACKED_START
/// struct B {
///   int i;
///   int j;
///   int k;
///   long long l;
/// };
/// KAIJU_PACKED_END
#ifdef _MSC_VER
# define KAIJU_PACKED(d) __pragma(pack(push, 1)) d __pragma(pack(pop))
# define KAIJU_PACKED_START __pragma(pack(push, 1))
# define KAIJU_PACKED_END   __pragma(pack(pop))
#else
# define KAIJU_PACKED(d) d __attribute__((packed))
# define KAIJU_PACKED_START _Pragma("pack(push, 1)")
# define KAIJU_PACKED_END   _Pragma("pack(pop)")
#endif

/// \macro KAIJU_PTR_SIZE
/// \brief A constant integer equivalent to the value of sizeof(void*).
/// Generally used in combination with KAIJU_ALIGNAS or when doing computation in
/// the preprocessor.
#ifdef __SIZEOF_POINTER__
# define KAIJU_PTR_SIZE __SIZEOF_POINTER__
#elif defined(_WIN64)
# define KAIJU_PTR_SIZE 8
#elif defined(_WIN32)
# define KAIJU_PTR_SIZE 4
#elif defined(_MSC_VER)
# error "could not determine KAIJU_PTR_SIZE as a constant int for MSVC"
#else
# define KAIJU_PTR_SIZE sizeof(void *)
#endif

/// \macro KAIJU_MEMORY_SANITIZER_BUILD
/// \brief Whether KAIJU itself is built with MemorySanitizer instrumentation.
#if __has_feature(memory_sanitizer)
# define KAIJU_MEMORY_SANITIZER_BUILD 1
# include <sanitizer/msan_interface.h>
#else
# define KAIJU_MEMORY_SANITIZER_BUILD 0
# define __msan_allocated_memory(p, size)
# define __msan_unpoison(p, size)
#endif

/// \macro KAIJU_ADDRESS_SANITIZER_BUILD
/// \brief Whether KAIJU itself is built with AddressSanitizer instrumentation.
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
# define KAIJU_ADDRESS_SANITIZER_BUILD 1
# include <sanitizer/asan_interface.h>
#else
# define KAIJU_ADDRESS_SANITIZER_BUILD 0
# define __asan_poison_memory_region(p, size)
# define __asan_unpoison_memory_region(p, size)
#endif

/// \macro KAIJU_THREAD_SANITIZER_BUILD
/// \brief Whether KAIJU itself is built with ThreadSanitizer instrumentation.
#if __has_feature(thread_sanitizer) || defined(__SANITIZE_THREAD__)
# define KAIJU_THREAD_SANITIZER_BUILD 1
#else
# define KAIJU_THREAD_SANITIZER_BUILD 0
#endif

#if KAIJU_THREAD_SANITIZER_BUILD
// Thread Sanitizer is a tool that finds races in code.
// See http://code.google.com/p/data-race-test/wiki/DynamicAnnotations .
// tsan detects these exact functions by name.
#ifdef __cplusplus
extern "C" {
#endif
void AnnotateHappensAfter(const char *file, int line, const volatile void *cv);
void AnnotateHappensBefore(const char *file, int line, const volatile void *cv);
void AnnotateIgnoreWritesBegin(const char *file, int line);
void AnnotateIgnoreWritesEnd(const char *file, int line);
#ifdef __cplusplus
}
#endif

// This marker is used to define a happens-before arc. The race detector will
// infer an arc from the begin to the end when they share the same pointer
// argument.
# define TsanHappensBefore(cv) AnnotateHappensBefore(__FILE__, __LINE__, cv)

// This marker defines the destination of a happens-before arc.
# define TsanHappensAfter(cv) AnnotateHappensAfter(__FILE__, __LINE__, cv)

// Ignore any races on writes between here and the next TsanIgnoreWritesEnd.
# define TsanIgnoreWritesBegin() AnnotateIgnoreWritesBegin(__FILE__, __LINE__)

// Resume checking for racy writes.
# define TsanIgnoreWritesEnd() AnnotateIgnoreWritesEnd(__FILE__, __LINE__)
#else
# define TsanHappensBefore(cv)
# define TsanHappensAfter(cv)
# define TsanIgnoreWritesBegin()
# define TsanIgnoreWritesEnd()
#endif

/// \macro KAIJU_NO_SANITIZE
/// \brief Disable a particular sanitizer for a function.
#if __has_attribute(no_sanitize)
#define KAIJU_NO_SANITIZE(KIND) __attribute__((no_sanitize(KIND)))
#else
#define KAIJU_NO_SANITIZE(KIND)
#endif

/// \brief Mark debug helper function definitions like dump() that should not be
/// stripped from debug builds.
/// Note that you should also surround dump() functions with
/// `#if !defined(NDEBUG) || defined(KAIJU_ENABLE_DUMP)` so they do always
/// get stripped in release builds.
// FIXME: Move this to a private config.h as it's not usable in public headers.
#if !defined(NDEBUG) || defined(KAIJU_ENABLE_DUMP)
#define KAIJU_DUMP_METHOD KAIJU_ATTRIBUTE_NOINLINE KAIJU_ATTRIBUTE_USED
#else
#define KAIJU_DUMP_METHOD KAIJU_ATTRIBUTE_NOINLINE
#endif

/// \macro KAIJU_PRETTY_FUNCTION
/// \brief Gets a user-friendly looking function signature for the current scope
/// using the best available method on each platform.  The exact format of the
/// resulting string is implementation specific and non-portable, so this should
/// only be used, for example, for logging or diagnostics.
#if defined(_MSC_VER)
#define KAIJU_PRETTY_FUNCTION __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define KAIJU_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define KAIJU_PRETTY_FUNCTION __func__
#endif

/// \macro KAIJU_THREAD_LOCAL
/// \brief A thread-local storage specifier which can be used with globals,
/// extern globals, and static globals.
///
/// This is essentially an extremely restricted analog to C++11's thread_local
/// support, and uses that when available. However, it falls back on
/// platform-specific or vendor-provided extensions when necessary. These
/// extensions don't support many of the C++11 thread_local's features. You
/// should only use this for PODs that you can statically initialize to
/// some constant value. In almost all circumstances this is most appropriate
/// for use with a pointer, integer, or small aggregation of pointers and
/// integers.
#if KAIJU_ENABLE_THREADS
#if __has_feature(cxx_thread_local)
#define KAIJU_THREAD_LOCAL thread_local
#elif defined(_MSC_VER)
// MSVC supports this with a __declspec.
#define KAIJU_THREAD_LOCAL __declspec(thread)
#else
// Clang, GCC, and other compatible compilers used __thread prior to C++11 and
// we only need the restricted functionality that provides.
#define KAIJU_THREAD_LOCAL __thread
#endif
#else // !KAIJU_ENABLE_THREADS
// If threading is disabled entirely, this compiles to nothing and you get
// a normal global variable.
#define KAIJU_THREAD_LOCAL
#endif

/// \macro KAIJU_ENABLE_EXCEPTIONS
/// \brief Whether KAIJU is built with exception support.
#if __has_feature(cxx_exceptions)
#define KAIJU_ENABLE_EXCEPTIONS 1
#elif defined(__GNUC__) && defined(__EXCEPTIONS)
#define KAIJU_ENABLE_EXCEPTIONS 1
#elif defined(_MSC_VER) && defined(_CPPUNWIND)
#define KAIJU_ENABLE_EXCEPTIONS 1
#endif

/// \macro KAIJU_PLUGIN_IMPORT
/// \brief Used to import the well-known entry point for registering loaded pass
/// plugins
#ifdef WIN32
#define KAIJU_PLUGIN_IMPORT __declspec(dllimport)
#else
#define KAIJU_PLUGIN_IMPORT
#endif

/// \macro KAIJU_PLUGIN_EXPORT
/// \brief Used to export the well-known entry point for registering loaded pass
/// plugins
#ifdef WIN32
#define KAIJU_PLUGIN_EXPORT __declspec(dllexport)
#else
#define KAIJU_PLUGIN_EXPORT
#endif

#endif
