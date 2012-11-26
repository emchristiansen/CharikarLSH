#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#ifndef UTIL_HPP
#define UTIL_HPP

namespace Util {
using namespace std;

// Range is [start, stop).
vector<int> range(int start, int stop) {
  vector<int> out;
  for (int i = start; i < stop; ++i)
    out.push_back(i);
  return out;
}

template<class A>
string mkString(const vector<A>& vec, const string& left,
    const string& separator, const string& right) {
  stringstream buffer;
  buffer << left;
  for (int i = 0; i < vec.size() - 1; ++i)
    buffer << vec.at(i) << separator;
  buffer << vec.at(vec.size() - 1);
  buffer << right;
  return buffer.str();
}

template<class A>
string toString(const vector<A>& vec) {
  return mkString(vec, "[", ", ", "]");
}

void println(const string& message) {
  cout << message << endl;
}

template<class A, class B>
B takeSecond(pair<A, B> x) {
  return x.second;
}

//const function<int(pair<int, int>)> takeSecond =
//    [](pair<int, int> x) {return x.second;};

bool randomBool() {
  // Because C++ doesn't understand how % is supposed to work.
  const uint64_t positiveInt = rand();
  return positiveInt % 2 == 1 ? true : false;
}

template<class A>
int binarySearch(const vector<A>& vec, const A& query) {
  const auto iter = lower_bound(vec.begin(), vec.end(), query);
//  assert(iter != vec.end());
  const int offset = iter - vec.begin();
//  assert(offset >= 0);
//  assert(offset < vec.size());
//  assert(*iter == vec.at(offset));
  return offset;
}

template<class A>
vector<A> matlabSlice(const vector<A>& vec, const vector<int>& indices) {
  vector<A> out;
  for (int index : indices) out.push_back(vec.at(index));
  return out;
}
}

#endif
