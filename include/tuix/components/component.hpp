#pragma once

namespace tuix {

class TRenderer;

class TComponent {
public:
  virtual ~TComponent() = default;

  virtual void Paint(TRenderer &renderer) const = 0;
};

} // namespace tuix