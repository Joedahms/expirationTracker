#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "item_list.h"

/**
 * @param displayGlobal Global display variables.
 */
ItemList::ItemList(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  // Placeholder text
  const char* fontPath               = "../display/fonts/16020_FUTURAM.ttf";
  const char* placeholderTextContent = "Placeholder for item list";
  SDL_Color placeholderTextColor     = {0, 255, 0, 255}; // Green
  SDL_Rect placeholderTextRectangle  = {
      100,
      100,
      0,
      0,
  }; // x y w h
  this->placeholderText =
      std::make_unique<Text>(this->displayGlobal, fontPath, placeholderTextContent, 24,
                             placeholderTextColor, placeholderTextRectangle);
  this->placeholderText->centerHorizontal(windowSurface);

  previousUpdate = std::chrono::steady_clock::now();

  openDatabase(&this->database);
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
    struct FoodItem foodItem;

    int sqlReturn = sqlite3_exec(this->database, selectAll, readFoodItemCallback,
                                 &foodItem, &errorMessage);
    if (sqlReturn != SQLITE_OK) {
      LOG(FATAL) << "SQL Exec Error: " << errorMessage;
    }
    std::cout << foodItem.name << std::endl;

    this->previousUpdate = this->currentUpdate;
  }
}

void ItemList::render() {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->placeholderText->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
