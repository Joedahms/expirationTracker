#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "../display_global.h"
#include "../panel.h"
#include "../scroll_box.h"
#include "../text.h"
#include "item_list.h"

/**
 * @param dg Global display variables.
 */
ItemList::ItemList(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  previousUpdate = std::chrono::steady_clock::now();

  openDatabase(&this->database);

  std::unique_ptr<ScrollBox> scrollBox = std::make_unique<ScrollBox>(this->displayGlobal);
  SDL_Rect scrollBoxRect               = {0, 0, 300, 100};
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->displayGlobal.window, &windowWidth, &windowHeight);
  scrollBoxRect.h = windowHeight - 1;
  scrollBox->setRectangle(scrollBoxRect);
  scrollBox->setPanelHeight(30);
  scrollBox->addBorder(1);
  this->scrollBoxes.push_back(std::move(scrollBox));
}

ItemList::~ItemList() { sqlite3_close(database); }

int ItemList::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) { // While events in the queue
    switch (event.type) {
    case SDL_QUIT: // Quit event
      *displayIsRunning = false;
      break;
      // Touch event here
    case SDL_MOUSEWHEEL:
      if (event.wheel.y > 0) {
        this->scrollBoxes[0]->scrollUp();
      }
      else if (event.wheel.y < 0) {
        this->scrollBoxes[0]->scrollDown();
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

void ItemList::update() {
  this->currentUpdate = std::chrono::steady_clock::now();

  std::chrono::seconds updateDifference;
  updateDifference = std::chrono::duration_cast<std::chrono::seconds>(
      this->currentUpdate - this->previousUpdate);

  if (updateDifference.count() > 5) { // 5 or more seconds since last update
    char* errorMessage    = nullptr;
    const char* selectAll = "SELECT * FROM foodItems;";
    this->allFoodItems.clear();

    // All food items
    int sqlReturn = sqlite3_exec(this->database, selectAll, readFoodItemCallback,
                                 &allFoodItems, &errorMessage);

    this->scrollBoxes[0]->updatePanelContents(this->allFoodItems);

    if (sqlReturn != SQLITE_OK) {
      LOG(FATAL) << "SQL Exec Error: " << errorMessage;
    }
    this->previousUpdate = this->currentUpdate;
  }
}

void ItemList::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  renderElements();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
