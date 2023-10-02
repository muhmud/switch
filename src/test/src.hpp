#ifndef _SWITCH_TEST_SRC_HPP_
#define _SWITCH_TEST_SRC_HPP_
#include <cstdlib>
#include <memory>

// Source headers
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
