#include <glog/logging.h>
#include <iostream>

void visionEntry() {
  LOG(INFO) << "Within vision process";
  std::cout << "VISION" << std::endl;
}