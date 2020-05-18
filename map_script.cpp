#include <iostream>

int main() {
  int num;
  std::cin >> num;
  std::cout << num << "\n";
  double key;
  while (std::cin >> key) {
    std::cout << key << "\t1\n";
  }
  return 0;
}

