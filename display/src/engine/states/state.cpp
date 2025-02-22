#include <iostream>

#include "../display_global.h"
#include "state.h"

void State::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    if (event.type == SDL_QUIT) {
      *displayIsRunning = false;
      break;
    }
    else {
      this->rootElement->handleEvent(event);
    }
  }
}

EngineState State::getCurrentState() { return this->currentState; }

void State::setCurrentState(EngineState currentState) {
  this->currentState = currentState;
}

/**
 * Check if any of the buttons in a state have been moused over and clicked by the user.
 *
 * @param mouseX The x coordinate of the mouse when the mouse button was clicked
 * @param mouseY The y coordinate of the mouse when the mouse button was clicked
 * @return The return value of the button after it was clicked
 */
EngineState State::checkButtonsClicked(const int& mouseX, const int& mouseY) const {
  /*
  int returnValue = -1;
  for (auto& currButton : this->buttons) {
    if (currButton->checkHovered(mouseX, mouseY)) {
      std::cout << "button clicks" << std::endl;
      returnValue = currButton->getClickReturn();
      break;
    }
  }
  return returnValue;
  */
}
