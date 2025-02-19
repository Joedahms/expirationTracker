#ifndef EXTERNAL_HANDLER_H
#define EXTERNAL_HANDLER_H

#include <sqlite3.h>

#include "../../food_item.h"
#include "../../pipes.h"

class DisplayHandler {
public:
  DisplayHandler(struct Pipes externalPipes, int* displayToEngine, int* engineToDisplay);
  void handleExternal();
  void handleEngine();

private:
  struct Pipes externalPipes;
  int* displayToEngine;
  int* engineToDisplay;

  void sendStartSignal();
};

#endif
