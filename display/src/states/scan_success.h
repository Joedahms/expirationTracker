#ifndef SCAN_SUCCESS_H
#define SCAN_SUCCESS_H

#include "state.h"

class ScanSuccess : public State {
public:
  ScanSuccess(struct DisplayGlobal displayGlobal);
  void render() const override;

  void setRetryScan(bool retryScan);
  bool getRetryScan();

private:
  Logger logger;
};

#endif
