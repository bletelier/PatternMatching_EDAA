#ifndef FM_INDEX_HPP
#define FM_INDEX_HPP

#include <sdsl/suffix_arrays.hpp>

namespace edaa {
  template <uint32_t sa_sampling = 512, uint32_t isa_sampling = 1024>
  class fm_index {
    public:
      typedef uint64_t                                       t_size;
      typedef sdsl::rrr_vector<63>                           t_rv;
      typedef sdsl::wt_huff<t_rv>                            t_wth;
      typedef sdsl::csa_wt<t_wth, sa_sampling, isa_sampling> t_fmi;
      typedef std::string                                    t_str;
    protected:
      t_fmi     fmi;
    public:
      //Construye un FMI desde un texto en el folder input
      fm_index(t_str input_file, t_str cds_file) {
        t_str input = "input/" + input_file;
        sdsl::construct(fmi, input, 1);
        t_str cds = "cds/" + cds_file + ".sdsl";
        sdsl::store_to_file(fmi, cds);       
      }

      //Cargar un FMI existente en folder cds
      fm_index(t_str cds_file) {
        t_str cds = "cds/" + cds_file + ".sdsl";
        if(!sdsl::load_from_file(fmi, cds)) {
          std::cout << "ERROR: File " << cds << " does not exist. Exit.\n";
        }
      }

      //Retorna el tamaño en MB del FMI
      t_size size_in_mb() {
        return (t_size) size_in_mega_bytes(fmi);
      }

      t_size count(t_str pat) {
        return sdsl::count(fmi, pat.begin(), pat.end());
      }
  };
}

#endif //FM_INDEX_HPP
