#pragma once

#include "tuix/core/factory/float.hpp"
namespace tuix {

struct TSize {
  float width = tuix::make_default_float();
  float height = tuix::make_default_float();
};

} // namespace tuix