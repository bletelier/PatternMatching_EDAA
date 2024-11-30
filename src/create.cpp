#include <iostream>
#include <fm_index/fm_index.hpp>
#include <suffix_array/suffix_array.hpp>
#include <csa/csa.hpp>
#include <chrono>
int main(int argc, char* argv[]) {
  if(argc < 3) {
    std::cout << "Ejecute como ./build/main <input_filename> <out_filename>";
    return 1;
  }


  auto begin_time = std::chrono::high_resolution_clock::now();
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::nano> elapsed_time = end_time - begin_time;

  std::string input = argv[1];
  std::string output= argv[2];
  std::cout << "Creating from: " << input << '\n';

  std::vector<double> times(3, 0);

  std::cout << "Building FMI\n"; 
  begin_time = std::chrono::high_resolution_clock::now();
  edaa::fm_index<> fmi(input); 
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - begin_time;
  times[0] = elapsed_time.count();
  /*for(int i = 1; i < 1; ++i) {
    begin_time = std::chrono::high_resolution_clock::now();
    fmi = edaa::fm_index<>(input);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[0] = std::min(times[0], elapsed_time.count());
  }*/

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

  std::cout << "Building SA\n"; 
  begin_time = std::chrono::high_resolution_clock::now();
  edaa::suffix_array sa(input); 
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - begin_time;
  times[1] = elapsed_time.count();
  /*for(int i = 1; i < 1; ++i) {
    begin_time = std::chrono::high_resolution_clock::now();
    sa = edaa::suffix_array(input);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[1] = std::min(times[1], elapsed_time.count());
  }*/
  std::string sa_o = "cds/sa_"+output+".sdsl";
  sdsl::store_to_file(sa, sa_o);

  std::cout << "Building CSA\n"; 
  begin_time = std::chrono::high_resolution_clock::now();
  edaa::compressed_sa csa(input); 
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - begin_time;
  times[2] = elapsed_time.count();
  /*for(int i = 1; i < 1; ++i) {
    begin_time = std::chrono::high_resolution_clock::now();
    csa = edaa::compressed_sa(input);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[2] = std::min(times[2], elapsed_time.count());
  }*/

  std::string csa_o = "cds/csa_"+output+".sdsl";
  sdsl::store_to_file(csa, csa_o);

  //std::cout << csa.count("winkle") << '\n';
  //std::cout << fmi.count("winkle") << '\n';
  //std::cout << sa.count_without("winkle") << '\n';
  

  std::ofstream time_data;
  time_data.open("res/create.csv", std::ios::app);
  time_data << "FMI,"<< output << "," << times[0]<<","<<sdsl::size_in_mega_bytes(fmi) << '\n';
  time_data << "SA,"<< output << "," << times[1] <<","<<sdsl::size_in_mega_bytes(sa) << '\n';
  time_data << "CSA,"<< output << "," << times[2] <<sdsl::size_in_mega_bytes(csa) << '\n';
  time_data.close();


  std::cout << "Done\n\n";

  /*edaa::suffix_array saa;
  std::string inputxx = "cds/sa_"+output+ ".sdsl";
  std::ifstream input_file(inputxx);
  saa.load(input_file);

  std::cout << saa.count_without("winkle") << '\n';*/
  return 0;
}
