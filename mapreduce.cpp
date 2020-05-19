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
    names.push_back(tmpnam(nullptr));
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
    int block_size = 5;
    std::ifstream fin(args[IN_FILE]);
    int j = 0;
    int number_of_bins;
    fin >> number_of_bins;
    fin.ignore(1);
    std::ofstream all_maps_output(args[OUT_FILE]);
    all_maps_output << number_of_bins << std::endl;
    std::vector<bp::child> map_processes;
    std::vector<std::string> output_files;
    while(!fin.eof()) {
      j++;
      std::string current_map_input = tmpnam(nullptr);
      std::ofstream cur_in(current_map_input);
      std::vector<std::string> lines;
      for (int i = 0; i < block_size; ++i) {
        std::string line;
        std::getline(fin, line);
        lines.push_back(line);
        if ( line == "\n") break;
      }
      cur_in << number_of_bins << "\n";
      for(const auto& el : lines) {
        cur_in << el << "\n";
      }
      std::string current_map_output = tmpnam(nullptr);
      output_files.push_back(current_map_output);
      map_processes.emplace_back(args[SCRIPT_ADDRESS],
                                 bp::std_out > current_map_output,
                                 bp::std_in < current_map_input);
      cur_in.close();
    }
    for(auto& child : map_processes) {
      child.wait();
    }
    for(const auto& file_name : output_files) {
      std::ifstream one_map_output(file_name);
      std::string line;
      while(getline(one_map_output, line)) {
        if (line == "\n") continue;
        all_maps_output << line << "\n";
      }
      one_map_output.close();
    }
  } else if (std::string(args[COMMAND]) == "reduce") {
    std::ifstream unsorted_input(args[IN_FILE]);
    Sort(args[IN_FILE], "sorted.txt");
    std::ifstream sorted_input("sorted.txt");
    double first_key;
    std::vector<std::string> output_files;
    sorted_input >> first_key;
    int i = 0;
    std::vector<bp::child> reduce_processes;
    while(!sorted_input.eof()){
      i++;
      std::string current_reduce_input = tmpnam(nullptr);
      std::ofstream cur_in(current_reduce_input);
      int el;
      sorted_input >> el;
      std::vector<int> values;
      double cur_key;
      cur_in << first_key << '\t' << el << '\n';
      while(sorted_input >> cur_key) {
        if (first_key != cur_key){
          break;
        }
        sorted_input >> el;
        cur_in << first_key << '\t' << el << '\n';
      }
      first_key = cur_key;
      cur_in.close();
      std::string current_reduce_output = tmpnam(nullptr);
      output_files.push_back(current_reduce_output);
      std::ofstream cur_out(current_reduce_output);
      reduce_processes.emplace_back(bp::child(args[SCRIPT_ADDRESS],
                                              bp::std_out > current_reduce_output,
                                              bp::std_in < current_reduce_input));
    }
    for(auto& child : reduce_processes){
      child.wait();
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

