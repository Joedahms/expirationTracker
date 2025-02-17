#include <iostream>

#include "../display_global.h"
#include "state.h"

/**
 * Check if any of the buttons in a state have been moused over and clicked by the user.
 *
 * @param mouseX The x coordinate of the mouse when the mouse button was clicked
 * @param mouseY The y coordinate of the mouse when the mouse button was clicked
 * @return The return value of the button after it was clicked
 */
int State::checkButtonsClicked(const int& mouseX, const int& mouseY) const {
  int returnValue = -1;
  for (auto& currButton : this->buttons) {
    if (currButton->checkHovered(mouseX, mouseY)) {
      std::cout << "button clicks" << std::endl;
      returnValue = currButton->getClickReturn();
      break;
    }
  }
  return returnValue;
}

/**
 * Function to render all elements that exist. If any of the element vectors is empty, no
 * elements of that type will be rendered.
 *
 * @param None
 * @return None
 */
void State::renderElements() const {
  for (auto& currButton : this->buttons) {
    currButton->render();
  }
  for (auto& currText : this->texts) {
    currText->render();
  }
  for (auto& currScrollBox : this->scrollBoxes) {
    currScrollBox->render();
  }
}
