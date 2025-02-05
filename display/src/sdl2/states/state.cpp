#include "../display_global.h"
#include "state.h"

int State::checkButtonsClicked(const int& mouseX, const int& mouseY) const {
  int returnValue = -1;
  for (auto& currButton : this->buttons) {
    if (currButton->checkHovered(mouseX, mouseY)) {
      returnValue = currButton->getClickReturn();
      break;
    }
  }
  return returnValue;
}

void State::renderElements() const {
  for (auto& currButton : this->buttons) {
    currButton->render();
  }
  for (auto& currText : this->texts) {
    currText->render();
  }
}
