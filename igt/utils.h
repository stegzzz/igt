#pragma once
#ifdef USING_DLL
#define DECLSPEC __declspec(dllimport)
#else
#ifdef LINUX
#define DECLSPEC __attribute__ ((visibility ("default")))
#else
#ifdef STDCPP
#define DECLSPEC
#else
#define DECLSPEC __declspec(dllexport)
#endif
#endif
#endif
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <random>
#include <regex>
#include <sstream> // string to number conversion
#include <string>
#include <type_traits>
#include <vector>

namespace spgxyz {

#define RTE                                                                    \
  auto err = [func = __func__](std::string msg) {                              \
    throw std::runtime_error(std::string{} + msg + "\tin: " + func);           \
  }
DECLSPEC extern const size_t veryTinyBuff; // = 8;
DECLSPEC extern const size_t tinyBuff;     //= 64;
DECLSPEC extern const size_t smallBuff;    // = 256;
DECLSPEC extern const size_t largeBuff;    // = 1024;
DECLSPEC extern const size_t hugeBuff;     //= 4096;
/**
 * @file utils.h
 * @author Steven Glautier
 * @copyright Steven Glautier 1998-2022
 * @section Licence
 * Creative Commons Attribution 4.0 International Public License
 * every effort is made to ensure this software works correctly but absolutely
 * no guarantees are made please report errors and contact Steven Glautier in
 * case of difficulty spgxyz@gmail.com
 * @section Description
 */

/**
\mainpage Summary for utils project

\section utils_main1 General utilities
A collection of useful functions and classes. Portable, only use std
*/

/**
 * @brief split string s using regex returning vector of string tokens
 * auto words=splitStr(s, "\\s+") // split on whitespace
 *
 * @param const std::string& s
 * @param std::string regex
 * @return std::vector<std::string>
 *
 */
DECLSPEC std::vector<std::string> splitStr(const std::string &s,
                                           const std::string &regex);

/**
 * @brief filter vector of strings returning those which match regx
 *
 * @param std::vector<std::string> &in - to be filtered
 * @param std::string &regx - filter
 * @return std::vector<std::string> - filtered strings
 */
DECLSPEC std::vector<std::string>
filterStrings(std::vector<std::string> const &in, std::string const &regx);

/**
 * @brief read file f returning vector of lines as strings
 *
 * @param const std::string& s
 * @exception std::runtime_error
 * @return std::vector<std::string>
 */
DECLSPEC std::vector<std::string> readFile(const std::string &f);

/**
 * @brief read file f, lines [s to e], returning vector of lines as strings
 *
 * @param const std::string& s
 * @param int s - first line to read
 * @param int e - last line to read
 * @exception std::runtime_error, std::out_of_range
 * @return std::vector<std::string>
 */
DECLSPEC std::vector<std::string> readFile(const std::string &f, int s, int e);

/**
 * @brief templated functor for reading numeric files row by row doubles or
 * integers catered for
 *
 * @see utilsTests.cpp for usage example
 *
 * spgxyz::ReadFile<double> rdr;
 * auto buff = rdr(dfile);
 */
template <typename T> struct ReadFile {
  std::vector<std::vector<T>> operator()(const std::string &f) {
    auto lines = readFile(f);
    std::vector<std::vector<T>> result;
    for (auto line : lines) {
      std::vector<T> nline;
      auto fields = splitStr(line, "\\s+");
      for (auto fld : fields) {
        try {
          if (std::is_integral<T>())
            nline.push_back(std::stol(fld));
          if (std::is_floating_point<T>())
            nline.push_back(std::stod(fld));
        } catch (std::invalid_argument) {
          nline.push_back(NAN);
        }
      }
      result.push_back(nline);
    }
    return result;
  }
}; // struct ReadFile{

/**
 * @brief templated functor for reading numeric files row by row doubles or
 * integers catered for, choose line range to read
 *
 * @see readFile(const std::string &f, int s, int e)
 *
 * spgxyz::ReadFile<double> rdr;
 * auto buff = rdr(dfile,10,20);
 */
template <typename T> struct ReadFileLines {
  std::vector<std::vector<T>> operator()(const std::string &f, int s, int e) {
    auto lines = readFile(f, s, e);
    std::vector<std::vector<T>> result;
    for (auto line : lines) {
      std::vector<T> nline;
      auto fields = splitStr(line, "\\s+");
      for (auto fld : fields) {
        try {
          if (std::is_integral<T>())
            nline.push_back(std::stol(fld));
          if (std::is_floating_point<T>())
            nline.push_back(std::stod(fld));
        } catch (std::invalid_argument) {
          nline.push_back(NAN);
        }
      }
      result.push_back(nline);
    }
    return result;
  }
}; // struct ReadFile{

/**
 * @brief write to file truncating file
 * @see void writef(const std::string &f, std::vector<T> &v, std::ios::openmode
 * m)
 */
template <typename T> void writef(const std::string &f, std::vector<T> &v) {
  writef(f, v, std::ios_base::trunc);
}

/**
 * @brief write vector of type T supporting << to file f, as single field,
 * return after each item
 *
 * @param const std::string &f
 * @param std::vector<T> &v
 * @param std::ios::openmode m
 *
 */
template <typename T>
void writef(const std::string &f, std::vector<T> &v, std::ios::openmode m) {
  std::ofstream ouf;
  ouf.open(f, m);
  for (auto d : v)
    ouf << d << std::endl;
  ouf.close();
}

/**
 * @brief write to file truncating file
 * @see const std::string &f, const std::vector<T> &v, const int dp, const int
 * w, std::ios_base::openmode m
 *
 */
template <typename T>
void writef(const std::string &f, const std::vector<T> &v, const int dp,
            const int w) {
  writef(f, v, dp, w, std::ios_base::trunc);
}

/**
 * @brief write vector of type T to file with dp decimal places and with a
 * return after w fields written
 *
 * @param const std::string& f
 * @param const std::vector<T>& v
 * @param const int dp
 * @param const int w
 * @param std::ios_base::openmode m
 * @return void
 *
 */
template <typename T>
void writef(const std::string &f, const std::vector<T> &v, const int dp,
            const int w, std::ios_base::openmode m) {
  std::ofstream ouf;
  ouf.open(f, m);
  ouf.precision(dp);
  int len;
  {
    const auto [min, max] = std::minmax_element(v.begin(), v.end());
#ifdef LINUX
    auto absMax = std::ceil(*max);//apparently there is a bugi https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79700
    auto absMin = std::ceil(*min);
#else
    auto absMax = std::ceilf(*max);
    auto absMin = std::ceilf(*min);
#endif
    if (absMin > absMax)
      absMax = absMin;
    len = std::to_string(static_cast<int>(absMax)).length() + dp +
          2; //+2 allows for a minus sign and point
  }
  int i = 0;
  auto it = v.begin();
  auto end = v.end();
  while (it != end) {
    ouf << std::right << std::setw(len) << std::fixed << *it;
    if ((++it) == end)
      break;
    if ((++i) == w) {
      ouf << std::endl;
      i = 0;
    } else
      ouf << "\t";
  }

  ouf.close();
}

/**
 * @brief print a tab separated line of values in v, for floating point types
 * the number of decimal places can be specified, default=3. all T must support
 * <<
 *
 * @param std::vector<T> const &v - values to print
 * @param int const dp = 3 - decimal places for floating point
 */
template <typename T>
void printVectorLine(std::vector<T> const &v, int const dp = 3) {
  auto sz = v.size();
  if (std::is_floating_point<T>::value) {
    for (int i = 0; i < (sz - 1); ++i)
      std::cout << std::setprecision(dp) << v[i] << "\t";
    std::cout << std::setprecision(dp) << v[sz - 1] << "\n";

  } else {
    for (int i = 0; i < (sz - 1); ++i)
      std::cout << v[i] << "\t";
    std::cout << v[sz - 1] << "\n";
  }
}

/**
 * @brief print a tab separated line of values pointed to by pointers in v,
 * for floating point types the number of decimal places can be specified,
 * default=3. all T must support <<
 *
 * @param std::vector<T*> const &v - pointers to values to print
 * @param int const dp = 3 - decimal places for floating point
 */
template <typename T>
void printVectorLine(std::vector<T *> const &v, int const dp = 3) {
  auto sz = v.size();
  if (std::is_floating_point<T *>::value) {
    for (int i = 0; i < (sz - 1); ++i)
      std::cout << std::setprecision(dp) << *v[i] << "\t";
    std::cout << std::setprecision(dp) << *v[sz - 1] << "\n";

  } else {
    for (int i = 0; i < (sz - 1); ++i)
      std::cout << *v[i] << "\t";
    std::cout << *v[sz - 1] << "\n";
  }
}

/**
 * @brief vector of strings from environment path
 * @return std::vector<std::string>
 *
 */
DECLSPEC std::vector<std::string> getPath();

/**
 * @brief Get the directories in directory
 *
 * @param dir
 * @return DECLSPEC
 */
DECLSPEC std::vector<std::string>
get_directories(std::filesystem::path const &dir);

/**
 * @brief get list of files in dir
 *
 * @param std::filesystem::path const & dir
 * @return std::vector<std::string>
 */
DECLSPEC std::vector<std::string>
visit_directory(std::filesystem::path const &dir);

/**
 * @brief get list of files (+/- path) in dir
 *
 * @param std::filesystem::path const & dir
 * @param bool op  -- include path in output
 * @return std::vector<std::string>
 */
DECLSPEC std::vector<std::string>
visit_directory(std::filesystem::path const &dir, bool opath);

/**
 * @brief get recursive listing of files in dir
 *
 * @param std::filesystem::path const & dir
 * @return std::vector<std::string>
 */
DECLSPEC std::vector<std::string>
visit_directory_recursive(std::filesystem::path const &dir);

/**
 * @brief compile time comparison of two strings
 * @param char const * a
 * @param char const * b
 *
 */
DECLSPEC constexpr bool strings_equal(char const *a, char const *b) {
  return *a == *b && (*a == '\0' || strings_equal(a + 1, b + 1));
}

/**
@brief get a lower case version of s
@param std::string const & s
@return std::string
*/
DECLSPEC std::string toLowerCase(std::string const &s);

/**
@brief get an uppper case version of s
@param std::string const & s
@return std::string
*/
DECLSPEC std::string toUpperCase(std::string const &s);

/**
 * @brief tests whether string str starts with prefix string pre
 *
 * @param std::string const &str
 * @param std::string const& pre
 * @result bool
 */
DECLSPEC bool startsWith(std::string const &str, std::string const &pre);
/**
 * \see startsWith(std::string const &str, std::string const& pre)
 *
 * @param std::string const &str
 * @param std::string const& suf
 * @result bool
 */
DECLSPEC bool endsWith(std::string const &str, std::string const &suf);

/**
 * @brief date-time-stamp now
 */
DECLSPEC std::string DTS();

/**
@brief a mersenne twister rng. use getRNG() and then use a distribution e.g.
auto myrng=spgxyz::MyMTRNG();
auto rng=myrng.getRNG();
auto ud = std::uniform_int_distribution<>{ 0, 100 };
for (int i = 0; i < n; i++)
        result.push_back(ud(rng));

to seed the generator use MyMTRNG(std::vector<int> seeds), the length of seeds
can be anything up to the length of mt19937 internal state integer (624). the
default constructor seeds all internal state integers. seed with single value
will seed a single internal state integer

*/
class DECLSPEC MyMTRNG {
  std::mt19937 const *rngPtr;

  MyMTRNG(MyMTRNG const &) = delete;
  MyMTRNG &operator=(MyMTRNG const &) = delete; // don't want copy operations
public:
  MyMTRNG();
  MyMTRNG(std::vector<int> seeds);
  std::mt19937 const &getRNG();
  ~MyMTRNG();
};

/**
given random number generator, lower and upper bounds, and n
provide integers from uniform random dist, get with sort true will provide
sorted integers
*/
class DECLSPEC UniqueUniformRandomInts {
  std::vector<int> ints;
  std::vector<int> sortedInts;

public:
  UniqueUniformRandomInts(MyMTRNG &mmt, int l, int u, int n);
  std::vector<int> const &get(bool sort);
};

/**
 * @brief convert scoped enum to underlying type so can be used e.g. as index
 *
 * https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int/8357462#8357462
 * e.g.
 * enum class idx {a,b,c};
 * idx myidx=idx::a;
 * int value=myvector[to_underlying(myidx)];
 *
 * @param E e
 * @return std::underlying_type<E>::type
 */
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

/**
 * @brief extract an integer or double from string between two delimiters
 *
 * double cp=0
 * extractFromString(str, ">", "</", cp);
 *
 * int cp=0
 * extractFromString(str, ">", "</", cp);
 *
 */
template <typename T, typename = typename std::enable_if<
                          std::is_arithmetic<T>::value, T>::type>
void extractFromString(std::string const &str, std::string const &delim1,
                       std::string const &delim2, T &result) {
  auto pos1 = str.find(delim1);
  if (pos1 == std::string::npos)
    throw std::runtime_error("could not find " + delim1 + " in " + str +
                             " in " + __func__);
  pos1 += delim1.size();              // pos1 is first position after target
  auto pos2 = str.find(delim2, pos1); // pos2 is start of closing tag
  auto ss =
      str.substr(pos1, pos2 - pos1); // ss should contain an arithmetic type T

  if (std::is_integral<T>::value)
    result = std::stoi(ss); // there are two arithmetic types -- integral
  else
    result = std::stod(ss); // or floating point
}

template <typename E>
constexpr typename std::underlying_type<E>::type getEnumValue(E e) {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

/**
 * @brief delete the file, checks for existence first
 *
 * @param f
 * @return bool true on success
 */
DECLSPEC bool deleteFile(std::string f);

/**
 * @brief delete the file, check for existence first
 *
 * @param f
 * @return bool true on success
 */
DECLSPEC bool deleteFile(std::filesystem::path const &f);

/**
 * @brief provide n equally spaced cutpoints in the range (0,T). n,T>0
 *
 * @param double T
 * @param int n
 * @return std::vector<double>
 */
DECLSPEC std::vector<double> getRanges(double T, int n);

/**
 * @brief provide n equally spaced cutpoints in the range (0,T). n,T>0
 *
 * @note the range 0..first cut may not be the same as x..last cut
 * @param int T
 * @param int n
 * @return std::vector<int>
 */
DECLSPEC std::vector<int> getRanges(int T, int n);

} // namespace spgxyz
