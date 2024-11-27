#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <iostream>
#include <sdsl/suffix_trees.hpp>
#include <fstream>

namespace edaa {
  template <bool has_lcp = false>
  class suffix_array {
    public:
      typedef uint64_t                size_type;
      typedef uint64_t                t_64;
      typedef sdsl::int_vector<>      t_iv;
      typedef sdsl::int_vector<8>     t_1v;
      typedef std::string             t_str;
    protected:
      t_1v   text;
      t_iv   sa;
      t_iv   lcp;
      size_type text_size;
    private:
      void calculate_lcp() {
        int n = sa.size();

        lcp = t_iv(n, 0);

        // An auxiliary array to store inverse of suffix array
        // elements. For example if suffixArr[0] is 5, the
        // invSuff[5] would store 0.  This is used to get next
        // suffix string from suffix array.
        t_iv invSuff(n, 0);

        // Fill values in invSuff[]
        for (t_64 i=0; i < n; i++)
          invSuff[sa[i]] = i;

        // Initialize length of previous LCP
        t_64 k = 0;

        // Process all suffixes one by one starting from
        // first suffix in txt[]
        for (t_64 i=0; i<n; i++)
        {
          /* If the current suffix is at n-1, then we don’t
             have next substring to consider. So lcp is not
             defined for this substring, we put zero. */
          if (invSuff[i] == n-1)
          {
            k = 0;
            continue;
          }

          /* j contains index of the next substring to
             be considered  to compare with the present
             substring, i.e., next string in suffix array */
          t_64 j = sa[invSuff[i]+1];

          // Directly start matching from k'th index as
          // at-least k-1 characters will match
          while (i+k<n && j+k<n && text[i+k]==text[j+k])
            k++;

          lcp[invSuff[i]] = k; // lcp for the present suffix.

          // Deleting the starting character from the string.
          if (k>0)
            k--;
        }
      }

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
        calculate_lcp();
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
