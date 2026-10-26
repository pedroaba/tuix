#include "tuix/components/container.hpp"
#include "tuix/core/utility/conversions.hpp"
#include "tuix/layout/alignment.hpp"
#include "tuix/layout/direction.hpp"
#include "tuix/layout/size.hpp"

#include <algorithm>
#include <utility>

namespace {
struct TFlexItem {
  float baseSize = 0.0f;
  float finalSize = 0.0f;

  float minSize = 0.0f;
  float maxSize = 0.0f;

  float grow = 0.0f;
  float shrink = 1.0f;

  bool frozen = false;
};
} // namespace

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
  const float EPSILON = 0.001f;
  const bool isVertical = props_.direction == TLayoutDirection::Vertical;
  const int gapCount = std::max(0, GetTotalOfChildren() - 1);

  // Espaço disponível, descontando os gaps
  const float availableForChildren =
      std::max(0.0f, availableMainSize - gapCount * props_.gap);

  std::vector<float> baseSizes;
  std::vector<TFlexItem> items;

  items.reserve(children_.size());

  float totalBaseSize = 0.0f;
  float totalGrow = 0.0f;
  float totalScaledShrink = 0.0f;

  // 1. Medir os filhos
  for (const auto &child : children_) {
    const TSize size = child->Measure();
    const TSize minSize = child->GetMinSize();
    const TSize maxSize = child->GetMaxSize();

    const float minimum =
        std::max(0.0f, isVertical ? minSize.height : minSize.width);

    const float maximum =
        std::max(minimum, isVertical ? maxSize.height : maxSize.width);

    const float desiredSize = isVertical ? size.height : size.width;
    const float baseSize = std::clamp(desiredSize, minimum, maximum);

    TFlexItem item{.baseSize = baseSize,
                   .finalSize = baseSize,
                   .minSize = minimum,
                   .maxSize = maximum,
                   .grow = std::max(0.0f, child->GetFlexGrow()),
                   .shrink = std::max(0.0f, child->GetFlexShrink()),
                   .frozen = false};
    baseSizes.push_back(baseSize);

    items.push_back(item);
    totalBaseSize += baseSize;
  }

  // 3. Descobrir se precisamos crescer ou encolher
  const float difference = availableForChildren - totalBaseSize;

  const bool growing = difference > EPSILON;
  const bool shrinking = difference < -EPSILON;

  if (growing || shrinking) {
    while (true) {
      // Recalcular usando os itens congelados
      float occupiedSize = 0.0f;
      float totalWeight = 0.0f;

      for (const auto &item : items) {
        occupiedSize += item.frozen ? item.finalSize : item.baseSize;

        if (!item.frozen) {
          totalWeight += growing ? item.grow : item.shrink * item.baseSize;
        }
      }

      // Espaço que ainda precisamos distribuir
      const float remainingSpace = availableForChildren - occupiedSize;

      // Verificar se terminamos
      if (growing && remainingSpace <= EPSILON) {
        break;
      }

      if (shrinking && remainingSpace >= -EPSILON) {
        break;
      }

      // Não há mais componentes elegíveis
      if (totalWeight <= EPSILON) {
        break;
      }

      // Calcular propostas sem alterar os itens
      std::vector<float> proposals(items.size());
      bool anyFrozen = false;

      for (std::size_t i = 0; i < items.size(); ++i) {
        auto &item = items[i];
        if (item.frozen) {
          continue;
        }

        const float weight = growing ? item.grow : item.shrink * item.baseSize;
        const float proposedSize =
            item.baseSize + remainingSpace * (weight / totalWeight);

        proposals[i] = proposedSize;

        // Atingiu o máximo durante o crescimento
        if (growing && proposedSize > item.maxSize + EPSILON) {
          item.finalSize = item.maxSize;
          item.frozen = true;
          anyFrozen = true;
        } else if (shrinking && proposedSize < item.minSize - EPSILON) {
          // Atingiu o mínimo durante o encolhimento
          item.finalSize = item.minSize;
          item.frozen = true;
          anyFrozen = true;
        }
      }

      // Se congelamos algum item, recalcular
      if (anyFrozen) {
        continue;
      }

      // Nenhum item atingiu um limite:
      // aceitar as propostas e finalizar
      for (std::size_t i = 0; i < items.size(); ++i) {
        if (!items[i].frozen) {
          items[i].finalSize =
              std::clamp(proposals[i], items[i].minSize, items[i].maxSize);
        }
      }

      break;
    }
  }

  // 5. Retornar somente as dimensões finais
  std::vector<float> finalSizes;
  finalSizes.reserve(items.size());

  for (const auto &item : items) {
    finalSizes.push_back(item.finalSize);
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