#include <iostream>
#include <fm_index/fm_index.hpp>
#include <suffix_array/suffix_array.hpp>
int main() {
  std::string query = "winkle";
  edaa::fm_index<> fmi("example.txt");
  std::string output2 = "cds/fmi.sdsl";
  sdsl::store_to_file(fmi, output2);

  std::string input2 = "cds/fmi.sdsl";
  std::ifstream input_file2(input2);
  edaa::fm_index<> fmi2;
  fmi2.load(input_file2);
  
  std::cout << fmi.size_in_mb() << '\n';
  std::cout << fmi.count(query) << '\n';
  std::cout << fmi2.size_in_mb() << '\n';
  std::cout << fmi2.count(query) << '\n';

  edaa::suffix_array<> sa("example.txt");
  std::string output = "cds/sa.sdsl";
  sdsl::store_to_file(sa, output);

  edaa::suffix_array<> saa;
  std::string input = "cds/sa.sdsl";
  std::ifstream input_file(input);
  saa.load(input_file);

  sa.print_ids();
  saa.print_ids();
  return 0;
}
