// utils.cpp : Defines the exported functions for the DLL application.
//
#include "utils.h"
namespace spgxyz {

const size_t veryTinyBuff = 8;
const size_t tinyBuff = 64;
const size_t smallBuff = 256;
const size_t largeBuff = 1024;
const size_t hugeBuff = 4096;
std::vector<std::string> splitStr(const std::string &s,
                                  const std::string &regex) {
  std::vector<std::string> result;
  std::regex rgx(regex);
  std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
  std::sregex_token_iterator end;
  while (iter != end) {
    result.push_back(*iter);
    ++iter;
  }
  return result;
} // std::vector<std::string> splitStr(const std::string &s, const std::string
  // &regex)

std::vector<std::string> getPath() {
#pragma warning(suppress : 4996)
  auto path = std::getenv("PATH");
  std::string p{path};
  std::vector<std::string> result;
  result = splitStr(p, ";");
  return result;
}

std::vector<std::string> readFile(const std::string &f) {
  std::vector<std::string> result;
  std::ifstream inf(f);
  if (!inf)
    throw std::runtime_error{
        std::string{"Could not open file " + f + " "}.append(__func__)};
  std::string buff;
  while (std::getline(inf, buff))
    result.push_back(buff);
  inf.close();
  return result;
} // std::vector<std::string> readFile(const std::string& f)

std::vector<std::string> readFile(const std::string &f, int s, int e) {
  std::vector<std::string> result;
  std::ifstream inf(f);
  if (!inf)
    throw std::runtime_error{
        std::string{"Could not open file " + f + " "}.append(__func__)};
  std::string buff;
  int count = 0;
  while (std::getline(inf, buff)) {
    ++count;
    if ((count >= s) && (count <= e))
      result.push_back(buff);
    if (count == e)
      break;
  }
  inf.close();
  if (count != e)
    throw std::out_of_range{
        std::string{"Invalid start and/or end index "}.append(__func__)};
  return result;
}

std::vector<std::string> filterStrings(std::vector<std::string> const &in,
                                       std::string const &regx) {
  std::vector<std::string> result;
  std::smatch m;
  auto rgx = std::regex(regx);
  for (auto &s : in) {
    std::regex_search(s, m, rgx);
    if (!m.empty())
      result.push_back(s);
  } // for(auto &s : in) {
  return result;
}

std::vector<std::string> get_directories(std::filesystem::path const &dir) {
  std::vector<std::string> result;
  if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir))
    for (auto const &entry : std::filesystem::directory_iterator(dir))
      if (std::filesystem::is_directory(entry.status()))
        result.push_back(entry.path().filename().string());
  return result;
}

std::vector<std::string> visit_directory(std::filesystem::path const &dir) {
  return visit_directory(dir, false);
} // std::vector<std::string> visit_directory(std::filesystem::path const &
  // dir)

std::vector<std::string> visit_directory(std::filesystem::path const &dir,
                                         bool opath) {
  std::vector<std::string> result;
  if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
    for (auto const &entry : std::filesystem::directory_iterator(dir)) {
      std::filesystem::path filename;
      if (opath)
        filename = entry.path();
      else
        filename = entry.path().filename();
      /*if (std::filesystem::is_directory(entry.status()))
              std::cout << "[+]" << filename << std::endl;
      else if (std::filesystem::is_symlink(entry.status()))
              std::cout << "[>]" << filename << std::endl;
      else if (std::filesystem::is_regular_file(entry.status()))
              std::cout << " " << filename << std::endl;
      else
              std::cout << "[?]" << filename << std::endl;*/
      if (std::filesystem::is_regular_file(entry.status()))
        result.push_back(filename.string());
    }
  }
  return result;
} // std::vector<std::string> visit_directory(std::filesystem::path const &
  // dir)

std::vector<std::string>
visit_directory_recursive(std::filesystem::path const &dir) {
  std::vector<std::string> result;
  if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
    for (auto const &entry :
         std::filesystem::recursive_directory_iterator(dir)) {
      auto filename = entry.path(); //.filename();
      /*if (std::filesystem::is_directory(entry.status()))
              std::cout << "[+]" << filename << std::endl;
      else if (std::filesystem::is_symlink(entry.status()))
              std::cout << "[>]" << filename << std::endl;
      else if (std::filesystem::is_regular_file(entry.status()))
              std::cout << " " << filename << std::endl;
      else
              std::cout << "[?]" << filename << std::endl;*/
      if (std::filesystem::is_regular_file(entry.status()))
        result.push_back(filename.string());
    }
  }
  return result;
} // std::vector<std::string> visit_directory(std::filesystem::path const &
  // dir)

std::locale loc;
// needed to disambiguate tolower, a version with locale parameter is being
// selected because locale is being included
std::string toLowerCase(std::string const &s) {
  auto result = std::string(s);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c, loc); });
  return result;
}

std::string toUpperCase(std::string const &s) {
  auto result = std::string(s);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c, loc); });
  return result;
}

bool startsWith(std::string const &str, std::string const &pre) {
  if (pre.length() > str.length())
    return false;
  auto strIt = str.cbegin();
  for (auto it = pre.cbegin(); it != pre.cend(); ++it) {
    if (*it != *strIt)
      return false;
    ++strIt;
  }
  return true;
}

bool endsWith(std::string const &str, std::string const &suf) {
  if (suf.length() > str.length())
    return false;
  auto strIt = str.crbegin();
  for (auto it = suf.crbegin(); it != suf.crend(); ++it) {
    if (*it != *strIt)
      return false;
    ++strIt;
  }
  return true;
}

std::string DTS() {
  auto dts =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#pragma warning(suppress : 4996)
  std::string result{std::ctime(&dts)};
  return result;
}

MyMTRNG::MyMTRNG() {
  std::random_device rd;
  std::array<int, std::mt19937::state_size> seed_data{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  rngPtr = new std::mt19937(seq);
}

MyMTRNG::MyMTRNG(std::vector<int> seeds) {
  std::seed_seq seq(std::begin(seeds), std::end(seeds));
  rngPtr = new std::mt19937(seq);
}
std::mt19937 const &MyMTRNG::getRNG() {
  std::mt19937 const &result = *rngPtr;
  return result;
}

MyMTRNG::~MyMTRNG() { delete rngPtr; }

UniqueUniformRandomInts::UniqueUniformRandomInts(MyMTRNG &mmt, int l, int u,
                                                 int n) {
  assert(
      ("u-l must be greater than n in UniqueUniformRandomInts", (u - l) > n));
  ints.reserve(n);
  auto ud = std::uniform_int_distribution<>{l, u};
  auto rng = mmt.getRNG();
  while (ints.size() < n) {
    auto i = ud(rng);
    if (std::find(ints.begin(), ints.end(), i) == ints.end())
      ints.push_back(i);
  }
  sortedInts = ints;
  std::sort(sortedInts.begin(), sortedInts.end());
}

std::vector<int> const &UniqueUniformRandomInts::get(bool sort) {
  if (sort)
    return sortedInts;
  return ints;
}

bool deleteFile(std::string f) { return deleteFile(std::filesystem::path{f}); }

bool deleteFile(std::filesystem::path const &f) {
  if (std::filesystem::exists(f)) {
    if (!std::filesystem::remove(f))
      std::cout << f << " could not be removed" << std::endl;
    else
      return true;
  }
  std::cout << f << " not found" << std::endl;
  return false;
}

std::vector<int> getRanges(int T, int n) {
  if (T < 2 || n < 1 || T <= n)
    throw std::out_of_range(std::string{" out of range in function:  "} +
                            __func__);
  auto stride = static_cast<double>(T) / (n + 1);
  std::vector<int> result;
  for (int i = 0; i < n; ++i)
    result.push_back(std::round((i + 1) * stride));
  return result;
}

std::vector<double> getRanges(double T, int n) {
  if (T <= 0 || n <= 0)
    throw std::out_of_range(std::string{" out of range in function:  "} +
                            __func__);
  auto stride = T / (n + 1);
  std::vector<double> result;
  for (int i = 0; i < n; ++i)
    result.push_back((i + 1) * stride);
  return result;
}

} // namespace spgxyz
