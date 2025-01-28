#include <glog/logging.h>
#include <iostream>
#include <unistd.h>

#include "food_item.h"

void sendFoodItem(struct FoodItem foodItem, int pipeToWrite) {
  // Send size of struct in bytes
  uint32_t foodItemSize = sizeof(foodItem);
  write(pipeToWrite, &foodItemSize, sizeof(foodItemSize));

  // Send struct piece by piece
  write(pipeToWrite, &foodItem.photoPath, sizeof(foodItem.photoPath));
  write(pipeToWrite, &foodItem.name, sizeof(foodItem.name));
  write(pipeToWrite, &foodItem.scanDate, sizeof(foodItem.scanDate));
  write(pipeToWrite, &foodItem.expirationDate, sizeof(foodItem.expirationDate));
  write(pipeToWrite, &foodItem.weight, sizeof(foodItem.weight));
  write(pipeToWrite, &foodItem.catagory, sizeof(foodItem.catagory));
  write(pipeToWrite, &foodItem.quantity, sizeof(foodItem.quantity));
}

struct FoodItem receiveFoodItem(int pipeToRead) {
  uint32_t imageSize;
  if (read(pipeToRead, &imageSize, sizeof(imageSize)) <= 0) {
    LOG(FATAL) << "Failed to read size header";
  }

  std::cout << imageSize << std::endl;

  /*
  while (imageSize > 0) {
    ssize_t bytesRead = read(pipeToRead, )
  }
  */
}
