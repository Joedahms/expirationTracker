#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>
#include <sstream>

#include "../../food_item.h"
#include "sql_food.h"

/**
 * Open the database and make sure the table for the food items is present.
 *
 * @param database Pointer to database pointer so that it can be modified (opened)
 * @return None
 */
void openDatabase(sqlite3** database) {
  int sqlReturn = sqlite3_open("test.db", database);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "Error opening database: " << sqlite3_errmsg(*database);
  }

  const char* createSqlTable = "CREATE TABLE IF NOT EXISTS foodItems("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "name TEXT,"
                               "category TEXT,"
                               "scanDateYear INTEGER,"
                               "scanDateMonth INTEGER,"
                               "scanDateDay INTEGER,"
                               "expirationDateYear INTEGER,"
                               "expirationDateMonth INTEGER,"
                               "expirationDateDay INTEGER,"
                               "weight FLOAT,"
                               "quantity INTEGER);";

  char* errorMessage = nullptr;
  sqlReturn          = sqlite3_exec(*database, createSqlTable, NULL, NULL, &errorMessage);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "SQL Exec Error: " << errorMessage;
    sqlite3_free(errorMessage);
  }
}

/**
 * Store a food item into a database. Stores all food item fields besides the photo path.
 * Not storing photo path due to the photo only being used before the food item is stored
 * into the database.
 *
 * @param database The database to store the food item into
 * @param foodItem The food item to store
 * @return None
 */
void storeFoodItem(sqlite3* database, struct FoodItem foodItem) {
  const char* insertSql =
      "INSERT INTO foodItems (name, category, scanDateYear, "
      "scanDateMonth, scanDateDay, expirationDateYear, expirationDateMonth, "
      "expirationDateDay, weight, quantity) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

  sqlite3_stmt* statement = nullptr;
  int sqlReturn = sqlite3_prepare_v2(database, insertSql, -1, &statement, nullptr);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "Prepare error: " << sqlite3_errmsg(database);
  }

  // Pull date elements out of chrono
  int scanDateYear           = static_cast<int>(foodItem.scanDate.year());
  unsigned int scanDateMonth = static_cast<unsigned>(foodItem.scanDate.month());
  unsigned int scanDateDay   = static_cast<unsigned>(foodItem.scanDate.day());
  int expirationDateYear     = static_cast<int>(foodItem.expirationDate.year());
  unsigned int expirationDateMonth =
      static_cast<unsigned>(foodItem.expirationDate.month());
  unsigned int expirationDateDay = static_cast<unsigned>(foodItem.expirationDate.day());

  // Bind to query
  sqlite3_bind_text(statement, 1, foodItem.name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, foodItem.category.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(statement, 3, scanDateYear);
  sqlite3_bind_int(statement, 4, scanDateMonth);
  sqlite3_bind_int(statement, 5, scanDateDay);
  sqlite3_bind_int(statement, 6, expirationDateYear);
  sqlite3_bind_int(statement, 7, expirationDateMonth);
  sqlite3_bind_int(statement, 8, expirationDateDay);
  sqlite3_bind_double(statement, 9, foodItem.weight);
  sqlite3_bind_int(statement, 10, foodItem.quantity);

  sqlReturn = sqlite3_step(statement);
  if (sqlReturn != SQLITE_DONE) {
    LOG(FATAL) << "Execution Error: " << sqlite3_errmsg(database);
  }

  sqlite3_finalize(statement);
}

/**
 * When reading a food item from a database, this function provides an interface between
 * the columns in the database and the FoodItem type. It gives the ability to assin a food
 * item field from a column in the database.
 *
 * @param foodItem The food item to assign the field of
 * @param columnValue The value read out of the column
 * @param columnName The name of the column that columnValue was read from
 * @return None
 */
void setFoodItem(struct FoodItem& foodItem,
                 const std::string& columnValue,
                 const std::string& columnName) {
  if (columnName == "id") {
    foodItem.id = stoi(columnValue);
  }
  if (columnName == "name") {
    foodItem.name = columnValue;
  }
  else if (columnName == "category") {
    foodItem.category = columnValue;
  }
  else if (columnName == "scanDateYear") {
    foodItem.scanDate =
        std::chrono::year_month_day(std::chrono::year{stoi(columnValue)},
                                    foodItem.scanDate.month(), foodItem.scanDate.day());
  }
  else if (columnName == "scanDateMonth") {
    foodItem.scanDate = std::chrono::year_month_day(
        foodItem.scanDate.year(),
        std::chrono::month{static_cast<unsigned>(std::stoi(columnValue))},
        foodItem.scanDate.day());
  }
  else if (columnName == "scanDateDay") {
    foodItem.scanDate = std::chrono::year_month_day(
        foodItem.scanDate.year(), foodItem.scanDate.month(),
        std::chrono::day{static_cast<unsigned>(stoi(columnValue))});
  }
  else if (columnName == "expirationDateYear") {
    foodItem.expirationDate = std::chrono::year_month_day(
        std::chrono::year{stoi(columnValue)}, foodItem.expirationDate.month(),
        foodItem.expirationDate.day());
  }
  else if (columnName == "expirationDateMonth") {
    foodItem.expirationDate = std::chrono::year_month_day(
        foodItem.expirationDate.year(),
        std::chrono::month{static_cast<unsigned>(stoi(columnValue))},
        foodItem.expirationDate.day());
  }
  else if (columnName == "expirationDateDay") {
    foodItem.expirationDate = std::chrono::year_month_day(
        foodItem.expirationDate.year(), foodItem.expirationDate.month(),
        std::chrono::day{static_cast<unsigned>(stoi(columnValue))});
  }
  /*
  else if (std::string(columnNames[i]) == "weight") {
    foodItem.weight = columns[i];
  }
  */
  else if (columnName == "quantity") {
    foodItem.quantity = stoi(columnValue);
  }
}

/**
 * Read all food items currently stored in the database.
 *
 * @param None
 * @return Vector of food item objects, each corresponding to a food item read from the
 * database.
 */
std::vector<FoodItem> readAllFoodItems() {
  sqlite3* database = nullptr;
  openDatabase(&database);

  char* errorMessage    = nullptr;
  const char* selectAll = "SELECT * FROM foodItems;";

  std::vector<FoodItem> allFoodItems;
  int sqlReturn = sqlite3_exec(database, selectAll, readAllFoodItemsCallback,
                               &allFoodItems, &errorMessage);

  sqlite3_close(database);
  return allFoodItems;
}

/**
 * Read a food item from the database by its id. Since the ID is the primary key in the
 * database, only one food item will be returned.
 *
 * @param id The id of the food item to be read
 * database.
 * @return The food item with the matching id
 */
FoodItem readFoodItemById(const int& id) {
  sqlite3* database = nullptr;
  openDatabase(&database);

  char* errorMessage = nullptr;

  std::stringstream selectId;
  selectId << "SELECT * FROM foodItems WHERE id = " << id << ";";

  FoodItem foodItem;
  int sqlReturn = sqlite3_exec(database, selectId.str().c_str(), readFoodItemByIdCallback,
                               &foodItem, &errorMessage);

  sqlite3_close(database);
  return foodItem;
}

void updateFoodItemQuantity(const int& id, const int& newQuantity) {
  sqlite3* database = nullptr;
  openDatabase(&database);
  char* errorMessage = nullptr;

  std::stringstream updateId;
  updateId << "UPDATE foodItems SET quantity = " << newQuantity << " WHERE id = " << id
           << ";";

  FoodItem foodItem;
  int sqlReturn =
      sqlite3_exec(database, updateId.str().c_str(), nullptr, nullptr, &errorMessage);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "Error updating food item";
  }

  sqlite3_close(database);
}

/**
 * Callback function for reading a food item by its id.
 *
 * @param passedFoodItem The food item to put the read out data into
 * @param numColumns How many columns in the table
 * @param columns The values stored in the columns
 * @param columnNames The names of the columns
 * @return Always 0
 */
int readFoodItemByIdCallback(void* passedFoodItem,
                             int numColumns,
                             char** columns,
                             char** columnNames) {
  FoodItem* foodItem = static_cast<FoodItem*>(passedFoodItem);

  for (int i = 0; i < numColumns; i++) {
    std::string columnValue(columns[i], strlen(columns[i]));
    std::string columnName(columnNames[i], strlen(columnNames[i]));
    setFoodItem(*foodItem, columnValue, columnName);
  }
  return 0;
}

/**
 * Callback function for reading a food item by its id.
 *
 * @param foodItemVector Vector to store read food items into
 * @param numColumns How many columns in the table
 * @param columns The values stored in the columns
 * @param columnNames The names of the columns
 * @return Always 0
 */
int readAllFoodItemsCallback(void* foodItemVector,
                             int numColumns,
                             char** columns,
                             char** columnNames) {
  std::vector<FoodItem>* allFoodItems =
      static_cast<std::vector<FoodItem>*>(foodItemVector);
  struct FoodItem foodItem;

  for (int i = 0; i < numColumns; i++) {
    std::string columnValue(columns[i], strlen(columns[i]));
    std::string columnName(columnNames[i], strlen(columnNames[i]));
    setFoodItem(foodItem, columnValue, columnName);
  }
  allFoodItems->push_back(foodItem);
  return 0;
}
