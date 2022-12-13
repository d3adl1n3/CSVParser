#include <iostream>
#include <string>
#include <iterator>
#include <istream>
#include <fstream>
#include <tuple>
#include <vector>
#include <sstream>

using namespace std;

template<typename typeA, typename typeB>
ostream &operator<<(ostream &os, tuple<typeA, typeB> const& t) {
    return os << "(" << get<0>(t) << ", " << get<1>(t) << ")" << endl;
}

// class... Types
template<typename typeA, typename typeB>
class CSVParser {
public:
    struct line_iterator {
        using iterator_category = input_iterator_tag;
        using value_type = tuple<typeA, typeB>;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = const value_type*;

        line_iterator(): file(nullptr) {}
        line_iterator(istream& in): file(&in) { ++*this; }

        reference operator*() const { return output; }
        pointer operator->() const { return &**this; }

        line_iterator& operator++() {
            if (!getline(*file, s)) {
                file = nullptr;
                //throw error
            }
            stringstream line(s);
            string element;
            int k = 0;
            while (getline(line, element, ';')) {
                if (k == 0) {
                    get<0>(output) = stoi(element);
                } else if (k == 1) {
                    get<1>(output) = (typeB) element;
                }
                k++;
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
        vector<string> arr;
        tuple<typeA, typeB> output;
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
    for (tuple<int, string> rs : parser) {
        cout << rs << endl;
    }
    return 0;
}

