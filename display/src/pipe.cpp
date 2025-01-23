#include <glog/logging.h>
#include <iostream>

void displayEntry() {
  LOG(INFO) << "Within display process";
  std::cout << "here" << std::endl;
}
