#pragma once

#include "tuix/components/component.hpp"
#include "tuix/rendering/color.hpp"

namespace tuix {

struct TBoxProps {
  Size size;
  Color background;
  float radius = 0.0f;
};

class TBox final : public TComponent {
public:
  explicit TBox(TBoxProps props);

  Size Measure() const override;
  void Arrange(Rect bounds) override;

  void Paint(TRenderer &renderer) const override;

private:
  TBoxProps props_;
  Rect bounds_{};
};

} // namespace tuix