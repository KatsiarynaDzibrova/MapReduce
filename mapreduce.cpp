#include <boost/process.hpp>
#include <iostream>
#include <queue>
#include <unordered_map>

namespace bp = boost::process;

enum ArgType{
  COMMAND = 1,
  SCRIPT_ADDRESS = 2,
  IN_FILE = 3,
  OUT_FILE = 4,
};


const uint64_t BUFF_SIZE = 4000;

std::vector<double> buffer;

void SortOneArray(int64_t size) {
  std::priority_queue<double, std::vector<double>, std::greater<>> queue;
  for (int i = 0; i < size; ++i) {
    queue.push(buffer[i]);
  }
  for (int i = 0; i < size; ++i) {
    buffer[i] = queue.top();
    queue.pop();
  }
}

void Sort(const std::string& in_file, const std::string& out_file) {
  std::ifstream fin(in_file);
  std::ofstream fout(out_file);
  int n;
  fin >> n;
  std::vector<std::string> names;
  int num = 0;
  std::vector<int64_t> sizes;
  buffer.reserve(BUFF_SIZE);
  while(!fin.eof()) {
    names.push_back("temp" + std::to_string(num) + ".txt");
    std::ofstream file(names[num]);
    int size;
    for (size = 0; size < BUFF_SIZE; ++size) {
      double key;
      int val;
      if (!(fin >> key >> val)){
        break;
      }
      buffer[size] = key;
    }
    sizes.push_back(size);
    SortOneArray(size);
    for (int j = 0; j < size; ++j) {
      file << buffer[j] << "\t";
    }
    file.close();
    num++;
  }
  std::priority_queue<std::vector<double>,
                      std::vector<std::vector<double>>,
                      std::greater<>> queue;
  std::vector<std::ifstream> files(num);
  for (uint64_t i = 0; i < num; ++i) {
    files[i] = std::ifstream(names[i]);
    files[i].seekg(0, std::ios::beg);
    files[i] >> buffer[i];
    queue.push({buffer[i], (double)i});
  }
  double write_buffer[BUFF_SIZE];
  uint64_t i = 0;
  while (!queue.empty()) {
    auto top = queue.top();
    write_buffer[i] = top[0];
    sizes[top[1]]--;
    if (sizes[top[1]] > 0) {
      double x;
      files[top[1]] >> x;
      queue.push({x, top[1]});
    }
    queue.pop();
    i++;
    if (i >= BUFF_SIZE) {
      i = 0;
      for(const auto& it : write_buffer) {
        fout << it << "\t1\n";
      }
    }
  }
  for(int j = 0; j < i; ++j) {
    fout << write_buffer[j] << "\t1\n";
  }
}

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
    std::ifstream unsorted_input(args[IN_FILE]);
    Sort(args[IN_FILE], "sorted.txt");
    std::ifstream sorted_input("sorted.txt");
    double first_key;
    std::vector<std::string> output_files;
    sorted_input >> first_key;
    int i = 0;
    while(!sorted_input.eof()){
      i++;
      std::cout << first_key << ": ";
      std::string current_reduce_input = tmpnam(nullptr);
      std::ofstream cur_in(current_reduce_input);
      int el;
      sorted_input >> el;
      std::vector<int> values;
      std::cout << el << " ";
      double cur_key;
      cur_in << first_key << '\t' << el << '\n';
      while(sorted_input >> cur_key) {
        if (first_key != cur_key){
          break;
        }
        std::cout << el << " ";
        sorted_input >> el;
        std::cout << "write: " << first_key << '\t' << el << '\n';
        cur_in << first_key << '\t' << el << '\n';
      }
      std::cout << std::endl;
      first_key = cur_key;
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
    int num;
    unsorted_input >> num;
    global_output << num << "\n";
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

