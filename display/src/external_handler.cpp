#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>

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

  sqlite3* database;
  int sqlReturn = sqlite3_open("test.db", &database);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "Error opening database: " << sqlite3_errmsg(database);
  }

  const char* createSqlTable = "CREATE TABLE IF NOT EXISTS foodItems("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "name TEXT NOT NULL,"
                               "catagory TEXT NOT NULL,"
                               "scanDateYear INTEGER NOT NULL,"
                               "scanDateMonth INTEGER NOT NULL,"
                               "scanDateDay INTEGER NOT NULL,"
                               "expirationDateYear INTEGER NOT NULL,"
                               "expirationDateMonth INTEGER NOT NULL,"
                               "expirationDateDay INTEGER NOT NULL,"
                               "weight FLOAT NOT NULL,"
                               "quantity INTEGER NOT NULL);";
  char* errorMessage         = nullptr;
  sqlReturn = sqlite3_exec(database, createSqlTable, NULL, NULL, &errorMessage);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "SQL Error: " << errorMessage;
    sqlite3_free(errorMessage);
  }

  sqlite3_close(database);
  std::cout << "here" << std::endl;
}
