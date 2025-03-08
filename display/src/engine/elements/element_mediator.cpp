#include "dropdown.h"
#include "element.h"
#include "element_mediator.h"
#include "scroll_box.h"

void Mediator::addScrollBox(std::shared_ptr<ScrollBox> scrollBox) {
  this->scrollBox = scrollBox;
  scrollBox->setMediator(shared_from_this());
}
void Mediator::addDropdown(std::shared_ptr<Dropdown> dropDown) {
  this->dropDown = dropDown;
  dropDown->setMediator(shared_from_this());
}

void Mediator::notify(std::shared_ptr<Element> sender, const std::string& event) {
  if (event == "sort_changed") {
    std::cout << "sort changed" << std::endl;
  }
}
