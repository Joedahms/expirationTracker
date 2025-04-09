#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>

#include "../../endpoints.h"

#include "display_engine.h"
#include "display_global.h"
#include "states/state.h"

/**
 * @param windowTitle Title of the display window
 * @param windowXPosition X position of the display window
 * @param windowYPostition Y position of the display window
 * @param screenWidth Width of the display window in pixels
 * @param screenHeight Height of the display window in pixels
 * @param fullscreen Whether or not the display window should be fullscreen
 * @param displayGlobal Global display variables
 */
DisplayEngine::DisplayEngine(const char* windowTitle,
                             int windowXPosition,
                             int windowYPosition,
                             int screenWidth,
                             int screenHeight,
                             bool fullscreen,
                             const zmqpp::context& context)
    : logger("display_engine.txt"), displayHandler(context) {
  this->logger.log("Constructing display engine");

  this->displayGlobal.window = setupWindow(windowTitle, windowXPosition, windowYPosition,
                                           screenWidth, screenHeight, fullscreen);

  initializeEngine(this->displayGlobal.window);

  // States
  this->scanning   = std::make_unique<Scanning>(this->displayGlobal);
  this->itemList   = std::make_unique<ItemList>(this->displayGlobal);
  this->zeroWeight = std::make_unique<ZeroWeight>(this->displayGlobal);
  this->cancelScanConfirmation =
      std::make_unique<CancelScanConfirmation>(this->displayGlobal);

  displayIsRunning = true;
  this->logger.log("Engine is constructed and now running");
}

/**
 * Setup the SDL display window.
 *
 * @param windowTitle The name of the window. Is what is displayed on the top bezel of
 * the window when the display is running.
 * @param windowXPosition The X position of the window on the user's screen.
 * @param windowYPosition The Y position of the window on the user's screen.
 * @param screenWidth The width of the screen in pixels.
 * @param screenHeight The height of the screen in pixels.
 * @param fullscreen Whether or not the display window should be fullscreen.
 * @return Pointer to the SDL display window.
 */
SDL_Window* DisplayEngine::setupWindow(const char* windowTitle,
                                       int windowXPosition,
                                       int windowYPosition,
                                       int screenWidth,
                                       int screenHeight,
                                       bool fullscreen) {
  this->logger.log("Creating SDL display window");

  int flags = 0;
  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  try {
    return SDL_CreateWindow(windowTitle, windowXPosition, windowYPosition, screenWidth,
                            screenHeight, flags);
  } catch (...) {
    LOG(FATAL) << "Error setting up SDL display window";
  }

  this->logger.log("SDL display window created");
}

/**
 * Setup SDL, the renderer, and TTF. Renderer is part of the global display objects.
 *
 * @param window The SDL display window
 * @return None
 */
void DisplayEngine::initializeEngine(SDL_Window* window) {
  this->logger.log("Initializing engine");
  try {
    int sdlInitReturn = SDL_Init(SDL_INIT_EVERYTHING);
    if (sdlInitReturn != 0) {
      throw;
    }
  } catch (...) {
    LOG(FATAL) << "Failed to initialize engine";
    exit(1);
  }

  // Create renderer
  try {
    this->displayGlobal.renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!this->displayGlobal.renderer) {
      throw;
    }
    SDL_SetRenderDrawColor(this->displayGlobal.renderer, 255, 255, 255, 255);
  } catch (...) {
    LOG(FATAL) << "Error creating renderer";
    exit(1);
  }

  // Initialize TTF
  try {
    int ttfInitReturn = TTF_Init();
    if (ttfInitReturn == -1) {
      throw;
    }
  } catch (...) {
    LOG(FATAL) << "Failed to initialize TTF";
    exit(1);
  }
  this->logger.log("Engine initialized");
}

void DisplayEngine::start() {
  LOG(INFO) << "SDL display process started successfully";

  std::chrono::milliseconds msPerFrame = std::chrono::milliseconds(16);

  while (this->displayIsRunning) {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    handleEvents();
    checkState();
    checkKeystates();
    checkState();
    update();
    renderState();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::milliseconds sleepDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(start + msPerFrame - now);
    std::this_thread::sleep_for(sleepDuration);
  }

  clean();
}

/**
 * Check if the current state has requested a state switch.
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkState() {
  switch (this->engineState) {
  case EngineState::SCANNING:
    checkScanning();
    break;

  case EngineState::ITEM_LIST:
    checkItemList();
    break;

  case EngineState::ZERO_WEIGHT:
    checkZeroWeight();
    break;

  case EngineState::CANCEL_SCAN_CONFIRMATION:
    checkCancelScanConfirmation();
    break;

  default:
    std::cout << engineStateToString(this->engineState) << std::endl;
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Check if the scanning state has requested a state switch
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkScanning() {
  this->engineState = this->scanning->getCurrentState();

  this->scanning->setCurrentState(EngineState::SCANNING);
}

/**
 * Check if the scanning state has requested a state switch
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkItemList() {
  this->engineState = this->itemList->getCurrentState();

  if (this->engineState == EngineState::SCANNING) {
    startToHardware();
  }

  this->itemList->setCurrentState(EngineState::ITEM_LIST);
}

/**
 * Check if the scanning state has requested a state switch
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkZeroWeight() {
  this->engineState = this->zeroWeight->getCurrentState();

  // Override
  if (this->engineState == EngineState::SCANNING) {
    zeroWeightChoiceToHardware(Messages::OVERRIDE);
  }
  // Cancel
  else if (this->engineState == EngineState::ITEM_LIST) {
    zeroWeightChoiceToHardware(Messages::CANCEL);
  }
  // Retry
  else if (this->zeroWeight->getRetryScan()) {
    zeroWeightChoiceToHardware(Messages::RETRY);
    this->zeroWeight->setRetryScan(false);
    startToHardware();
  }

  this->zeroWeight->setCurrentState(EngineState::ZERO_WEIGHT);
}

/**
 * Check if the scanning state has requested a state switch
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkCancelScanConfirmation() {
  this->engineState = this->cancelScanConfirmation->getCurrentState();

  if (this->engineState == EngineState::ITEM_LIST) { // Scan was cancelled
    scanCancelledToVision();
  }
  else if (this->engineState == EngineState::SCANNING) { // Scan was not cancelled
  }

  this->cancelScanConfirmation->setCurrentState(EngineState::CANCEL_SCAN_CONFIRMATION);
}

/**
 * Check the current state, and call that state's handle events method.
 *
 * @param engineToDisplay Pipe from the display engine to the display
 * @param displayToEngine Pipe from the display to the displayEngine
 * @return None
 */
void DisplayEngine::handleEvents() {
  switch (this->engineState) {
  case EngineState::SCANNING:
    {
      this->scanning->handleEvents(&this->displayIsRunning);

      std::string response      = Messages::AFFIRMATIVE;
      std::string visionRequest = this->displayHandler.receiveMessage(response, 0);
      if (visionRequest == "null") {
        break;
      }

      if (visionRequest == Messages::ITEM_DETECTION_SUCCEEDED) {
        this->logger.log("New food item received, switching to item list state");
        this->engineState = EngineState::ITEM_LIST;
      }
      else if (visionRequest == Messages::ITEM_DETECTION_FAILED) {
        this->logger.log("Item detection failed, switching to item list state");
        this->engineState = EngineState::ITEM_LIST;
      }
      else {
        this->logger.log("Invalid request received from vision: " + visionRequest);
        LOG(FATAL) << "Invalid request received from vision: " << visionRequest;
      }
      break;
    }

  case EngineState::ITEM_LIST:
    this->itemList->handleEvents(&this->displayIsRunning);
    break;

  case EngineState::ZERO_WEIGHT:
    this->zeroWeight->handleEvents(&this->displayIsRunning);
    break;

  case EngineState::CANCEL_SCAN_CONFIRMATION:
    this->cancelScanConfirmation->handleEvents(&this->displayIsRunning);
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Check the current state of the display and call that state's method to check the
 * key states
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkKeystates() {
  switch (this->engineState) {
  case EngineState::SCANNING:
    this->engineState = this->scanning->checkKeystates();
    break;

  case EngineState::ITEM_LIST:
    this->engineState = this->itemList->checkKeystates();
    break;

  case EngineState::ZERO_WEIGHT:
    break;

  case EngineState::CANCEL_SCAN_CONFIRMATION:
    break;

  default:
    LOG(FATAL) << "Invalid state " << engineStateToString(this->engineState);
    break;
  }
}

/**
 * Not implemented but may want to add:
 * First check if it's time to update. If it is, reset the time since last
 * update.
 *
 * @param None
 * @return None
 */
void DisplayEngine::update() {
  switch (this->engineState) {
  case EngineState::SCANNING:
    this->scanning->update();
    break;

  case EngineState::ITEM_LIST:
    this->itemList->update();
    break;

  case EngineState::ZERO_WEIGHT:
    this->zeroWeight->update();
    break;

  case EngineState::CANCEL_SCAN_CONFIRMATION:
    this->cancelScanConfirmation->update();
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Check current state and call that state's function to render.
 *
 * @param None
 * @return None
 */
void DisplayEngine::renderState() {
  switch (this->engineState) {
  case EngineState::SCANNING:
    this->scanning->render();
    break;

  case EngineState::ITEM_LIST:
    this->itemList->render();
    break;

  case EngineState::ZERO_WEIGHT:
    this->zeroWeight->render();
    break;

  case EngineState::CANCEL_SCAN_CONFIRMATION:
    this->cancelScanConfirmation->render();
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Free SDL resources and quit.
 *
 * @param None
 * @return None
 */
void DisplayEngine::clean() {
  SDL_DestroyWindow(this->displayGlobal.window);
  SDL_DestroyRenderer(this->displayGlobal.renderer);
  SDL_Quit();
  LOG(INFO) << "DisplayEngine cleaned";
}

void DisplayEngine::scanCancelledToVision() {
  this->logger.log("Scan cancelled, sending scan cancelled message to vision");

  std::string visionResponse = this->displayHandler.sendMessage(
      Messages::SCAN_CANCELLED, ExternalEndpoints::visionEndpoint);

  this->logger.log("Received response from vision");
  if (visionResponse == Messages::AFFIRMATIVE) {
    this->logger.log("Vision received scan cancelled message");
  }
  else {
    this->logger.log("Invalid response received from vision: " + visionResponse);
    LOG(FATAL) << "Invalid response received from vision: " << visionResponse;
  }
}

void DisplayEngine::startToHardware() {
  this->logger.log("Scan initialized, sending start signal to hardware");

  std::string hardwareResponse = this->displayHandler.sendMessage(
      Messages::START_SCAN, ExternalEndpoints::hardwareEndpoint);

  if (hardwareResponse == Messages::AFFIRMATIVE) {
    this->logger.log("Hardware starting scan");
  }
  else if (hardwareResponse == Messages::ZERO_WEIGHT) {
    this->logger.log(
        "Hardware indicated zero weight on platform, switching to zero weight state");
    this->engineState = EngineState::ZERO_WEIGHT;
  }
  else {
    this->logger.log("Invalid response received from hardware: " + hardwareResponse);
    LOG(FATAL) << "Invalid response received from hardware: " << hardwareResponse;
  }
}

void DisplayEngine::zeroWeightChoiceToHardware(const std::string& zeroWeightChoice) {
  this->logger.log("Sending zero weight choice: " + zeroWeightChoice + " to hardware");

  std::string hardwareResponse = this->displayHandler.sendMessage(
      zeroWeightChoice, ExternalEndpoints::hardwareEndpoint);

  if (hardwareResponse == Messages::AFFIRMATIVE) {
    this->logger.log(
        "Zero weight choice successfully sent to hardware, switching to scanning state");
  }
  else {
    this->logger.log("Invalid response received from hardware: " + hardwareResponse);
    LOG(FATAL) << "Invalid response received from hardware: " << hardwareResponse;
  }
}
