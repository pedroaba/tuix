#include "tuix/components/container.hpp"
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

Size TContainer::Measure() const {
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
  } else if (props_.direction == tuix::TLayoutDirection::Vertical) {
    // Adicionar gaps somente entre os filhos
    containerHeight += 2 * props_.padding;

    // Adicionar padding nas duas extremidades
    float gapMultiplier = std::max(0, totalOfChildren - 1);
    containerWidth += (2 * props_.padding) + (gapMultiplier * props_.gap);
  }

  return tuix::Size{.width = containerWidth, .height = containerHeight};
}

int TContainer::GetTotalOfChildren() const { return children_.size(); }

void TContainer::Arrange(Rect bounds) {
  bounds_ = bounds;

  // 1. Calcular o X inicial considerando padding
  float currentX = bounds_.x + props_.padding;

  // 2. Calcular o Y inicial considerando padding
  float currentY = bounds_.y + props_.padding;

  // 3. Percorrer children_
  const int totalOfChildren = GetTotalOfChildren();
  for (int i = 0; i < totalOfChildren; i++) {
    // 4. Consultar Measure() de cada filho
    Size childSize = children_.at(i)->Measure();

    // 5. Chamar Arrange() passando o Rect calculado
    Rect childBounds = Rect({
        .x = currentX,
        .y = currentY,
        .width = childSize.width,
        .height = childSize.height,
    });

    children_.at(i)->Arrange(childBounds);

    // 6. Avançar Y usando altura + gap
    switch (props_.direction) {
    case tuix::TLayoutDirection::Horizontal:
      currentX += props_.gap + childSize.width;
      break;
    case tuix::TLayoutDirection::Vertical:
      currentY += props_.gap + childSize.height;
      break;
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