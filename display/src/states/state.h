#ifndef STATE_H
#define STATE_H

#include <memory>
#include <vector>

#include "../display_global.h"
#include "../display_messenger.h"
#include "../elements/container.h"
#include "../engine_state.h"
#include "../log_files.h"

class State {
public:
  State(const struct DisplayGlobal& displayGlobal,
        const std::string& logFile,
        const EngineState& state);
  virtual void handleEvents(bool* displayIsRunning);
  virtual void update();
  virtual void render() const = 0;
  virtual void enter();
  virtual void exit() = 0;

  EngineState getCurrentState();
  void setCurrentState(EngineState currentState);

  bool checkStateChange();

protected:
  struct DisplayGlobal displayGlobal;
  const std::string logFile;
  std::unique_ptr<Logger> logger;

  const EngineState defaultState;
  EngineState currentState;

  std::shared_ptr<Container> rootElement;
  SDL_Surface* windowSurface = nullptr;

  DisplayMessenger& displayMessenger;
};

#endif
