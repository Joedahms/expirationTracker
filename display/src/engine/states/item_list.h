#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <chrono>
#include <memory>
#include <sqlite3.h>
#include <vector>

#include "../../../../food_item.h"
#include "../display_global.h"
#include "../elements/scroll_box.h"
#include "../elements/text.h"
#include "state.h"

class ItemList : public State {
public:
  ItemList(struct DisplayGlobal displayGlobal);

  EngineState checkKeystates();
  void update() override;
  void render() const override;

private:
  std::chrono::steady_clock::time_point previousUpdate;
  std::chrono::steady_clock::time_point currentUpdate;
  SDL_Point mousePosition;
};

#endif
