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

class Panel : public CompositeElement {
public:
  Panel(struct DisplayGlobal displayGlobal,
        int id,
        SDL_Rect boundaryRectangle,
        SDL_Point positionRelativeToParent,
        std::vector<std::unique_ptr<Text>> t);
  Panel(struct DisplayGlobal displayGlobal, int id, SDL_Point positionRelativeToParent);

  void handleMouseButtonDown(const SDL_Point& mousePosition);
  void addText(const std::string& fontPath,
               const std::string& content,
               const int& fontSize,
               const SDL_Color& color);
  void addFoodItem(const FoodItem& foodItem);

  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event) override;

private:
  int id = 0;

  void addFoodItemName(const FoodItem& foodItem);
  void addFoodItemExpirationDate(const FoodItem& foodItem);

  //  std::vector<std::unique_ptr<Text>> texts;
  // NumberSetting itemQuantity;
};

#endif
