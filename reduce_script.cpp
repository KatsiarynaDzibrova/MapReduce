#include <iostream>

int main() {
  std::string key;
  int sum = 0;
  while (std::cin >> key) {
    int value;
    std::cin >> value;
    sum += value;
  }
  std::cout << key << '\t' << sum << std::endl;
  return 0;
}

