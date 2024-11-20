#include <iostream>
#include <sdsl/suffix_arrays.hpp>
#include <fm_index/fm_index.hpp>
int main() {
  std::string query = "winkle";
  edaa::fm_index<> fmi("example.txt", "fmi");
  std::cout << fmi.size_in_mb() << '\n';
  std::cout << fmi.count(query) << '\n';
  return 0;
}
