#ifndef ZERO_WEIGHT_H
#define ZERO_WEIGHT_H

#include "state.h"

class ZeroWeight : public State {
public:
  ZeroWeight(struct DisplayGlobal displayGlobal);
  void render() const override;

  void setRetryScan(bool retryScan);
  bool getRetryScan();

private:
  Logger logger;

  bool retryScan = false;
  void retry();
  void override();
  void cancel();
};

#endif
