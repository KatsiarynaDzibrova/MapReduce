#include <iostream>

int main() {
  std::string s;
  int sum = 0;
  while (std::cin >> s) {
    int x;
    std::cin >> x;
    sum += x;
  }
  std::cout << s << '\t' << sum << std::endl;
  return 0;
}
