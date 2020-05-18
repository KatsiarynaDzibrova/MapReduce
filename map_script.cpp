#include <iostream>

int main() {
  std::string key;
  while (!std::cin.eof()) {
    getline(std::cin, key, '\t');
    while (std::cin.peek() != '\n') {
      std::string value;
      std::cin >> value;
      std::cout << value << "\t1\n";
    }
    std::cin.ignore(2);
  }
  return 0;
}

