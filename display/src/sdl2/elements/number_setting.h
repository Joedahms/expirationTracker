#define DECREMENT_SETTING 0
#define INCREMENT_SETTING 1

#ifndef NUMBER_SETTING_H
#define NUMBER_SETTING_H

#include <vector>

#include "button.h"
#include "element.h"
#include "text.h"

class NumberSetting : public Element {
public:
  NumberSetting(struct DisplayGlobal displayGlobal, int settingId);
  void setSettingId(const int& newSettingId);
  void update() override;
  void render() const override;

private:
  Button decreaseButton;
  Button increaseButton;
  Text settingValueText;

  int settingValue = -1;
  int settingId    = -1;
};

#endif
