#ifndef PANEL_H
#define PANEL_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "../../../../food_item.h"
#include "../display_global.h"
#include "button.h"
#include "element.h"
#include "number_setting.h"
#include "text.h"

class Panel : public Element {
public:
  Panel(struct DisplayGlobal displayGlobal, int id);
  Panel(struct DisplayGlobal displayGlobal, int id, SDL_Rect rect);
  Panel(struct DisplayGlobal displayGlobal,
        int id,
        SDL_Rect rect,
        std::vector<std::unique_ptr<Text>> t);
  Panel(struct DisplayGlobal displayGlobal,
        int id,
        SDL_Rect rect,
        std::vector<std::unique_ptr<Text>> t,
        std::vector<std::unique_ptr<Button>> b);

  void addFoodItemName(const FoodItem& foodItem);
  void addFoodItemExpirationDate(const FoodItem& foodItem);

  void update() override;
  void render() const override;

private:
  std::vector<std::unique_ptr<Text>> texts;
  std::vector<std::unique_ptr<Button>> buttons;
  NumberSetting itemQuantity;
};

#endif
