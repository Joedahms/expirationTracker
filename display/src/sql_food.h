#ifndef SQL_FOOD_H
#define SQL_FOOD_H

#include <sqlite3.h>
#include <vector>

void openDatabase(sqlite3**);
void storeFoodItem(sqlite3*, struct FoodItem);

void setFoodItem(struct FoodItem& foodItem,
                 const std::string& columnValue,
                 const std::string& columnName);
std::vector<FoodItem> readAllFoodItems();
FoodItem readFoodItemById(const int& id);
void updateFoodItemQuantity(const int& id, const int& newQuantity);

int readFoodItemByIdCallback(void* passedFoodItem,
                             int numColumns,
                             char** columns,
                             char** columnNames);
int readAllFoodItemsCallback(void* foodItemVector,
                             int numColumns,
                             char** columns,
                             char** columnNames);

#endif
