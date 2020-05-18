#include <iostream>
#include <cmath>

int main() {
  int num;
  std::cin >> num;
  std::cout << num << "\n";
  double key;
  while (std::cin >> key) {
    key = (double)(int)(key * num) / num;
    std::cout << key << "\t1\n";
  }
  return 0;
}

