#pragma once

#include <memory>
#include <vector>

#include "tuix/components/component.hpp"

namespace tuix {

class TContainer final : public TComponent {
public:
  void AddChild(std::unique_ptr<TComponent> child);

  void Paint(TRenderer &renderer) const override;

private:
  std::vector<std::unique_ptr<TComponent>> children_;
};

} // namespace tuix