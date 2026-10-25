#pragma once

#include "tuix/components/component.hpp"
#include "tuix/rendering/color.hpp"
#include "tuix/rendering/rect.hpp"

namespace tuix {

struct TBoxProps {
  Rect bounds;
  Color background;
  float radius = 0.0f;
};

class TBox final : public TComponent {
public:
  explicit TBox(TBoxProps props);

  void Paint(TRenderer &renderer) const override;

private:
  TBoxProps props_;
};

} // namespace tuix