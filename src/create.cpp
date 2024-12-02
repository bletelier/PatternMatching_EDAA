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
  
  std::ofstream time_data;
  time_data.open("res/create.csv", std::ios::app);

  std::vector<double> times(3, 0);

  std::cout << "Building FMI\n"; 
  begin_time = std::chrono::high_resolution_clock::now();
  edaa::fm_index<> fmi(input); 
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - begin_time;
  times[0] = elapsed_time.count();
  std::string fmi_o = "cds/fmi_"+output+".sdsl";
  sdsl::store_to_file(fmi, fmi_o);
  time_data << "FMI,"<< output << "," << times[0]<<","<<sdsl::size_in_mega_bytes(fmi) << '\n';
  
  std::cout << "Building CSA\n"; 
  begin_time = std::chrono::high_resolution_clock::now();
  edaa::compressed_sa csa(input); 
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - begin_time;
  times[2] = elapsed_time.count();
  std::string csa_o = "cds/csa_"+output+".sdsl";
  sdsl::store_to_file(csa, csa_o);
  time_data << "CSA,"<< output << "," << times[2] <<sdsl::size_in_mega_bytes(csa) << '\n';

  if(output != "genome") {
    std::cout << "Building SA\n"; 
    begin_time = std::chrono::high_resolution_clock::now();
    edaa::suffix_array sa(input); 
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[1] = elapsed_time.count(); 
    std::string sa_o = "cds/sa_"+output+".sdsl";
    sdsl::store_to_file(sa, sa_o);
    time_data << "SA,"<< output << "," << times[1] <<","<<sdsl::size_in_mega_bytes(sa) << '\n';
  }
  time_data.close();


  std::cout << "Done\n\n";


  return 0;
}
