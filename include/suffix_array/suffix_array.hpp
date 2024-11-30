#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <iostream>
#include <sdsl/suffix_trees.hpp>
#include <sdsl/rmq_support.hpp>
#include <fstream>

namespace edaa {
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
      sdsl::rmq_succinct_sct<> rmq;
    private:
      
    public:
      suffix_array() {};

      suffix_array(t_str input_file) {
        t_str input = input_file;
        sdsl::load_vector_from_file(text, input, 1);
        if (text.size()==0 or text[text.size()-1] != 0) {
          text.resize(text.size()+1);
          text[text.size()-1] = 0;
        }
        sdsl::qsufsort::construct_sa(sa, text);
        text_size = (size_type) text.size();
        //calculate_lcp();
      }
           
      void print_ids() {
        for(int i = 0; i < 6; ++i)
          std::cout << sa[i] << ' ';
        std::cout << '\n';
      }

      void compute_lcp() {
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
        rmq = sdsl::rmq_succinct_sct<>(&lcp);
      }



      //pido perdon a todos los que creyeron en mi, me rindo. No pude hacer que
      //la funcion encuentre el upper_bound, probe de miles de formas y todas
      //las veces me encuentra el lower_bound. BLete.
      //PD: no se si esta función este funcional, llevo una semana implementandolo
      //y lo deje tirado luego de miles de pruebas asi que no se en que quedó.
      //Basicamente buscaba hacer la técnica de max-skipping usando el LCE 
      //(Longest Common Extension) entre sufijos del texto y el LCP que ibamos
      //descubriendo entre el text[sa[m]] con P (un poco la idea de min-skipping).
      //La cosa es que funcionó para encontrar el lower_bound, pero el upper_bound
      //me fue imposible. Aplique la idea propuesta en:
      //https://courses.csail.mit.edu/6.851/spring07/psets/ps4-sol.pdf y otros sitios
      //que dice que el LCE(i,j) = min(LCP[i], LCP[i+1],...,LCP[j-1]) con una rmq
      //+ la idea de min-skipping de ir encontrando el LCP entre el patrón y texto
      //a medida que avanzamos sobre la BS.
      t_64 count_with(t_64 pos, t_64 len) {
        t_64 l = 1, r = text_size - 1;
        t_64 lb = text_size;
        t_64 mcp = 0, lce, mid, path = 1,  p_id;
        bool match;
        lce = lcp[rmq(l,r)];
        t_64 c =0;
        while(l < r) {
          mid = l + (r-l)/2;
          if(mcp < lce and l < r) {
            if(path == 1) {
              r = mid - 1;
              t_64 mid2 = l + (r-l)/2;
              lce = lcp[rmq(mid2,r)];
            }
            else {
              l = mid + 1;
              t_64 mid2 = l + (r-l)/2;
              lce = lcp[rmq(l, mid2)];
            }
          }
          else if(mcp > lce and l < r) {
            if(path == 1) {
              l = mid + 1;
              t_64 mid2 = l + (r-l)/2;
              lce = lcp[rmq(l, mid2)];
            }
            else {
              r = mid - 1;
              t_64 mid2 = l + (r-l)/2;
              lce = lcp[rmq(mid2, r)];
            }
          }
          else {
            match = true;
            for(p_id = mcp; p_id < len; ++p_id) {
              if(text[sa[mid] + p_id] > text[pos + p_id]) {
                match = false;
                path = 1;
                mcp = p_id;
                r = mid ;
                lce = lcp[rmq(l, r)];
                break;
              }
              else if(text[sa[mid] + p_id] < text[pos + p_id]) {
                match = false;
                mcp = p_id;
                path = 2;
                l = mid + 1;
                lce = lcp[rmq(l, r)];
                break;
              }
            }
            if(match) {
              lb = mid;
              l = mid + 1; 
              if(l > r) break;
              mcp = 0;
              lce = lcp[rmq(l, r)];
              path = 1;
            }

          }
        }
        return lb;
      }

      t_64 count_without(t_str pat) {
        t_64 l = 1, r = text_size - 1; 
        t_64 lb = text_size;
        t_64 len = (t_64) pat.size();
        while(l <= r) {
          t_64 mid = l + (r-l)/2;
          bool match = true;
          for(t_64 p_id = 0; p_id < len; ++p_id) {
            if(text[sa[mid] + p_id] > pat[p_id]) {
              match = false;
              r = mid - 1;
              break;
            }
            else if(text[sa[mid] + p_id] < pat[p_id]) {
              match = false;
              l = mid + 1;
              break;
            }
          }
          if(match) {
            lb = mid;
            r = mid - 1; 
          }
        }
        if(lb == text_size) return 0;

        l = lb, r = text_size - 1; 
        t_64 rb = text_size;
        while(l <= r) {
          t_64 mid = l + (r-l)/2;
          bool match = true;
          for(t_64 p_id = 0; p_id < len; ++p_id) {
            if(text[sa[mid] + p_id] > pat[p_id]) {
              match = false;
              r = mid - 1;
              break;
            }
            else if(text[sa[mid] + p_id] < pat[p_id]) {
              match = false;
              l = mid + 1;
              break;
            }
          }
          if(match) {
            rb = mid;
            l = mid + 1; 
          }
        }
        return rb - lb + 1;
      }


      size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const {
        sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
        size_type written_bytes = 0;

        written_bytes += text.serialize(out, child, "text");
        written_bytes += sa.serialize(out, child, "sa");
        written_bytes += sdsl::write_member(text_size, out, child, "text_size");


        written_bytes += lcp.serialize(out, child, "lcp");//no tiene nada
        written_bytes += rmq.serialize(out, child, "rmq");//no tiene nada

        sdsl::structure_tree::add_size(child, written_bytes);
        return written_bytes;
      }

      void load(std::istream& in) {
        text.load(in);
        sa.load(in);
        sdsl::read_member(text_size, in);
        lcp.load(in);//nada
        rmq.load(in);//nada
      }
      
  };
}

#endif //SUFFIX_ARRAY_HPP
