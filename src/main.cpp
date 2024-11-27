#include <iostream>
#include <fm_index/fm_index.hpp>
#include <suffix_array/suffix_array.hpp>
int main(int argc, char* argv[]) {
  if(argc < 3) {
    std::cout << "Ejecute como ./build/main <input_filename> <out_filename>";
    return 1;
  }
  std::string input = argv[1];
  std::string output= argv[2];
  edaa::fm_index<> fmi(input);
  std::string fmi_o = "cds/fmi_"+output+".sdsl";
  sdsl::store_to_file(fmi, fmi_o);

  /*std::string input2 = "cds/fmi.sdsl";
  std::ifstream input_file2(input2);
  edaa::fm_index<> fmi2;
  fmi2.load(input_file2);
  
  std::cout << fmi.size_in_mb() << '\n';
  std::cout << fmi.count(query) << '\n';
  std::cout << fmi2.size_in_mb() << '\n';
  std::cout << fmi2.count(query) << '\n';*/

  edaa::suffix_array<> sa(input);
  std::string sa_o = "cds/sa"+output+".sdsl";
  sdsl::store_to_file(sa, sa_o);

  /*edaa::suffix_array<> saa;
  std::string input = "cds/sa.sdsl";
  std::ifstream input_file(input);
  saa.load(input_file);

  sa.print_ids();
  saa.print_ids();*/
  return 0;
}
