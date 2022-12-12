#include <iostream>
#include <string>
#include <iterator>
#include <istream>
#include <fstream>
#include <tuple>

using namespace std;

template<typename typeA, typename typeB>
ostream &operator<<(ostream &os, tuple<typeA, typeB> const& t) {
    return os << "(" << get<0>(t) << ", " << get<1>(t) << ")" << endl;
}

template<typename typeA, typename typeB>
class CSVParser {
public:
    struct line_iterator {
      using iterator_category = input_iterator_tag;
      using value_type = string;
      using difference_type = ptrdiff_t;
      using reference = const value_type&;
      using pointer = const value_type*;

      line_iterator(): file(nullptr) {}
      line_iterator(istream& in): file(&in) { ++*this; }

      reference operator*() const { return s; }
      pointer operator->() const { return &**this; }

      line_iterator& operator++() {
          if (!getline(*file, s)) {
              file = nullptr;
              //throw error
          }
        return *this;
      }

      line_iterator operator++(int) {
        auto copy(*this);
        ++*this;
        return copy;
      }

      friend bool operator==(const line_iterator& x, const line_iterator& y) {
        return x.file == y.file;
      }

      friend bool operator!=(const line_iterator& x, const line_iterator& y) {
        return !(x == y);
      }

     private:
      istream* file;
      string s;
    };
    
  CSVParser(std::istream& input_file): file(input_file) {}

  line_iterator begin() const { return line_iterator(file); }
  line_iterator end() const { return line_iterator(); }

 private:
  std::istream& file;
};


int main(int argc, const char * argv[]) {
    std::ifstream file("/Users/iliakateshov/Desktop/test_csv/test_csv/test_table.csv");
    
    CSVParser<int, string> parser(file);
    for (string rs : parser) {
        cout << rs << endl;
    }
    return 0;
}
