#include <iostream>
#include <boost/process.hpp>
#include <unordered_map>

namespace bp = boost::process;

int main(int arg, char** args) {
  int result = 0;
  if (std::string(args[1]) == "map") {
    result = bp::system(args[2], bp::std_out > args[4], bp::std_in < args[3]);
  } else if (std::string(args[1]) == "reduce") {
    std::unordered_map<std::string, std::vector<int>> words_map;
    std::ifstream global_input(args[3]);
    std::string s;
    while (global_input >> s) {
      int x;
      global_input >> x;
      if (words_map.find(s) != words_map.end()) {
        words_map.at(s).push_back(x);
      } else {
        words_map[s] = std::vector<int>{x};
      }
    }
    std::vector<std::string> output_files;
    for (const auto&[key, values] : words_map) {
      std::string current_reduce_input = tmpnam(nullptr);
      std::ofstream cur_in(current_reduce_input);
      for (const auto& el : values) {
        cur_in << key << '\t' << el << '\n';
      }
      cur_in.close();
      std::string current_reduce_output = tmpnam(nullptr);
      output_files.push_back(current_reduce_output);
      std::ofstream cur_out(current_reduce_output);
      result += bp::system(args[2],
                           bp::std_out > current_reduce_output,
                           bp::std_in < current_reduce_input);
    }
    std::ofstream global_output(args[4]);
    for (const auto& input_name : output_files) {
      std::ifstream current_input(input_name);
      std::string cur_string;
      std::getline(current_input, cur_string);
      global_output << cur_string << '\n';
    }
  } else {
    std::cerr << "unknown command: " << args[1] << '\n';
    result = 1;
  }
  if (result != 0) result = 1;
  return result;
}
