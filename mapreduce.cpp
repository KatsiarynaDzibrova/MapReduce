#include <iostream>
#include <boost/process.hpp>
#include <unordered_map>

namespace bp = boost::process;

enum ArgType{
  COMMAND = 1,
  SCRIPT_ADDRESS = 2,
  IN_FILE = 3,
  OUT_FILE = 4,
};

int main(int arg, char** args) {
  if(arg != 5) {
    std::cerr << "require 5 args, got " << arg << std::endl;
    return 2;
  }
  if (std::string(args[COMMAND]) == "map") {
    int result = bp::system(args[SCRIPT_ADDRESS], bp::std_out > args[OUT_FILE],
        bp::std_in < args[IN_FILE]);
    if(result != 0) {
      std::cerr << "cannot run map" << std::endl;
      return 3;
    }
  } else if (std::string(args[COMMAND]) == "reduce") {
    std::unordered_map<std::string, std::vector<int>> words_map;
    std::ifstream global_input(args[IN_FILE]);
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
      int result = bp::system(args[SCRIPT_ADDRESS],
                           bp::std_out > current_reduce_output,
                           bp::std_in < current_reduce_input);
      if (result != 0) {
        std::cerr << "cannot run reduce" << '\n';
        return 4;
      }
    }
    std::ofstream global_output(args[OUT_FILE]);
    for (const auto& input_name : output_files) {
      std::ifstream current_input(input_name);
      std::string cur_string;
      std::getline(current_input, cur_string);
      global_output << cur_string << '\n';
    }
  } else {
    std::cerr << "unknown command: " << args[COMMAND] << '\n';
    return 5;
  }
  return 0;
}

