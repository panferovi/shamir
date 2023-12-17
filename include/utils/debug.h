#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

namespace shagit {
[[noreturn]] void AssertionFail(const char *expr, const char *file, unsigned line, const char *function);
}  // namespace shagit

#endif  // INCLUDE_DEBUG_H
