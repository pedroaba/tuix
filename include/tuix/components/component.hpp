#pragma once

#include "tuix/layout/alignment.hpp"
#include "tuix/layout/size.hpp"
#include "tuix/rendering/rect.hpp"
#include <optional>

namespace tuix {

class TRenderer;

class TComponent {
public:
  virtual ~TComponent() = default;

  virtual TSize Measure() const = 0;
  virtual void Arrange(TRect bounds) = 0;

  virtual void Paint(TRenderer &renderer) const = 0;

  void SetFlexGrow(float grow) {
    if (grow < 0) {
      return;
    }

    flexGrow_ = grow;
  }

  void SetAlignSelf(TLayoutAlignment alignment) { alignSelf_ = alignment; }

  float GetFlexGrow() const { return flexGrow_; };
  std::optional<TLayoutAlignment> GetAlignSelf() const { return alignSelf_; }

private:
  int flexGrow_ = 0;
  std::optional<TLayoutAlignment> alignSelf_;
};

} // namespace tuix