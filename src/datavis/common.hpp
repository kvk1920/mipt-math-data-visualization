#pragma once

#include <stdexcept>

inline void Verify(bool expr, const char* msg) {
  if (!expr) {
    throw std::logic_error(msg);
  }
}

#define STRINGIFY(x) STRINGIFY_IMPL(x)
#define STRINGIFY_IMPL(x) #x

#define VERIFY(expr) Verify((expr), __FILE__ ":" STRINGIFY(__LINE__) ": Verification failed: \"" #expr "\"")
