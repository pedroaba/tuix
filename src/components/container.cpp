#include "tuix/components/container.hpp"
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
  // W = max(children_widths) + 2(padding)
  // H = sum(children_heights) + 2(padding) + 2(gap)

  // Descobrir a maior largura entre os filhos
  const int totalOfChildren = GetTotalOfChildren();
  float biggerWidth = 0.0f;
  for (int i = 0; i < totalOfChildren; i++) {
    float currentChildWidth = children_.at(i)->Measure().width;
    if (biggerWidth < currentChildWidth) {
      biggerWidth = currentChildWidth;
    }
  }

  // Somar todas as alturas
  float heightOfAllChild = 0.0f;
  for (int i = 0; i < totalOfChildren; i++) {
    heightOfAllChild += children_.at(i)->Measure().height;
  }

  // Adicionar gaps somente entre os filhos
  biggerWidth += 2 * props_.padding;

  // Adicionar padding nas duas extremidades
  float gapMultiplier = std::max(0, totalOfChildren - 1);
  heightOfAllChild += (2 * props_.padding) + (gapMultiplier * props_.gap);

  return tuix::Size{.width = biggerWidth, .height = heightOfAllChild};
}

int TContainer::GetTotalOfChildren() const { return children_.size(); }

void TContainer::Arrange(Rect bounds) {
  bounds_ = bounds;

  // 1. Calcular o X inicial considerando padding
  float initialX = bounds_.x + props_.padding;

  // 2. Calcular o Y inicial considerando padding
  float initialY = bounds_.y + props_.padding;

  // 3. Percorrer children_
  float currentY = initialY;
  const int totalOfChildren = GetTotalOfChildren();
  for (int i = 0; i < totalOfChildren; i++) {
    // 4. Consultar Measure() de cada filho
    Size childSize = children_.at(i)->Measure();

    // 5. Chamar Arrange() passando o Rect calculado
    Rect childBounds = Rect({
        .x = initialX,
        .y = currentY,
        .width = childSize.width,
        .height = childSize.height,
    });

    children_.at(i)->Arrange(childBounds);

    // 6. Avançar Y usando altura + gap
    currentY += props_.gap + childSize.height;
  }
}

void TContainer::Paint(TRenderer &renderer) const {
  const int quantityOfChildren = children_.size();
  for (int i = 0; i < quantityOfChildren; i++) {
    children_.at(i)->Paint(renderer);
  }
}

} // namespace tuix