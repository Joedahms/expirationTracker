#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>

#include "../display_global.h"
#include "../elements/button.h"
#include "../elements/composite_element.h"
#include "../elements/text.h"
#include "../engine_state.h"
#include "state.h"

/**
 * Main menu state. This is the state the display enters upon launching.
 */
class MainMenu : public State {
public:
  MainMenu(struct DisplayGlobal);
  void update() override;
  void render() const override;
};

#endif
