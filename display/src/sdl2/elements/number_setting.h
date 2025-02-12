#ifndef NUMBER_SETTING_H
#define NUMBER_SETTING_H

#include <vector>

#include "button.h"
#include "element.h"
#include "text.h"

class NumberSetting : public Element {
public:
  NumberSetting();
  void setSettingId(const int& newSettingId);
  void update() override;
  void render() const override;

private:
  std::unique_ptr<Button> decreaseButton;
  std::unique_ptr<Button> increaseButton;
  int settingValue;
  std::unique_ptr<Text> settingValueText;
  int settingId = -1;
};

#endif
