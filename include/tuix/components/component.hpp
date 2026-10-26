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

  void SetFlexGrow(float grow) { flexGrow_ = grow > 0 ? grow : 0; }
  void SetAlignSelf(TLayoutAlignment alignment) { alignSelf_ = alignment; }
  void SetFlexShrink(float shrink) { flexShrink_ = shrink > 0 ? shrink : 0; }

  float GetFlexGrow() const { return flexGrow_; };
  std::optional<TLayoutAlignment> GetAlignSelf() const { return alignSelf_; }
  float GetFlexShrink() const { return flexShrink_; }

private:
  float flexGrow_ = 0.0f;
  float flexShrink_ = 1.0f;
  std::optional<TLayoutAlignment> alignSelf_;
};

} // namespace tuix