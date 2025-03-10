#ifndef ELEMENT_MEDIATOR_H
#define ELEMENT_MEDIATOR_H

#include <memory>
#include <vector>

class ScrollBox;
class Dropdown;
class Element;
class Button;

class Mediator : public std::enable_shared_from_this<Mediator> {
public:
  Mediator(const std::string& logFile);
  void addButton(std::shared_ptr<Button> button);
  void addScrollBox(std::shared_ptr<ScrollBox> scrollBox);
  void addDropdown(std::shared_ptr<Dropdown> dropDown);
  void notify(std::shared_ptr<Element> sender, const std::string& event);

private:
  Logger logger;
  std::shared_ptr<Button> button;
  std::shared_ptr<ScrollBox> scrollBox;
  std::shared_ptr<Dropdown> dropDown;
};

#endif
