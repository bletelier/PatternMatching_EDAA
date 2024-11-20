#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <iostream>
#include <sdsl/suffix_trees.hpp>
#include <fstream>

namespace edaa {
  template <bool has_lcp = false>
  class suffix_array {
    public:
      typedef uint64_t                      size_type;
      typedef sdsl::int_vector<>      t_iv;
      typedef sdsl::int_vector<8>     t_1v;
      typedef std::string             t_str;
    protected:
      t_1v   text;
      t_iv   sa;
      t_iv   lcp;
      size_type text_size;
    private:
      void calculate_lcp() {};

    public:
      suffix_array() {};

      suffix_array(t_str input_file) {
        t_str input = "input/" + input_file;
        sdsl::load_vector_from_file(text, input, 1);
        if (text.size()==0 or text[text.size()-1] != 0) {
          text.resize(text.size()+1);
          text[text.size()-1] = 0;
        }
        sdsl::qsufsort::construct_sa(sa, text);
        text_size = (size_type) text.size();
        if(has_lcp) calculate_lcp();
      }

      void print_ids() {
        for(int i = 0; i < 6; ++i)
          std::cout << sa[i] << ' ';
        std::cout << '\n';
      }

      size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const {
        sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
        size_type written_bytes = 0;

        written_bytes += text.serialize(out, child, "text");
        written_bytes += sa.serialize(out, child, "sa");
        written_bytes += lcp.serialize(out, child, "lcp");
        written_bytes += sdsl::write_member(text_size, out, child, "text_size");
        
        sdsl::structure_tree::add_size(child, written_bytes);
        return written_bytes;
      }

      void load(std::istream& in) {
        text.load(in);
        sa.load(in);
        lcp.load(in);
        sdsl::read_member(text_size, in);
      }
      
  };
}

#endif //SUFFIX_ARRAY_HPP
