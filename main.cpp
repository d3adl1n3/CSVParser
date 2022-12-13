#include <iostream>
#include <string>
#include <iterator>
#include <istream>
#include <fstream>
#include <tuple>
#include <vector>
#include <sstream>

using namespace std;

template <typename TupleT, size_t... Is>
std::ostream& printTupleImp(ostream& os, const TupleT& tp, index_sequence<Is...>) {
    size_t index = 0;
    auto printElem = [&index, &os](const auto& x) {
        if (index++ > 0)
            os << ", ";
        os << x;
    };

    os << "(";
    (printElem(get<Is>(tp)), ...);
    os << ")";
    return os;
}

template <typename TupleT, size_t TupSize = tuple_size<TupleT>::value>
std::ostream& operator <<(ostream& os, const TupleT& tp) {
    return printTupleImp(os, tp, make_index_sequence<TupSize>{});
}

template<class... Types>
class CSVParser {
public:
    struct args { using arr = vector<Types...>; };
    
    struct line_iterator {
        using iterator_category = input_iterator_tag;
        using value_type = tuple<Types...>;
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
                    get<1>(output) = element;
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
        tuple<Types...> output;
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

