#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Sample command. rpicam-apps allows bulding customized behavior
 * run libcamera-hello --list-cameras to get camera names
 */
#define IMAGE_DIR   "../../images/image_"
#define CAMERA1_CMD "rpicam-still --camera 0 -o "
#define CAMERA2_CMD "rpicam-still --camera 1 -o "

/**
 * Takes photos from both camera modules simultaneously using separate processes.
 * Photos should go to "../../images"
 * Image format "image_x_T" || "image_x_S", where x == angle.
 *
 * @param angle - The angle at which the photos are taken. (0-8, where 8==0)
 * @return None
 */
void takePhotos(int angle) {
  pid_t top_cam = fork();
  if (top_cam == -1) {
    LOG(FATAL) << "Error starting top camera process.";
  }
  else if (top_cam == 0) {
    std::string top_photo = std::string(CAMERA1_CMD) + std::string(IMAGE_DIR) +
                            std::to_string(angle) + "_T.jpg --nopreview";
    if (system(top_photo.c_str()) == -1) {
      LOG(FATAL) << "Failed to capture image from top camera.";
    }
    exit(0);
  }

  pid_t side_cam = fork();
  if (side_cam == -1) {
    LOG(FATAL) << "Error starting side camera process.";
  }
  else if (side_cam == 0) {
    std::string side_photo = std::string(CAMERA2_CMD) + IMAGE_DIR +
                             std::to_string(angle) + "_S.jpg --nopreview";
    if (system(side_photo.c_str()) == -1) {
      LOG(FATAL) << "Failed to capture image from side camera.";
    }
    exit(0);
  }

  waitpid(top_cam, NULL, 0);
  waitpid(side_cam, NULL, 0);

  LOG(INFO) << "Photos successful at position " << angle;
}

/**
 * Takes a photo using the Raspberry Pi camera module.
 * https://github.com/jeffmachyo/Raspberry-Pi-Camera-C-and-CPP.git
 *
 * @param filename The name of the file to save the photo to.
 * @return None
 */
static pid_t pid = 0;

void takePic(char* filename) {
  if ((pid = fork()) == 0) {
    execl("/usr/bin/raspistill", "/usr/bin/raspistill", "-n", "-vf", "-o", filename,
          NULL);
  }
}