#include <glog/logging.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Sample command. rpicam-apps allows bulding customized behavior
 * run libcamera-hello --list-cameras to get camera names
 */
#define IMAGE_DIR "/home/geromy/Desktop/Project/raspi-yolo/images/"

/**
 * Takes photos from both camera modules simultaneously using separate processes.
 * Photos should go to "../../images"
 * Image format "image_x_T" || "image_x_S", where x == angle.
 *
 * @param angle - The angle at which the photos are taken. (0-8, where 8==0)
 * @return None
 */
bool takePhotos(int angle) {
  std::string topPhoto = std::string(IMAGE_DIR) + std::to_string(angle) + "_top.jpg";
  google::ShutdownGoogleLogging();

  pid_t pidt = fork();
  if (pidt == -1) {
    LOG(FATAL) << "Error starting top camera process." << strerror(errno);
    return false;
  }
  if (pidt == 0) {
    // Reinitialize glog in the child process
    google::InitGoogleLogging("TakePhotoChild");
    LOG(INFO) << "Capturing top at position " << angle;
    execl("/usr/bin/rpicam-jpeg", "rpicam-jpeg", "1920:1080:12:U", "--nopreview",
          "--output", topPhoto.c_str(), "--timeout", "50", (char*)NULL);

    std::cerr << "Error: Failed to execute rpicam-still - top" << std::endl;
    LOG(FATAL) << "Error: Failed to execute rpicam-still - top";
    exit(1);
  }

  std::string sidePhoto = std::string(IMAGE_DIR) + std::to_string(angle) + "_side.jpg";
  google::ShutdownGoogleLogging();

  pid_t pids = fork();
  if (pids == -1) {
    LOG(FATAL) << "Error starting side camera process." << strerror(errno);
    return false;
  }
  if (pids == 0) {
    // Reinitialize glog in the child process
    google::InitGoogleLogging("TakePhotoChild");
    LOG(INFO) << "Capturing side at position " << angle;
    execl("/usr/bin/rpicam-jpeg", "rpicam-jpeg", "1920:1080:12:U", "--nopreview",
          "--output", sidePhoto.c_str(), "--timeout", "50", (char*)NULL);

    std::cerr << "Error: Failed to execute rpicam-still - side" << std::endl;
    LOG(FATAL) << "Error: Failed to execute rpicam-still - side";
    exit(1);
  }
  google::InitGoogleLogging("HardwareParent");
  LOG(INFO) << "Exiting photos at " << angle;
  return true;
}

/**
 * Takes a photo using rpicam-still and saves it to the images directory.
 *
 * @param angle Used to create the fileName for the image.
 * @return None - can add a 0 or -1 for success/failure
 */
bool takePhoto(int angle) {
  std::string fileName = std::string(IMAGE_DIR) + std::to_string(angle) + "_test.jpg";
  google::ShutdownGoogleLogging();

  pid_t pid = fork();
  if (pid == -1) {
    LOG(FATAL) << "Error starting camera process." << strerror(errno);
    return false;
  }
  if (pid == 0) {
    // Reinitialize glog in the child process
    google::InitGoogleLogging("TakePhotoChild");
    LOG(INFO) << "Capturing at position " << angle;
    execl("/usr/bin/rpicam-jpeg", "rpicam-jpeg", "1920:1080:12:U", "--nopreview",
          "--output", fileName.c_str(), // Save location
          "--timeout", "50", (char*)NULL);

    std::cerr << "Error: Failed to execute rpicam-still" << std::endl;
    exit(1);
  }
  google::InitGoogleLogging("HardwareParent");
  LOG(INFO) << "Exiting photos at " << angle;
  return true;
}
