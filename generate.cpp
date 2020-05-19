#include <fstream>
#include <iostream>
#include <random>

enum ArgType{
  FLAG1 = 1,
  ARG1 = 2,
  FLAG2 = 3,
  ARG2 = 4,
  //optional
  FLAG3 = 5,
  ARG3 = 6
};

int main(int argc, char** argv) {
  if(argc != 5 && argc != 7) {
    std::cerr << "require 5 or 7 args, got " << argc << std::endl;
    return 2;
  }
  std::string file;
  int number_of_lines;
  if (std::string(argv[FLAG1]) == "-n" && std::string(argv[FLAG2]) == "-d"){
    number_of_lines = atoi(argv[ARG1]);
    file = std::string(argv[ARG2]);
  } else if (std::string(argv[FLAG1]) == "-d" && std::string(argv[FLAG2]) ==
  "-n"){
    file = std::string(argv[ARG1]);
    number_of_lines = atoi(argv[ARG2]);
  } else {
    std::cerr << "unexpected flags" << argv[FLAG1] << ", " << argv[FLAG2] <<
    std::endl;
  }
  int number_of_bins = 20;
  if (argc == 7 && std::string(argv[FLAG3]) == "-b"){
    number_of_bins = atoi(argv[ARG3]);
  }
  std::ofstream fout(file);
  fout << number_of_bins  <<"\n";
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution (0.0,1.0);
  for (int i = 0; i < number_of_lines; ++i) {
    double cur_value = distribution(generator);
    fout << cur_value << "\t\n";
  }
  fout.close();
  return 0;
}