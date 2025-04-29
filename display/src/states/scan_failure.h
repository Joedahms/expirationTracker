#ifndef SCAN_FAILURE_H
#define SCAN_FAILURE_H

#include "state.h"

class ScanFailure : public State {
public:
  ScanFailure(const struct DisplayGlobal& displayGlobal, const EngineState& state);
  void render() const override;
  void exit() override;
};

#endif
