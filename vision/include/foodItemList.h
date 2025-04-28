#ifndef FOOD_ITEM_LIST_H
#define FOOD_ITEM_LIST_H

#include <string>
#include <unordered_map>

struct FoodInfo {
  int shelfLife;
};

inline std::unordered_map<std::string, FoodInfo> knownFoodData = {
    {"Apple", {30}},
    {"Bagel", {5}},
    {"Banana", {7}},
    {"Beer", {365}},
    {"Bell pepper", {7}},
    {"Bread", {7}},
    {"Broccoli", {7}},
    {"Cabbage", {21}},
    {"Cake", {5}},
    {"Cantaloupe", {10}},
    {"Carrot", {21}},
    {"Cheese", {30}},
    {"Coconut", {60}},
    {"Coffee", {180}},
    {"Common fig", {7}},
    {"Cookie", {30}},
    {"Cucumber", {10}},
    {"Doughnut", {3}},
    {"Egg", {21}},
    {"Garden Asparagus", {5}},
    {"Grape", {7}},
    {"Grapefruit", {21}},
    {"Guacamole", {3}},
    {"Lemon", {30}},
    {"Mango", {10}},
    {"Mushroom", {7}},
    {"Orange", {14}},
    {"Pancake", {3}},
    {"Pasta", {180}},
    {"Peach", {5}},
    {"Pear", {7}},
    {"Potato", {60}},
    {"Pumpkin", {90}},
    {"Radish", {14}},
    {"Salt and pepper shakers", {3650}},
    {"Squash", {30}},
    {"Strawberry", {5}},
    {"Sushi", {1}},
    {"Tomato", {7}},
    {"Watermelon", {14}},
    {"Wine", {3650}},
    {"Zucchini", {10}},
    /*
      // Additional items
      {"Braeburn Apple", {200, 30}},
      {"Cortland Apple", {200, 30}},
      {"Fuji Apple", {200, 30}},
      {"Gala Apple", {200, 30}},
      {"Honeycrisp Apple", {220, 30}},
      {"McIntosh Apple", {180, 30}},
      {"Red Delicious Apple", {210, 30}},
      {"Golden Delicious Apple", {210, 30}},
      {"Hass Avocado", {200, 7}},
      {"Large Hass Avocado", {250, 7}},
      {"Florida Avocado", {300, 7}},
      {"Banana (Small)", {100, 7}},
      {"Red Banana", {120, 7}},
      {"Green Bell Pepper", {170, 7}},
      {"Red Bell Pepper", {160, 7}},
      {"Orange Bell Pepper", {160, 7}},
      {"Yellow Bell Pepper", {160, 7}},
      {"Mangosteen", {50, 10}},
      {"Mulberry", {15, 3}},
      {"Gooseberry", {20, 5}},
      {"Lemon (Large)", {80, 30}},
      {"Lime", {60, 30}},
      {"Navel Orange", {220, 14}},
      {"Valencia Orange", {200, 14}},
      {"Blood Orange", {200, 14}},
      {"Cara Cara Orange", {210, 14}},
      {"Pink Grapefruit", {250, 21}},
      {"Cantaloupe (Large)", {1800, 10}},
      {"Seedless Watermelon", {9000, 14}},
      {"Watermelon with Seeds", {9500, 14}},
      {"Asian Pear", {300, 14}},
      {"Green Anjou Pear", {275, 14}},
      {"Red Anjou Pear", {275, 14}},
      {"Bartlett Pear", {280, 14}},
      {"Bosc Pear", {290, 14}},
      {"Mango (Large)", {250, 10}},
      {"Ataulfo (Honey) Mango", {150, 10}},
      {"Guava", {150, 7}},
      {"Lychee", {20, 5}},
      {"Plantain", {250, 10}},
      {"Cherimoya", {300, 7}},
      {"Black Plum", {150, 7}},
      {"Pomegranate", {300, 30}},
      {"Russet Potato", {300, 60}},
      {"Sweet Potato", {250, 60}},
      {"Beefsteak Tomato", {250, 7}},
      {"Heirloom Tomato", {300, 7}},
      {"Roma Tomato", {150, 7}},
      {"Loose Carrots", {80, 21}},
      {"Celery", {400, 14}},
      {"Iceberg Lettuce", {700, 10}},
      {"Zucchini (Large)", {400, 10}},
      {"Butternut Squash", {1200, 30}},
      {"Starfruit (Carambola)", {125, 7}
    }
      */
};

inline std::chrono::year_month_day calculateExpirationDate(
    const std::chrono::year_month_day& scanDate, const std::string& foodItem) {
  std::chrono::sys_days sysScanDate = scanDate;
  sysScanDate += std::chrono::days{knownFoodData[foodItem].shelfLife};
  return sysScanDate;
}

#endif
