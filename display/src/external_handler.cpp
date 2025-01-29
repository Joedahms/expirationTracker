#include <iostream>

#include "../../food_item.h"
#include "../../pipes.h"
#include "external_handler.h"

void externalHandler(struct DisplayPipes pipes) {
  struct FoodItem foodItem;
  // while (1) {
  receiveFoodItem(foodItem, pipes.fromVision[WRITE]);
  //}
}
