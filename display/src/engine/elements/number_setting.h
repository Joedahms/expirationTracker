#ifndef NUMBER_SETTING_H
#define NUMBER_SETTING_H

#include <vector>

#include "button.h"
#include "element.h"
#include "text.h"

class NumberSetting : public CompositeElement {
public:
  NumberSetting(struct DisplayGlobal displayGlobal, int settingId);
  void setSettingId(const int& newSettingId);
  void handleMouseButtonDown(const SDL_Point& mousePosition);

  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event) override;

private:
  int settingValue = -1;
  int settingId    = -1;
};

#endif
