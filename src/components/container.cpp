#include "tuix/components/container.hpp"
#include "tuix/core/utility/conversions.hpp"
#include "tuix/layout/alignment.hpp"
#include "tuix/layout/direction.hpp"
#include "tuix/layout/size.hpp"

#include <algorithm>
#include <utility>

namespace tuix {

TContainer::TContainer(TContainerProps props) : props_(props) {}

void TContainer::AddChild(std::unique_ptr<TComponent> child) {
  if (!child) {
    return;
  }

  children_.push_back(std::move(child));
}

TSize TContainer::Measure() const {
  // formulas:

  // Vertical
  // W = max(children_widths) + 2(padding)
  // H = sum(children_heights) + 2(padding) + (N - 1)(gap)

  // Horizontal
  // H = max(children_heights) + 2(padding)
  // W = sum(children_widths) + 2(padding) + (N - 1)(gap)

  // Descobrir a maior largura entre os filhos
  const int totalOfChildren = GetTotalOfChildren();
  float containerWidth = 0.0f;
  float containerHeight = 0.0f;

  for (int i = 0; i < totalOfChildren; i++) {
    float currentChildWidth = children_.at(i)->Measure().width;
    float currentChildHeight = children_.at(i)->Measure().height;

    switch (props_.direction) {
    case tuix::TLayoutDirection::Vertical:
      if (containerWidth < currentChildWidth) {
        containerWidth = currentChildWidth;
      }

      containerHeight += currentChildHeight;
      break;
    case tuix::TLayoutDirection::Horizontal:
      if (containerHeight < currentChildHeight) {
        containerHeight = currentChildHeight;
      }

      containerWidth += currentChildWidth;
      break;
    }
  }

  if (props_.direction == tuix::TLayoutDirection::Vertical) {
    // Adicionar gaps somente entre os filhos
    containerWidth += 2 * props_.padding;

    // Adicionar padding nas duas extremidades
    float gapMultiplier = std::max(0, totalOfChildren - 1);
    containerHeight += (2 * props_.padding) + (gapMultiplier * props_.gap);
  } else if (props_.direction == tuix::TLayoutDirection::Horizontal) {
    // Adicionar gaps somente entre os filhos
    containerHeight += 2 * props_.padding;

    // Adicionar padding nas duas extremidades
    float gapMultiplier = std::max(0, totalOfChildren - 1);
    containerWidth += (2 * props_.padding) + (gapMultiplier * props_.gap);
  }

  return tuix::TSize{.width = containerWidth, .height = containerHeight};
}

int TContainer::GetTotalOfChildren() const { return children_.size(); }

std::vector<float> TContainer::ResolveMainSizes(float availableMainSize) const {
  const bool isVertical = props_.direction == TLayoutDirection::Vertical;
  const int gapCount = std::max(0, GetTotalOfChildren() - 1);

  // Espaço disponível, descontando os gaps
  const float availableForChildren =
      std::max(0.0f, availableMainSize - gapCount * props_.gap);

  std::vector<float> baseSizes;
  baseSizes.reserve(children_.size());

  float totalBaseSize = 0.0f;
  float totalGrow = 0.0f;
  float totalScaledShrink = 0.0f;

  // 1. Medir os filhos
  for (const auto &child : children_) {
    const TSize size = child->Measure();

    const float baseSize = isVertical ? size.height : size.width;

    baseSizes.push_back(baseSize);

    totalBaseSize += baseSize;
    totalGrow += child->GetFlexGrow();

    totalScaledShrink += child->GetFlexShrink() * baseSize;
  }

  // 2. Calcular espaço sobrando ou faltante
  const float freeSpace = std::max(0.0f, availableForChildren - totalBaseSize);
  const float deficit = std::max(0.0f, totalBaseSize - availableForChildren);

  // 3. Resolver as dimensões finais
  std::vector<float> finalSizes;
  finalSizes.reserve(children_.size());

  for (std::size_t i = 0; i < children_.size(); ++i) {
    const auto &child = children_[i];

    float finalSize = baseSizes[i];

    if (freeSpace > 0.0f && totalGrow > 0.0f) {
      finalSize += freeSpace * (child->GetFlexGrow() / totalGrow);
    } else if (deficit > 0.0f && totalScaledShrink > 0.0f) {
      const float scaledShrink = child->GetFlexShrink() * baseSizes[i];

      finalSize -= deficit * (scaledShrink / totalScaledShrink);
    }

    // Respeitar o tamanho mínimo no eixo principal
    const float minSize =
        isVertical ? child->GetMinHeight() : child->GetMinWidth();

    finalSize = std::max(finalSize, minSize);
    finalSize = std::max(finalSize, 0.0f);

    finalSizes.push_back(finalSize);
  }

  return finalSizes;
}

void TContainer::Arrange(TRect bounds) {
  bounds_ = bounds;

  // 1. Identificar a direção
  const bool isVertical = props_.direction == TLayoutDirection::Vertical;
  const int totalOfChildren = GetTotalOfChildren();

  // 2. Calcular a área interna
  const float availableWidth = std::max(
      0.0f, bounds_.width - (tuix::convert_to_float(2) * props_.padding));

  const float availableHeight = std::max(
      0.0f, bounds_.height - (tuix::convert_to_float(2) * props_.padding));

  // 3. Identificar os eixos
  const float availableMainSize = isVertical ? availableHeight : availableWidth;

  const float availableCrossSize =
      isVertical ? availableWidth : availableHeight;

  const std::vector<float> mainSizes = ResolveMainSizes(availableMainSize);

  // 5. Incluir os gaps originais
  const int gapCount = std::max(0, totalOfChildren - 1);
  float contentMainSize = gapCount * props_.gap;
  for (float size : mainSizes) {
    contentMainSize += size;
  }

  const float remainingFreeSpace =
      std::max(0.0f, availableMainSize - contentMainSize);

  // 7. Resolver o justify
  float mainOffset = 0.0f;
  float effectiveGap = props_.gap;

  switch (props_.justify) {
  case TLayoutJustify::Start:
    break;

  case TLayoutJustify::Center:
    mainOffset = remainingFreeSpace / tuix::convert_to_float(2);
    break;

  case TLayoutJustify::End:
    mainOffset = remainingFreeSpace;
    break;

  case TLayoutJustify::SpaceBetween:
    if (gapCount > 0) {
      effectiveGap += remainingFreeSpace / gapCount;
    }
    break;
  }

  // 8. Definir a posição inicial
  float currentX = bounds_.x + props_.padding;
  float currentY = bounds_.y + props_.padding;

  // Aplicar o justify no eixo principal
  if (isVertical) {
    currentY += mainOffset;
  } else {
    currentX += mainOffset;
  }

  // 9. Posicionar cada filho
  for (std::size_t i = 0; i < children_.size(); ++i) {
    const auto &child = children_[i];
    TSize childSize = child->Measure();

    if (isVertical) {
      childSize.height = mainSizes.at(i);
    } else {
      childSize.width = mainSizes.at(i);
    }

    const float childCrossSize =
        isVertical ? childSize.width : childSize.height;

    // Resolver o alignment individual
    float crossOffset = 0.0f;
    const TLayoutAlignment childAlignment =
        child->GetAlignSelf().value_or(props_.alignment);

    switch (childAlignment) {
    case TLayoutAlignment::Start:
      break;

    case TLayoutAlignment::Stretch:
      if (isVertical) {
        childSize.width = availableCrossSize;
      } else {
        childSize.height = availableCrossSize;
      }
      break;

    case TLayoutAlignment::Center:
      crossOffset =
          (availableCrossSize - childCrossSize) / tuix::convert_to_float(2);
      break;

    case TLayoutAlignment::End:
      crossOffset = availableCrossSize - childCrossSize;
      break;
    }

    // if (child->GetMinHeight() > 0) {
    //   childSize.height = std::max(childSize.height, child->GetMinHeight());
    // }

    // if (child->GetMinWidth() > 0) {
    //   childSize.width = std::max(childSize.width, child->GetMinWidth());
    // }

    // 10. Construir o Rect final
    TRect childBounds{.x = currentX + (isVertical ? crossOffset : 0.0f),
                      .y = currentY + (isVertical ? 0.0f : crossOffset),
                      .width = childSize.width,
                      .height = childSize.height};

    child->Arrange(childBounds);

    // 11. Avançar para o próximo filho
    if (isVertical) {
      currentY += childSize.height + effectiveGap;
    } else {
      currentX += childSize.width + effectiveGap;
    }
  }
}

void TContainer::Paint(TRenderer &renderer) const {
  const int quantityOfChildren = children_.size();
  for (int i = 0; i < quantityOfChildren; i++) {
    children_.at(i)->Paint(renderer);
  }
}

} // namespace tuix