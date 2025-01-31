#ifndef SQL_FOOD_H
#define SQL_FOOD_H

void openDatabase(sqlite3**);
void storeFoodItem(sqlite3*, struct FoodItem);

#endif
