#include <iostream>

#include "../log_files.h"
#include "../sql_food.h"
#include "scan_success.h"

ScanSuccess::ScanSuccess(struct DisplayGlobal& displayGlobal, const EngineState& state)
    : State(displayGlobal, state), logger(LogFiles::SCAN_SUCCESS) {
  this->logger.log("Constructing scan success state");
  this->currentState = EngineState::SCAN_SUCCESS;

  this->displayGlobal = displayGlobal;

  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  SDL_Rect rootRectangle     = {0, 0, 0, 0};
  rootRectangle.w            = windowSurface->w;
  rootRectangle.h            = windowSurface->h;
  this->rootElement          = std::make_shared<Container>(rootRectangle);

  const std::string successMessageContent = "Scan successful, is this correct?";
  const SDL_Color successMessageColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect successMessageRectangle  = {0, 100, 0, 0};
  std::unique_ptr<Text> successMessage    = std::make_unique<Text>(
      this->displayGlobal, successMessageRectangle, DisplayGlobal::futuramFontPath,
      successMessageContent, 24, successMessageColor);
  successMessage->setCenteredHorizontal();
  this->rootElement->addElement(std::move(successMessage));

  std::shared_ptr<Button> yesButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 200, 0, 0}, "Yes", SDL_Point{0, 0},
      [this]() {
        correctItem();
        this->currentState = EngineState::ITEM_LIST;
      },
      LogFiles::SCAN_SUCCESS);
  yesButton->setCenteredHorizontal();
  this->rootElement->addElement(yesButton);

  std::shared_ptr<Button> noButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 250, 0, 0}, "No", SDL_Point{0, 0},
      [this]() {
        incorrectItem();
        this->currentState = EngineState::ITEM_LIST;
      },
      LogFiles::SCAN_SUCCESS);
  noButton->setCenteredHorizontal();
  this->rootElement->addElement(noButton);
}

void ScanSuccess::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}

void ScanSuccess::enter() {
  this->currentState = this->defaultState;
  this->foodItem     = this->displayHandler.detectionSuccess();

  this->logger.log("Storing food item in database");
  sqlite3* database = nullptr;
  openDatabase(&database);
  int id = storeFoodItem(database, foodItem);
  sqlite3_close(database);
  this->logger.log("Food item stored in database");

  std::cout << id << std::endl;

  SDL_Rect boundaryRectangle      = {0, 150, 400, 30};
  std::shared_ptr<Panel> newPanel = std::make_shared<Panel>(
      this->displayGlobal, boundaryRectangle, LogFiles::SCAN_SUCCESS);
  newPanel->setCenteredHorizontal();
  newPanel->addFoodItem(foodItem, SDL_Point{0, 0});
  newPanel->addBorder(1);
  this->rootElement->addElement(newPanel);
}

void ScanSuccess::exit() {}

void ScanSuccess::correctItem() {
  /*
  this->logger.log("Storing food item in database");
  sqlite3* database = nullptr;
  openDatabase(&database);
  storeFoodItem(database, foodItem);
  sqlite3_close(database);
  this->logger.log("Food item stored in database");
  */
}

void ScanSuccess::incorrectItem() { deleteById(this->foodItem.getId()); }
