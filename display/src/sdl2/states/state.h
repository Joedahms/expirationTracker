#ifndef STATE_H
#define STATE_H

#define MAIN_MENU  0
#define SCANNING   1
#define PAUSE_MENU 2
#define ITEM_LIST  3

#include <memory>
#include <vector>

#include "../button.h"
#include "../display_global.h"
#include "../text.h"

class State {
public:
  virtual void render() const     = 0;
  virtual int handleEvents(bool*) = 0;
  int checkButtonsClicked(const int& mouseX, const int& mouseY) const;
  void renderElements() const;

protected:
  struct DisplayGlobal displayGlobal;
  std::vector<std::unique_ptr<Text>> texts;
  std::vector<std::unique_ptr<Button>> buttons;
};

#endif
