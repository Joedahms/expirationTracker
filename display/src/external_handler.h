#ifndef EXTERNAL_HANDLER_H
#define EXTERNAL_HANDLER_H

#include <sqlite3.h>

#include "../../pipes.h"

void externalHandler(struct DisplayPipes);
void openDatabase(sqlite3**);

#endif
