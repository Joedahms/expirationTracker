#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "../display_global.h"
#include "../elements/panel.h"
#include "../elements/scroll_box.h"
#include "../elements/text.h"
#include "item_list.h"

/**
 * @param displayGlobal Global display variables.
 */
ItemList::ItemList(struct DisplayGlobal displayGlobal) {
  this->displayGlobal = displayGlobal;

  previousUpdate = std::chrono::steady_clock::now();

  std::unique_ptr<ScrollBox> scrollBox = std::make_unique<ScrollBox>(this->displayGlobal);
  SDL_Rect scrollBoxRect               = {0, 0, 400, 100};
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->displayGlobal.window, &windowWidth, &windowHeight);
  scrollBoxRect.h = windowHeight - 1;
  scrollBox->setRectangle(scrollBoxRect);
  scrollBox->setPanelHeight(30);
  scrollBox->addBorder(1);
  this->scrollBoxes.push_back(std::move(scrollBox));
  this->scrollBoxes[0]->update();
}

/**
 * Handle events in the SDL event queue. Check if user wants to quit and if scrolling
 *
 * @param displayIsRunning If the display is still runnning. Eg the user has not quit the
 * display.
 * @return The state the display is in after handling all events in the SDL event queue
 */
int ItemList::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) { // While events in the queue
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;

    switch (event.type) {
    case SDL_QUIT: // Quit event
      *displayIsRunning = false;
      break;

      // Touch event here

    case SDL_MOUSEMOTION:
      this->mousePosition.x = event.motion.x;
      this->mousePosition.y = event.motion.y;
      break;

    case SDL_MOUSEBUTTONDOWN:
      checkButtonsClicked(mouseX, mouseY);
      this->scrollBoxes[0]->handleMouseButtonDown(this->mousePosition);
      break;

    case SDL_MOUSEWHEEL:
      if (event.wheel.y > 0) {
        this->scrollBoxes[0]->scrollUp(&this->mousePosition);
      }
      else if (event.wheel.y < 0) {
        this->scrollBoxes[0]->scrollDown(&this->mousePosition);
      }
      break;

    default:
      break;
    }
  }

  return ITEM_LIST;
}

/**
 * Perform the appropriate action depending on which keyboard key has been pressed.
 *
 * @param None
 * @return The state the display is in after checking if any keys have been pressed
 */
int ItemList::checkKeystates() {
  const Uint8* keystates = SDL_GetKeyboardState(NULL);

  if (keystates[SDL_SCANCODE_ESCAPE]) {
    return PAUSE_MENU;
  }

  return ITEM_LIST;
}

/**
 * Perform operations that need to be done periodically within the state. Only update
 * scrollbox.
 *
 * @param None
 * @return None
 */
void ItemList::update() { this->scrollBoxes[0]->update(); }

void ItemList::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  renderElements();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
