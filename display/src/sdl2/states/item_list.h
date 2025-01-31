#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <memory>

#include "../display_global.h"
#include "../text.h"

class ItemList {
public:
  ItemList(struct DisplayGlobal);
  int handleEvents(bool*); // Handle SDL events while in the item list state
  int checkKeystates();
  void render();

private:
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Text> placeholderText;
};

#endif
