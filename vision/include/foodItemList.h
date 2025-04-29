#ifndef FOOD_ITEM_LIST_H
#define FOOD_ITEM_LIST_H

#include <string>
#include <unordered_map>

struct FoodInfo {
  int shelfLife;
};

inline std::unordered_map<std::string, FoodInfo> knownFoodData = {
    // Open Image foods
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

    // PLU mapped items
    {"Braeburn Apple", {30}},
    {"Cortland Apple", {30}},
    {"Fuji Apple", {30}},
    {"Gala Apple", {30}},
    {"Honeycrisp Apple", {30}},
    {"McIntosh Apple", {30}},
    {"Red Delicious Apple", {30}},
    {"Golden Delicious Apple", {30}},
    {"Hass Avocado", {7}},
    {"Large Hass Avocado", {7}},
    {"Florida Avocado", {7}},
    {"Banana (Small)", {7}},
    {"Red Banana", {7}},
    {"Green Bell Pepper", {7}},
    {"Red Bell Pepper", {7}},
    {"Orange Bell Pepper", {7}},
    {"Yellow Bell Pepper", {7}},
    {"Mangosteen", {10}},
    {"Mulberry", {3}},
    {"Gooseberry", {5}},
    {"Lemon (Large)", {30}},
    {"Lime", {30}},
    {"Navel Orange", {14}},
    {"Valencia Orange", {14}},
    {"Blood Orange", {14}},
    {"Cara Cara Orange", {14}},
    {"Pink Grapefruit", {21}},
    {"Cantaloupe (Large)", {10}},
    {"Seedless Watermelon", {14}},
    {"Watermelon with Seeds", {14}},
    {"Asian Pear", {14}},
    {"Green Anjou Pear", {14}},
    {"Red Anjou Pear", {14}},
    {"Bartlett Pear", {14}},
    {"Bosc Pear", {14}},
    {"Mango (Large)", {10}},
    {"Ataulfo (Honey) Mango", {10}},
    {"Guava", {7}},
    {"Lychee", {5}},
    {"Plantain", {10}},
    {"Cherimoya", {7}},
    {"Black Plum", {7}},
    {"Pomegranate", {30}},
    {"Russet Potato", {60}},
    {"Sweet Potato", {60}},
    {"Beefsteak Tomato", {7}},
    {"Heirloom Tomato", {7}},
    {"Roma Tomato", {7}},
    {"Loose Carrots", {21}},
    {"Celery", {14}},
    {"Iceberg Lettuce", {10}},
    {"Zucchini (Large)", {10}},
    {"Butternut Squash", {30}},
    {"Starfruit (Carambola)", {7}}};

inline std::chrono::year_month_day calculateExpirationDate(
    const std::chrono::year_month_day& scanDate, const std::string& foodItem) {
  std::chrono::sys_days sysScanDate = scanDate;
  sysScanDate += std::chrono::days{knownFoodData[foodItem].shelfLife};
  return sysScanDate;
}

#endif
