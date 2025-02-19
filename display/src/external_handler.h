#ifndef EXTERNAL_HANDLER_H
#define EXTERNAL_HANDLER_H

#include <sqlite3.h>

#include "../../food_item.h"
#include "../../pipes.h"

void externalHandler(struct Pipes pipes, int* displayToSdl);
bool sdlHandler(struct Pipes pipes, int* sdlToDisplay, int* displayToSdl);

#endif
