#ifndef STATE_H
#define STATE_H

#include <memory>
#include <vector>

#include "../display_global.h"
#include "../elements/button.h"
#include "../elements/composite_element.h"
#include "../elements/container.h"
#include "../elements/scroll_box.h"
#include "../elements/text.h"
#include "../engine_state.h"

class State {
public:
  virtual EngineState handleEvents(bool*) = 0;
  virtual void update()                   = 0;
  virtual void render() const             = 0;
  EngineState getCurrentState();
  void setCurrentState(EngineState currentState);
  EngineState checkButtonsClicked(const int& mouseX, const int& mouseY) const;

protected:
  EngineState currentState;
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Container> rootElement;
};

#endif
