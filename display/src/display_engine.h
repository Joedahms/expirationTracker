#ifndef DISPLAY_ENGINE_H
#define DISPLAY_ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>

#include "../../logger.h"

#include "states/cancel_scan_confirmation.h"
#include "states/item_list.h"
#include "states/scanning.h"
#include "states/zero_weight.h"

#include "display_global.h"
#include "display_handler.h"
#include "engine_state.h"

class DisplayEngine {
public:
  DisplayEngine(const char* windowTitle,
                int windowXPosition,
                int windowYPosition,
                int screenWidth,
                int screenHeight,
                bool fullscreen,
                const zmqpp::context& context);

  SDL_Window* setupWindow(const char* windowTitle,
                          int windowXPosition,
                          int windowYPosition,
                          int screenWidth,
                          int screenHeight,
                          bool fullscreen);
  void initializeEngine(SDL_Window* window);

  void start();

  void checkState();
  void handleEvents();
  void checkKeystates();
  void update();

  void renderState();
  void clean();

private:
  Logger logger;
  DisplayHandler displayHandler;
  struct DisplayGlobal displayGlobal;
  EngineState engineState = EngineState::ITEM_LIST;
  // State* engineState      = nullptr;
  bool displayIsRunning = false;

  // States
  std::unique_ptr<Scanning> scanning;
  std::unique_ptr<ItemList> itemList;
  std::unique_ptr<ZeroWeight> zeroWeight;
  std::unique_ptr<CancelScanConfirmation> cancelScanConfirmation;

  void checkScanning();
  void checkItemList();
  void checkZeroWeight();
  void checkCancelScanConfirmation();

  void startToHardware();
  void zeroWeightChoiceToHardware(const std::string& zeroWeightChoice);
  void scanCancelledToVision();
};

#endif
