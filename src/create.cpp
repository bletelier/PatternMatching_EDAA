#include <iostream>
#include <fm_index/fm_index.hpp>
#include <suffix_array/suffix_array.hpp>
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
  for(int i = 1; i < 5; ++i) {
    begin_time = std::chrono::high_resolution_clock::now();
    fmi = edaa::fm_index<>(input);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[0] = std::min(times[0], elapsed_time.count());
  } 

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
  for(int i = 1; i < 5; ++i) {
    begin_time = std::chrono::high_resolution_clock::now();
    sa = edaa::suffix_array(input);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[1] = std::min(times[1], elapsed_time.count());
  }

  std::cout << "Building LCP\n"; 
  begin_time = std::chrono::high_resolution_clock::now();
  sa.compute_lcp(); 
  end_time = std::chrono::high_resolution_clock::now();
  elapsed_time = end_time - begin_time;
  times[2] = elapsed_time.count();
  for(int i = 1; i < 5; ++i) {
    begin_time = std::chrono::high_resolution_clock::now();
    sa.compute_lcp();
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = end_time - begin_time;
    times[2] = std::min(times[2], elapsed_time.count());
  }

  std::string sa_o = "cds/sa_"+output+".sdsl";
  sdsl::store_to_file(sa, sa_o);

  std::ofstream time_data;
  time_data.open("res/create.csv", std::ios::app);
  time_data << "FMI,"<< output << "," << times[0] << '\n';
  time_data << "SA,"<< output << "," << times[1] << '\n';
  time_data << "SA+LCP,"<< output << "," << times[1]+times[2] << '\n';
  time_data.close();

  std::cout << "Done\n\n";
  /*edaa::suffix_array<> saa;
  std::string input = "cds/sa.sdsl";
  std::ifstream input_file(input);
  saa.load(input_file);

  sa.print_ids();
  saa.print_ids();*/
  return 0;
}
