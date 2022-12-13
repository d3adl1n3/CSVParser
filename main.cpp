#include <iostream>
#include <string>
#include <iterator>
#include <istream>
#include <fstream>
#include <tuple>
#include <vector>
#include <sstream>
#include <type_traits>

using namespace std;

template <typename T>
T format(string element) {
    return NULL;
    // throw error -- type unsupported
}

template <>
int format(string element) {
    return stoi(element);
}

template <>
string format(string element) {
    return element;
}

/* iterate through tuple, set elements from line stream */
template <size_t I = 0, typename... Ts>
typename enable_if<I == sizeof...(Ts), void>::type
parseString(tuple<Ts...> &tup, stringstream& line) { return; }

template <size_t I = 0, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), void>::type
parseString(tuple<Ts...> &tup, stringstream& line) {
    struct args { using arr = vector<Ts...>; };
    string element;
    if (!getline(line, element, ';')) {
        /* throw error (no enough colums in table)*/
    } else {
        auto buf = get<I>(tup);
        get<I>(tup) = format<decltype(buf)>(element);
    }
    parseString<I + 1>(tup, line);
}

/* tuple output */
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

/* main parser class */
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
                //throw error no enough rows in table
            }
            stringstream line(s);
            parseString(output, line);
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
    
  CSVParser(istream& input_file): file(input_file) {}

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
