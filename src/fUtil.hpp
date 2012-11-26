#include <functional>
#include <cassert>

#ifndef FUTIL_HPP
#define FUTIL_HPP

#include "util.hpp"

// Some higher-order functions common in functional languages.
namespace FUtil {
using namespace std;

template<class A, class B>
vector<B> map(const vector<A>& data, function<B(A)> func) {
  vector<B> out;
  for (A a : data)
    out.push_back(func(a));
  return out;
}

// Because C++ treats std::function differently than function
// pointers.
template<class A, class B>
vector<B> map(const vector<A>& data, B func(A)) {
  return map(data, function<B(A)>(func));
}

// Counts the number of times the predicate is satisfied.
template<class A>
int count(const vector<A>& data, function<bool(A)> predicate) {
  int total = 0;
  for (A a : data)
    total += predicate(a) ? 1 : 0;
  return total;
}

template<class A, class B>
vector<pair<A, B>> zip(const vector<A>& left, const vector<B>& right) {
  vector<pair<A, B>> out;
  auto l = left.begin();
  auto r = right.begin();
  for (; l != left.end() && r != right.end(); ++l, ++r)
    out.push_back(make_pair(*l, *r));
  return out;
}

template<class A>
vector<pair<A, int>> zipWithIndex(const vector<A>& vec) {
  const auto indices = Util::range(0, vec.size());
  return zip(vec, indices);
}

template<class A>
vector<A> fill(int size, const A& element) {
  vector<A> out;
  for (int i = 0; i < size; ++i) out.push_back(element);
  return out;
}

template<class A>
vector<A> fillResult(const int size, const function<A(void)> f) {
  vector<A> out;
  for (int i = 0; i < size; ++i) out.push_back(f());
  return out;
}

template<class A>
vector<A> slice(const vector<A>& vec, int begin, int end) {
  assert(begin >= 0);
  assert(end <= vec.size());

  vector<A> out;
  for (int i = begin; i < end; ++i) out.push_back(vec.at(i));
  return out;
}
}

#endif
