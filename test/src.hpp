#ifndef _SWITCH_TEST_SRC_H_
#define _SWITCH_TEST_SRC_H_
#include <memory>
#include <cstdlib>

// Source headers
extern "C" {
#include "../src/stack.h"
}

// Helper functions
template <typename T> std::unique_ptr<T, void (*)(void *)> allocate(T *ptr) {
  return std::unique_ptr<T, void (*)(void *)>(ptr, std::free);
}

#endif // _SWITCH_TEST_SRC_H_
