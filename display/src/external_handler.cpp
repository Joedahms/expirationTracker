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
                               "name TEXT,"
                               "catagory TEXT,"
                               "scanDateYear INTEGER,"
                               "scanDateMonth INTEGER,"
                               "scanDateDay INTEGER,"
                               "expirationDateYear INTEGER,"
                               "expirationDateMonth INTEGER,"
                               "expirationDateDay INTEGER,"
                               "weight FLOAT,"
                               "quantity INTEGER);";
  char* errorMessage         = nullptr;
  sqlReturn = sqlite3_exec(database, createSqlTable, NULL, NULL, &errorMessage);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "SQL Error: " << errorMessage;
    sqlite3_free(errorMessage);
  }

  const char* insertSql = "INSERT INTO foodItems (name, catagory) VALUES (?, ?);";

  sqlite3_stmt* statement;
  sqlReturn = sqlite3_prepare_v2(database, insertSql, -1, &statement, nullptr);
  if (sqlReturn != SQLITE_OK) {
    LOG(FATAL) << "Prepare error: " << sqlite3_errmsg(database);
  }

  sqlite3_bind_text(statement, 1, foodItem.name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, foodItem.catagory.c_str(), -1, SQLITE_TRANSIENT);

  sqlReturn = sqlite3_step(statement);
  if (sqlReturn != SQLITE_DONE) {
    LOG(FATAL) << "Execution Error: " << sqlite3_errmsg(database);
  }
  sqlite3_finalize(statement);
  /*
  int rc = sqlite3_exec(database, insert_sql, 0, 0, &errorMessage);
  if (rc != SQLITE_OK) {
    LOG(FATAL) << "Failed to insert: " << errorMessage;
    sqlite3_free(errorMessage);
    sqlite3_close(database);
  }
  */

  sqlite3_close(database);
  std::cout << "here" << std::endl;
}
