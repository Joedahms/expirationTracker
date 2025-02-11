#ifndef SQL_FOOD_H
#define SQL_FOOD_H

#include <sqlite3.h>
#include <vector>

void openDatabase(sqlite3**);
void storeFoodItem(sqlite3*, struct FoodItem);
std::vector<FoodItem> readAllFoodItems();
FoodItem readFoodItemById(const int& id);
int readFoodItemCallback(void*, int, char**, char**);

#endif
