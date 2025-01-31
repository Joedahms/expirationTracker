#ifndef SQL_FOOD_H
#define SQL_FOOD_H

#include <sqlite3.h>

void openDatabase(sqlite3**);
void storeFoodItem(sqlite3*, struct FoodItem);
int readFoodItemCallback(void*, int, char**, char**);

#endif
