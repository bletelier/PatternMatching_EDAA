/** uhr: generic time performance tester
 * Author: LELE
 *
 * Things to set up:
 * 0. Includes: include all files to be tested,
 * 1. Time unit: in elapsed_time,
 * 2. What to write on time_data,
 * 3. Data type and distribution of RNG,
 * 4. Additive or multiplicative stepping,
 * 5. The experiments: in outer for loop. */

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

std::string const genome = "genome";

#include<csa/csa.hpp>
#include<suffix_array/suffix_array.hpp>
#include<fm_index/fm_index.hpp>
// Include to be tested files here

inline void validate_input(int argc, char *argv[], std::int64_t& runs,
    std::int64_t& lower, std::int64_t& upper, std::int64_t& step)
{
    if (argc != 8) {
        std::cerr << "Usage: <filename> <RUNS> <LOWER> <UPPER> <STEP>" << std::endl;
        std::cerr << "<filename> is the name of the file where performance data will be written." << std::endl;
        std::cerr << "It is recommended for <filename> to have .csv extension and it should not previously exist." << std::endl;
        std::cerr << "<RUNS>: numbers of runs per test case: should be >= 32." << std::endl;
        std::cerr << "<LOWER> <UPPER> <STEP>: range of test cases." << std::endl;
        std::cerr << "These should all be positive." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Read command line arguments
    try {
        runs = std::stoll(argv[2]);
        lower = std::stoll(argv[3]);
        upper = std::stoll(argv[4]);
        step = std::stoll(argv[5]);
    } catch (std::invalid_argument const& ex) {
        std::cerr << "std::invalid_argument::what(): " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (std::out_of_range const& ex) {
        std::cerr << "std::out_of_range::what(): " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Validate arguments
    if (runs < 4) {
        std::cerr << "<RUNS> must be at least 4." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (step <= 0 or lower <= 0 or upper <= 0) {
        std::cerr << "<STEP>, <LOWER> and <UPPER> have to be positive." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (lower > upper) {
        std::cerr << "<LOWER> must be at most equal to <UPPER>." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

inline void display_progress(std::int64_t u, std::int64_t v)
{
    const double progress = u / double(v);
    const std::int64_t width = 70;
    const std::int64_t p = width * progress;
    std::int64_t i;

    std::cout << "\033[1m[";
    for (i = 0; i < width; i++) {
        if (i < p)
            std::cout << "=";
        else if (i == p)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << std::int64_t(progress * 100.0) << "%\r\033[0m";
    std::cout.flush();
}

inline void quartiles(std::vector<double>& data, std::vector<double>& q)
{
    q.resize(5);
    std::size_t n = data.size();
    std::size_t p;

    std::sort(data.begin(), data.end());

    if (n < 4) {
        std::cerr << "quartiles needs at least 4 data points." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Get min and max
    q[0] = data.front();
    q[4] = data.back();

    // Find median
    if (n % 2 == 1) {
        q[2] = data[n / 2];
    } else {
        p = n / 2;
        q[2] = (data[p - 1] + data[p]) / 2.0;
    }

    // Find lower and upper quartiles
    if (n % 4 >= 2) {
        q[1] = data[n / 4];
        q[3] = data[(3 * n) / 4];
    } else {
        p = n / 4;
        q[1] = 0.25 * data[p - 1] + 0.75 * data[p];
        p = (3 * n) / 4;
        q[3] = 0.75 * data[p - 1] + 0.25 * data[p];
    }
}


int main(int argc, char *argv[])
{
    // Validate and sanitize input
    std::int64_t runs, lower, upper, step;
    validate_input(argc, argv, runs, lower, upper, step);

    std::string input_text = argv[6];
    sdsl::int_vector<8> text;
    sdsl::load_vector_from_file(text, input_text, 1); //para obtener una lista de patrones a buscar.
    if (text.size()==0 or text[text.size()-1] != 0) {
      text.resize(text.size()+1);
      text[text.size()-1] = 0;
    }
    std::string shortname = argv[7];
    edaa::suffix_array sa;
    edaa::fm_index<> fmi;
    edaa::compressed_sa csa;
    std::string input = "cds/fmi_"+ shortname+".sdsl";
    std::ifstream input_file(input);
    fmi.load(input_file);

    input = "cds/csa_"+shortname+".sdsl";
    input_file = std::ifstream(input);
    csa.load(input_file);

    input = "cds/sa_"+shortname+".sdsl";
    input_file = std::ifstream(input);
    if(shortname != genome) 
      sa.load(input_file);

    // Set up clock variables
    std::int64_t n, i, executed_runs;
    std::int64_t total_runs_additive = runs * (((upper - lower) / step) + 1);
    std::int64_t total_runs_multiplicative = runs * (floor(log(upper / double(lower)) / log(step)) + 1);
    std::vector<std::vector<double>> times(3, std::vector<double>(runs));
    std::vector<double> q;
    auto begin_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time = end_time - begin_time;

    // Set up random number generation
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<std::int64_t> u_distr; // change depending on app

    // File to write time data
    std::ofstream time_data;
    std::string filename = argv[1];
    time_data.open("res/" + filename);
    time_data << "alg,n,t_mean,t_stdev,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4" << std::endl;

    uint64_t fixed_n = text.size();
    // Begin testing
    std::cout << "\033[0;36mRunning tests...\033[0m" << std::endl << std::endl;
    executed_runs = 0;
    for (n = lower; n <= upper; n += step) {
        std::vector<double> mean_time(3, 0);
        std::vector<double> dev(3, 0);
        std::vector<double> time_stdev(3, 0);
 

        uint64_t r1, r2, r3;

        int ki = 1 << 7;
        std::vector<std::string> keys(ki);
        // Run to compute elapsed time
        for (i = 0; i < runs; i++) {
            for(uint64_t k = 0; k < ki; ++k) {
              uint64_t id = u_distr(rng) % (fixed_n - n - 1);
              keys[k] = std::string(n, '_');
              for(uint64_t j = 0; j < n; ++j){
                keys[k][j] = text[id + j];
              } 
            }
        
            // Remember to change total depending on step type
            display_progress(++executed_runs, total_runs_additive);
            
            begin_time = std::chrono::high_resolution_clock::now();
            // Function to test goes here
            for(int k = 0; k < ki; ++k)
              r1 = csa.count(keys[k]);
            end_time = std::chrono::high_resolution_clock::now();
            elapsed_time = end_time - begin_time;
            times[0][i] = elapsed_time.count()/ki;
            mean_time[0] += times[0][i];

            begin_time = std::chrono::high_resolution_clock::now();
            // Function to test goes here
            for(int k = 0; k < ki; ++k)
              r2 = fmi.count(keys[k]);
            end_time = std::chrono::high_resolution_clock::now();
            elapsed_time = end_time - begin_time;
            times[1][i] = elapsed_time.count()/ki;
            mean_time[1] += times[1][i];

            if(shortname != genome) {
              begin_time = std::chrono::high_resolution_clock::now();
              // Function to test goes here
              for(int k = 0; k < ki; ++k)
                r3 = sa.count_without(keys[k]);
              end_time = std::chrono::high_resolution_clock::now();
              elapsed_time = end_time - begin_time;
              times[2][i] = elapsed_time.count()/ki;
              mean_time[2] += times[2][i];
            }

        }

        // Compute statistics
        mean_time[0] /= runs;
        mean_time[1] /= runs;
        if(shortname != genome)
          mean_time[2] /= runs;

        for (i = 0; i < runs; i++) {
            dev[0] = times[0][i] - mean_time[0];
            dev[1] = times[1][i] - mean_time[1];
            time_stdev[0] += dev[0] * dev[0];
            time_stdev[1] += dev[1] * dev[1];
            if(shortname != genome) {
              dev[2] = times[2][i] - mean_time[2];
              time_stdev[2] += dev[2] * dev[2];
            }
        }

        time_stdev[0] /= runs - 1; // Subtract 1 to get unbiased estimator
        time_stdev[0] = std::sqrt(time_stdev[0]);
        time_stdev[1] /= runs - 1; // Subtract 1 to get unbiased estimator
        time_stdev[1] = std::sqrt(time_stdev[1]);
        if(shortname != genome) {
          time_stdev[2] /= runs - 1; // Subtract 1 to get unbiased estimator
          time_stdev[2] = std::sqrt(time_stdev[2]);
        }

        quartiles(times[0], q);
        time_data << "csa,"<< n << "," << mean_time[0] << "," << time_stdev[0] << ",";
        time_data << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;
        
        quartiles(times[1], q);
        time_data << "fmi,"<< n << "," << mean_time[1] << "," << time_stdev[1] << ",";
        time_data << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;
        
        if(shortname != genome) {
          quartiles(times[2], q);
          time_data << "sa,"<< n << "," << mean_time[2] << "," << time_stdev[2] << ",";
          time_data << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;
        }
    }

    // This is to keep loading bar after testing
    std::cout << std::endl << std::endl;
    std::cout << "\033[1;32mDone!\033[0m" << std::endl;

    time_data.close();

    return 0;
}
