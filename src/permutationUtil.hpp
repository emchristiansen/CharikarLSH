#include <boost/algorithm/minmax_element.hpp>
#include <boost/range/algorithm/sort.hpp>

#ifndef PERMUTATIONUTIL_HPP
#define PERMUTATIONUTIL_HPP

#include "util.hpp"
#include "fUtil.hpp"

namespace PermutationUtil {
using namespace std;

bool isPermutation(vector<int> permutation) {
  // Boost wants a mutable copy.
  auto permutationCopy = permutation;
  const auto sorted = boost::sort(permutationCopy);
  const auto range = Util::range(0, permutation.size());

  return sorted == range;
}
}

// The |data| member is guaranteed to be a permutation.
struct Permutation {
  const std::vector<int> data;

  // The constructor ensures the |vector<int>| satisfies the
  // permutation property.
  Permutation(std::vector<int> permutation) :
      data(permutation) {
    assert(PermutationUtil::isPermutation(permutation));
  }
};

namespace PermutationUtil {
using namespace std;

Permutation randomPermutation(int size) {
  const vector<int> range = Util::range(0, size);

  const function<int(int)> runRand = [](int x) {return rand();};
  const vector<int> randomInts = FUtil::map(range, runRand);

  vector<pair<int, int>> intsWithIndices = FUtil::zipWithIndex(randomInts);
  const auto sorted = boost::sort(intsWithIndices);

  const auto permutationData = FUtil::map(sorted, function<int(pair<int, int>)>(Util::takeSecond<int, int>));

  return Permutation(permutationData);
}

Permutation invert(const Permutation& permutation) {
  auto withIndex = FUtil::zipWithIndex(permutation.data);
  const auto sorted = boost::sort(withIndex);
  const auto permutationData = FUtil::map(sorted, Util::takeSecond<int, int>);
  return Permutation(permutationData);
}

// Permutes the elements of |vec| according to the given permutation.
template<class A>
vector<A> permute(const Permutation& permutation, const vector<A>& vec) {
  assert(permutation.data.size() == vec.size());

  // We can express this as a map if we work with the inverse permutation.
  const auto inverse = invert(permutation);
  const auto range = Util::range(0, vec.size());

  const function<A(int)> grabValue =
      [inverse, vec](int j) {return vec.at(inverse.data.at(j));};
  const auto permuted = FUtil::map(range, grabValue);
  return permuted;
}
}

#endif
