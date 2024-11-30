#ifndef CSA_HPP
#define CSA_HPP

#include <sdsl/suffix_arrays.hpp>

namespace edaa {
  class compressed_sa {
    public:
      typedef uint64_t                                       size_type;
      typedef sdsl::csa_sada<>                               t_csa;
      typedef std::string                                    t_str;
    protected:
      t_csa     csa;
    public:
      compressed_sa() {} ;

      //Construye un CSA desde un texto en el folder input
      compressed_sa(t_str input_file) {
        t_str input = input_file;
        sdsl::construct(csa, input, 1);
      }

      //Retorna el tamaño en MB del FMI
      size_type size_in_mb() {
        return (size_type) size_in_mega_bytes(csa);
      }

      size_type count(t_str pat) {
        return sdsl::count(csa, pat.begin(), pat.end());
      }
      
      size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const {
        sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
        size_type written_bytes = 0;

        written_bytes += csa.serialize(out, child, "csa");
        
        sdsl::structure_tree::add_size(child, written_bytes);
        return written_bytes;
      }
      
      void load(std::istream& in) {
        csa.load(in);
      }
  };
}

#endif //FM_INDEX_HPP
