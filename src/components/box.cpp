#include "tuix/components/box.hpp"
#include "tuix/rendering/renderer.hpp"

namespace tuix {

TBox::TBox(TBoxProps props) : props_(props) {}

tuix::Size TBox::Measure() const { return props_.size; }

void TBox::Arrange(tuix::Rect rect) { bounds_ = rect; }

void TBox::Paint(TRenderer &renderer) const {
  renderer.FillRoundedRect(bounds_, props_.radius, props_.background);
}

} // namespace tuix