#ifndef FM_INDEX_HPP
#define FM_INDEX_HPP

#include <sdsl/suffix_arrays.hpp>

namespace edaa {
  template <uint32_t sa_sampling = 512, uint32_t isa_sampling = 1024>
  class fm_index {
    public:
      typedef uint64_t                                       size_type;
      typedef sdsl::rrr_vector<63>                           t_rv;
      typedef sdsl::wt_huff<t_rv>                            t_wth;
      typedef sdsl::csa_wt<t_wth, sa_sampling, isa_sampling> t_fmi;
      typedef std::string                                    t_str;
    protected:
      t_fmi     fmi;
    public:
      fm_index() {} ;

      //Construye un FMI desde un texto en el folder input
      fm_index(t_str input_file) {
        t_str input = "input/" + input_file;
        sdsl::construct(fmi, input, 1);
      }

      //Retorna el tamaño en MB del FMI
      size_type size_in_mb() {
        return (size_type) size_in_mega_bytes(fmi);
      }

      size_type count(t_str pat) {
        return sdsl::count(fmi, pat.begin(), pat.end());
      }
      
      size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const {
        sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
        size_type written_bytes = 0;

        written_bytes += fmi.serialize(out, child, "fmi");
        
        sdsl::structure_tree::add_size(child, written_bytes);
        return written_bytes;
      }
      
      void load(std::istream& in) {
        fmi.load(in);
      }
  };
}

#endif //FM_INDEX_HPP
