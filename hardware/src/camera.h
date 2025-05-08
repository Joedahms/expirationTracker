#ifndef CAMERA_H
#define CAMERA_H

#include <filesystem>
#include <string>

class Camera {
public:
  Camera(const std::filesystem::path& outputDirectory, const std::string& orientation);
  void takePhoto(const int angle);

private:
  const std::filesystem::path outputDirectory;
  std::string orientation;
  std::string cameraNumber;
  // void sendPhoto();
};

#endif
