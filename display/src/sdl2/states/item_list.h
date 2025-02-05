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
#include "state.h"

class ItemList : public State {
public:
  ItemList(struct DisplayGlobal dg);
  ~ItemList();
  int handleEvents(bool*) override;
  int checkKeystates();
  void update();
  void render() const override;

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
