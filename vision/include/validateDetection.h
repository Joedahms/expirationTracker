#ifndef VALIDATE_DETECTION_H
#define VALIDATE_DETECTION_H

#include <chrono>
#include <glog/logging.h>
#include <map>
#include <string>
#include <unordered_set>

enum class TextValidationResult {
  NOT_VALID,
  POSSIBLE_CLASSIFICATION,
  POSSIBLE_EXPIRATION_DATE
};

// Define valid ImageNet food classes using an unordered_set for fast lookup
const std::unordered_set<int> validFoodClasses = {
    924, 925, 926, 927, 928, 929, 930, 931, 932, 933, 934, 935, 936, 937, 938,
    939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952, 953,
    954, 955, 956, 957, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969};

// Optional: Map of valid food class indices to their corresponding names
const std::map<int, std::string> foodLabels = {{924, "guacamole"},
                                               {925, "consomme"},
                                               {926, "hot_pot"},
                                               {927, "trifle"},
                                               {928, "ice_cream"},
                                               {929, "ice_lolly"},
                                               {930, "French_loaf"},
                                               {931, "bagel"},
                                               {932, "pretzel"},
                                               {933, "cheeseburger"},
                                               {934, "hotdog"},
                                               {935, "mashed_potato"},
                                               {936, "head_cabbage"},
                                               {937, "broccoli"},
                                               {938, "cauliflower"},
                                               {939, "zucchini"},
                                               {940, "spaghetti_squash"},
                                               {941, "acorn_squash"},
                                               {942, "butternut_squash"},
                                               {943, "cucumber"},
                                               {944, "artichoke"},
                                               {945, "bell_pepper"},
                                               {946, "cardoon"},
                                               {947, "mushroom"},
                                               {948, "Granny_Smith"},
                                               {949, "strawberry"},
                                               {950, "orange"},
                                               {951, "lemon"},
                                               {952, "fig"},
                                               {953, "pineapple"},
                                               {954, "banana"},
                                               {955, "jackfruit"},
                                               {956, "custard_apple"},
                                               {957, "pomegranate"},
                                               {959, "carbonara"},
                                               {960, "chocolate_sauce"},
                                               {961, "dough"},
                                               {962, "meat_loaf"},
                                               {963, "pizza"},
                                               {964, "potpie"},
                                               {965, "burrito"},
                                               {966, "red_wine"},
                                               {967, "espresso"},
                                               {968, "cup"},
                                               {969, "eggnog"}};

// Function to check if a predicted class is a valid food item
bool isValidClassification(int);
TextValidationResult isValidText(
    const std::string&, bool&, bool&, std::string&, std::string&);
std::pair<bool, std::string> isTextClass(const std::string&);
bool isExpirationDate(const std::string&);
std::chrono::year_month_day parseExpirationDate(const std::string&);
inline std::string getFoodLabel(int predictedClass) {
  if (!validFoodClasses.contains(predictedClass)) {
    LOG(FATAL) << "Illegal item classified by model"
               << "ImageNet index: " << predictedClass;
  }
  return foodLabels.at(predictedClass);
}

#endif