#pragma once

#include <memory>
#include <vector>

#include "tuix/components/component.hpp"
#include "tuix/layout/alignment.hpp"
#include "tuix/layout/direction.hpp"
#include "tuix/layout/justify.hpp"

constexpr float DEFAULT_TCONTAINER_PADDING = 16.0f;
constexpr float DEFAULT_TCONTAINER_GAP = 12.0f;

namespace tuix {

struct TContainerProps {
  float padding = DEFAULT_TCONTAINER_PADDING;
  float gap = DEFAULT_TCONTAINER_GAP;
  tuix::TLayoutDirection direction = TLayoutDirection::Vertical;
  tuix::TLayoutAlignment alignment = TLayoutAlignment::Start;
  tuix::TLayoutJustify justify = TLayoutJustify::Start;
};

class TContainer final : public TComponent {
public:
  explicit TContainer(TContainerProps props = {});

  TSize Measure() const override;
  void Arrange(TRect bounds) override;

  void AddChild(std::unique_ptr<TComponent> child);

  void Paint(TRenderer &renderer) const override;

  int GetTotalOfChildren() const;

private:
  std::vector<std::unique_ptr<TComponent>> children_;

  TContainerProps props_;
  TRect bounds_{};
};

} // namespace tuix