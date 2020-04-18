#include <iostream>

int main() {
  std::string key;
  while (!std::cin.eof()) {
    while (getchar() != '\t') {}
    while (std::cin.peek() != '\n') {
      std::string x;
      std::cin >> x;
      std::cout << x << "\t1\n";
    }
    std::cin.ignore(2);
  }
  return 0;
}
