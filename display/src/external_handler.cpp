#include <sqlite3.h>

#include "../../food_item.h"
#include "../../pipes.h"
#include "external_handler.h"
#include "sql_food.h"

/**
 * Handle communication between the display and other external processes. Communication to
 * the SDL process is not handled in this function.
 *
 * @param pipes Display pipes used to communicate with external processes
 * @return None
 */
void externalHandler(struct DisplayPipes pipes) {
  struct FoodItem foodItem;
  bool received = false;
  struct timeval timeout;
  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;
  while (received == false) {
    received = receiveFoodItem(foodItem, pipes.fromVision[READ], timeout);
  }

  sqlite3* database = nullptr;
  openDatabase(&database);

  storeFoodItem(database, foodItem);

  sqlite3_close(database);
}
