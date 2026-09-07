#ifndef _SWITCH_TEST_SRC_HPP_
#define _SWITCH_TEST_SRC_HPP_
#include <cstdlib>
#include <memory>

// Source headers
/* atomic_int, as used by struct App, must come from <atomic> in C++ — and from
   outside the extern "C" block, since it is a template. */
#include <atomic>
using std::atomic_int;

extern "C" {
#include "../app.h"
#include "../app_keymap.h"
#include "../mods.h"
#include "../stack.h"
}

// Helper functions
template <typename T> std::unique_ptr<T, void (*)(void *)> allocate(T *ptr) {
  return std::unique_ptr<T, void (*)(void *)>(ptr, std::free);
}

#endif // _SWITCH_TEST_SRC_HPP_
