#include <iostream>
#include <string>
#include <fstream>
#include <tuple>
#include <sstream>

using namespace std;

template <typename T>                           // data formatting
T format(string element) { return NULL; }

template <>
int format(string element) { return stoi(element); }

template <>
float format(string element) { return stof(element); }

template <>
double format(string element) { return stod(element); }

template <>
bool format(string element) { return (element != "0"); }

template <>
string format(string element) { return element; }

template <typename TupleT, typename Fn>             // iterate through tuple
void for_each(TupleT&& tp, Fn&& fn) {
    apply([&fn]<typename ...T>(T&& ...args) {(fn(forward<T>(args)), ...);}, forward<TupleT>(tp)); // Explicit template parameter list for lambdas is a C++20 extension
}

template<typename... Args>                          // tuple output
ostream& operator<<(ostream& os, tuple<Args...> const& t) {
  bool first = true;
  apply([&](auto&&... args){ ((os << (first ? "" : ", ") << args, first = false), ...); }, t);
  return os;
}

template<class... Types>
class CSVParser {
public:
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
            if (!getline(*file, s)) { file = nullptr; }
            if (!s.empty()) {s.erase(s.length()-1);}
            line_num++;
        
            stringstream line(s); string element; int row;
            for_each(output, [&](auto&& x){
                if (getline(line, element, ';')) {
                    row++;
                    auto buf = x;
                    try { x = format<decltype(buf)>(element); }
                    catch (const std::invalid_argument& e) { cout << "Exc bad type in row: " << row << " line:" << line_num << endl; }
                }
            });
            return *this;
        }

        line_iterator operator++(int) { auto copy(*this); ++*this; return copy; }
        friend bool operator==(const line_iterator& x, const line_iterator& y) { return x.file == y.file; }
        friend bool operator!=(const line_iterator& x, const line_iterator& y) { return !(x == y); }
        
    private:
        istream* file;
        string s;
        tuple<Types...> output;
        int line_num = 0;
    };
    
  CSVParser(istream& input_file): file(input_file) {}

  line_iterator begin() const { return line_iterator(file); }
  line_iterator end() const { return line_iterator(); }

 private:
  std::istream& file;
};

int main(int argc, const char * argv[]) {
    ifstream file("/Users/iliakateshov/Desktop/test_csv/test_csv/test_table1.csv");
    CSVParser<int, string> parser(file);
    for (tuple<int, string> rs : parser) {
        cout << rs << endl;
    }
    cout << endl;
    file.close();
    return 0;
}
