#pragma once

#include "tuix/layout/size.hpp"
#include "tuix/rendering/rect.hpp"

namespace tuix {

class TRenderer;

class TComponent {
public:
  virtual ~TComponent() = default;

  virtual Size Measure() const = 0;
  virtual void Arrange(Rect bounds) = 0;

  virtual void Paint(TRenderer &renderer) const = 0;
};

} // namespace tuix