#include "tuix/components/box.hpp"
#include "tuix/rendering/renderer.hpp"

namespace tuix {

TBox::TBox(TBoxProps props) : props_(props) {}

void TBox::Paint(TRenderer &renderer) const {
  renderer.FillRoundedRect(props_.bounds, props_.radius, props_.background);
}

} // namespace tuix