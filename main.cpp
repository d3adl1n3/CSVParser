#include <iostream>
#include <string>
#include <istream>
#include <fstream>
#include <tuple>
#include <vector>
#include <sstream>

using namespace std;

/*data formatting*/
template <typename T>
T format(string element) {
    throw invalid_argument("type unsupported");
    return NULL;
}

template <>
int format(string element) {
    try { return stoi(element); }
    catch (...) { throw invalid_argument("type unsupported"); return 0; }
}

template <>
float format(string element) {
    try { return stof(element); }
    catch (...) { throw invalid_argument("type unsupported"); return 0;}
}

template <>
double format(string element) {
    try { return stod(element); }
    catch (...) { throw invalid_argument("type unsupported"); return 0;}
}

template <>
long double format(string element) {
    try { return stold(element); }
    catch (...) { throw invalid_argument("type unsupported"); return 0;}
}

template <>
bool format(string element) {
    try { return (element != "0"); }
    catch (...) { throw invalid_argument("type unsupported"); return 0;}
}

template <>
string format(string element) { return element; }

/* iterate through tuple, set elements from line stream */
template <size_t I = 0, typename... Ts>
typename enable_if<I == sizeof...(Ts), void>::type
parseString(tuple<Ts...> &tup, stringstream& line) { return; }

template <size_t I = 0, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), void>::type
parseString(tuple<Ts...> &tup, stringstream& line) {
    string element;
    if (getline(line, element, ';')) {
        auto buf = get<I>(tup);
        try { get<I>(tup) = format<decltype(buf)>(element); }
        catch (const std::invalid_argument& e) {
            throw string("An exception occurred bad type in line: " + to_string(I+1));
        }
    }
    parseString<I + 1>(tup, line);
}

/* tuple output */
template<typename... Args>
ostream& operator<<(ostream& os, tuple<Args...> const& t) {
  bool first = true;
  apply([&](auto&&... args){ ((os << (first ? "" : ", ") << args, first = false), ...); }, t);
  return os;
}

/* main parser class */
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
        
            stringstream line(s);
            try { parseString(output, line); }
            catch (const string& e) { cout << e << " row: " << line_num << endl; }
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
    cout << "---- First test file <int, string> ----" << endl;
    ifstream file("/Users/iliakateshov/Desktop/test_csv/test_csv/test_table1.csv");
    CSVParser<int, string> parser(file);
    for (tuple<int, string> rs : parser) {
        cout << rs << endl;
    }
    cout << endl;
    file.close();
    
    cout << "---- Second test file <int, float, bool> ----" << endl;
    ifstream file2("/Users/iliakateshov/Desktop/test_csv/test_csv/test_table2.csv");
    CSVParser<int, float, bool> parser2(file2);
    for (tuple<int, float, bool> rs : parser2) {
        cout << rs << endl;
    }
    cout << endl;
    file2.close();
    
    return 0;
}
