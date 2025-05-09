#include <iostream>

#include "camera.h"

Camera::Camera(const std::filesystem::path& outputDirectory,
               const std::string& orientation)
    : outputDirectory(outputDirectory) {
  if (orientation == "top") {
    this->orientation  = "top";
    this->cameraNumber = "0";
  }
  else if (orientation == "side") {
    this->orientation  = "side";
    this->cameraNumber = "1";
  }
  else {
    std::cerr << "Invalid camera orientation";
    exit(1);
  }
}

void Camera::takePhoto(const int angle) {
  const std::string rpicam     = "rpicam-jpeg --camera ";
  const std::string preview    = " --nopreview";
  const std::string resolution = " --width 4608 --height 2592";

  const std::string photoPath = " --output " + this->outputDirectory.string() +
                                std::to_string(angle) + "_" + this->orientation + ".jpg";

  const std::string command =
      rpicam + this->cameraNumber + preview + resolution + photoPath;

  system(command.c_str());

  for (const auto& entry : std::filesystem::directory_iterator(this->outputDirectory)) {
    std::cout << entry.path() << std::endl;
  }
}
