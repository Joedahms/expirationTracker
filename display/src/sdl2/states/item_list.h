#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <chrono>
#include <memory>
#include <sqlite3.h>
#include <vector>

#include "../../../../food_item.h"
#include "../display_global.h"
#include "../scroll_box.h"
#include "../text.h"

class ItemList {
public:
  ItemList(struct DisplayGlobal dg);
  ~ItemList();
  int handleEvents(bool*); // Handle SDL events while in the item list state
  int checkKeystates();
  void update();
  void render();

private:
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Text> placeholderText;
  sqlite3* database = nullptr;
  std::chrono::steady_clock::time_point previousUpdate;
  std::chrono::steady_clock::time_point currentUpdate;
  std::vector<FoodItem> allFoodItems;
  std::unique_ptr<ScrollBox> scrollBox;
};

#endif
