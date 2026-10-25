#pragma once

#include "tuix/components/component.hpp"
#include "tuix/rendering/color.hpp"

namespace tuix {

struct TBoxProps {
  TSize size;
  TColor background;
  float radius = 0.0f;
};

class TBox final : public TComponent {
public:
  explicit TBox(TBoxProps props);

  TSize Measure() const override;
  void Arrange(TRect bounds) override;

  void Paint(TRenderer &renderer) const override;

private:
  TBoxProps props_;
  TRect bounds_{};
};

} // namespace tuix