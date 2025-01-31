#ifndef EXTERNAL_HANDLER_H
#define EXTERNAL_HANDLER_H

#include <sqlite3.h>

#include "../../food_item.h"
#include "../../pipes.h"

void externalHandler(struct DisplayPipes);
void openDatabase(sqlite3**);
void storeFoodItem(sqlite3*, struct FoodItem);

#endif
