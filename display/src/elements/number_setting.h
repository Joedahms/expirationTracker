#ifndef NUMBER_SETTING_H
#define NUMBER_SETTING_H

#include <vector>

#include "button.h"
#include "element.h"
#include "text.h"

/**
 * Element that contains two buttons with some text sandwiched between them.
 * Used to display a setting and allow the user to change said setting.
 */
class NumberSetting : public CompositeElement {
public:
  NumberSetting(const struct DisplayGlobal& displayGlobal,
                const std::string& logFile,
                const SDL_Rect boundaryRectangle,
                const int settingId = -1);

  void setSettingId(const int& newSettingId);
  void updateSelf() override;
  void handleEventSelf(const SDL_Event& event) override;

private:
  bool databaseConnected = false;
  int settingValue       = -1;
  int settingId;
};

#endif
