#ifndef SCAN_SUCCESS_H
#define SCAN_SUCCESS_H

#include "../elements/panel.h"
#include "state.h"

class ScanSuccess : public State {
public:
  ScanSuccess(const struct DisplayGlobal& displayGlobal, const EngineState& state);
  void render() const override;
  void enter() override;
  void exit() override;

private:
  FoodItem foodItem;
  bool correctItem = false;
  std::shared_ptr<Panel> scannedItemPanel;
};

#endif
