#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

namespace shamir {
[[noreturn]] void AssertionFail(const char *expr, const char *file, unsigned line, const char *function);
}  // namespace shamir

#endif  // INCLUDE_DEBUG_H
