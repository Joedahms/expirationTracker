#ifndef ELEMENT_MEDIATOR_H
#define ELEMENT_MEDIATOR_H

#include <memory>
#include <vector>

class ScrollBox;
class Dropdown;
class Element;

class Mediator : public std::enable_shared_from_this<Mediator> {
public:
  void addScrollBox(std::shared_ptr<ScrollBox> scrollBox);
  void addDropdown(std::shared_ptr<Dropdown> dropDown);
  void notify(std::shared_ptr<Element> sender, const std::string& event);

private:
  std::shared_ptr<ScrollBox> scrollBox;
  std::shared_ptr<Dropdown> dropDown;
};

#endif
