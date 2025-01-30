#include <iostream>

#include "../../food_item.h"
#include "../../pipes.h"
#include "external_handler.h"

void externalHandler(struct DisplayPipes pipes) {
  struct FoodItem foodItem;
  bool received = false;
  struct timeval timeout;
  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;
  while (received == false) {
    received = receiveFoodItem(foodItem, pipes.fromVision[READ], timeout);
  }
}
