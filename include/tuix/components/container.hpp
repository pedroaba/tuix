#pragma once

#include <memory>
#include <vector>

#include "tuix/components/component.hpp"
#include "tuix/layout/direction.hpp"

constexpr float DEFAULT_TCONTAINER_PADDING = 16.0f;
constexpr float DEFAULT_TCONTAINER_GAP = 12.0f;

namespace tuix {

struct TContainerProps {
  float padding = DEFAULT_TCONTAINER_PADDING;
  float gap = DEFAULT_TCONTAINER_GAP;
  tuix::TLayoutDirection direction = TLayoutDirection::Vertical;
};

class TContainer final : public TComponent {
public:
  explicit TContainer(TContainerProps props = {});

  Size Measure() const override;
  void Arrange(Rect bounds) override;

  void AddChild(std::unique_ptr<TComponent> child);

  void Paint(TRenderer &renderer) const override;

  int GetTotalOfChildren() const;

private:
  std::vector<std::unique_ptr<TComponent>> children_;

  TContainerProps props_;
  Rect bounds_{};
};

} // namespace tuix