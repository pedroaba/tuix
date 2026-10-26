#pragma once

#include <algorithm>

#include "tuix/layout/alignment.hpp"
#include "tuix/layout/size.hpp"
#include "tuix/rendering/rect.hpp"
#include <limits>
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

  void SetMinSize(TSize size) {
    if (size.height >= maxSize_.height || size.width >= maxSize_.width) {
      return;
    }

    minSize_.width = std::max(0.0f, size.width);
    minSize_.height = std::max(0.0f, size.height);
  }

  void SetMinWidth(float minWidth) {
    if (minWidth >= 0 && minWidth <= maxSize_.width) {
      minSize_.width = minWidth;
    }
  }

  void SetMinHeight(float minHeight) {
    if (minHeight >= 0 && minHeight <= maxSize_.height) {
      minSize_.height = minHeight;
    }
  }

  void SetMaxSize(TSize size) {
    if (size.height <= minSize_.height || size.width <= minSize_.width) {
      return;
    }

    minSize_.width = std::max(0.0f, size.width);
    minSize_.height = std::max(0.0f, size.height);
  }

  void SetMaxWidth(float maxWidth) {
    if (maxWidth >= 0 && maxWidth >= minSize_.width) {
      maxSize_.width = maxWidth;
    }
  }

  void SetMaxHeight(float maxHeight) {
    if (maxHeight >= 0 && maxHeight >= minSize_.height) {
      maxSize_.height = maxHeight;
    }
  }

  TSize GetMinSize() const { return minSize_; }
  float GetMinWidth() const { return minSize_.width; }
  float GetMinHeight() const { return minSize_.height; }

  TSize GetMaxSize() const { return maxSize_; }
  float GetMaxWidth() const { return maxSize_.width; }
  float GetMaxHeight() const { return maxSize_.height; }

private:
  float flexGrow_ = 0.0f;
  float flexShrink_ = 1.0f;
  std::optional<TLayoutAlignment> alignSelf_;

  tuix::TSize minSize_ = {.width = 0.0f, .height = 0.0f};
  tuix::TSize maxSize_ = {.width = std::numeric_limits<float>::infinity(),
                          .height = std::numeric_limits<float>::infinity()};
};

} // namespace tuix