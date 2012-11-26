#include <memory>
#include <set>

#include "permutationUtil.hpp"

#ifndef CHARIKAR_HPP
#define CHARIKAR_HPP

// This is the basic unit of the Charikar LSH scheme. A CharikarStripe
// is analogous to a single hash function in other LSH schemes.
struct CharikarStripe {
  const int dimension;
  const int numActiveDimensions;
  const int size;
  const Permutation permutation;
  const std::vector<std::pair<std::vector<bool>, int>> sortedPermutedData;

  CharikarStripe(int dimension_, int numActiveDimensions_, int size_,
      const Permutation& permutation_,
      const std::vector<std::pair<std::vector<bool>, int>>& sortedPermutedData_) :
      dimension(dimension_), numActiveDimensions(numActiveDimensions_), size(
          size_), permutation(permutation_), sortedPermutedData(
          sortedPermutedData_) {
    assert(numActiveDimensions <= dimension);
    assert(size > 0);

    for (auto entry : sortedPermutedData)
      assert(entry.first.size() == numActiveDimensions);

    const auto indices = FUtil::map(sortedPermutedData,
        Util::takeSecond<std::vector<bool>, int>);
    assert(PermutationUtil::isPermutation(indices));

    for (int i = 0; i < size; ++i)
      for (int j = i + 1; j < size; ++j)
        assert(sortedPermutedData.at(i) <= sortedPermutedData.at(j));
  }
};

namespace CharikarUtil {
using namespace std;

// In the stored array, only the |numActiveDimensions| most significant
// bits will be stored; this is done to save memory and speed construction
// time. A good number for |numActiveDimensions| is probably between 8
// and 16.
shared_ptr<CharikarStripe> mkCharikarStripe(int numActiveDimensions,
    const vector<vector<bool>>& dataset) {
  const auto permutation = PermutationUtil::randomPermutation(
      dataset.at(0).size());
  const function<vector<bool>(const vector<bool>)> permuteAndSlice =
      [permutation, numActiveDimensions](const vector<bool> entry) {
        return FUtil::slice(PermutationUtil::permute(permutation, entry), 0, numActiveDimensions);
      };
  const vector<vector<bool>> permutedDataset = FUtil::map(dataset,
      permuteAndSlice);
  // boost::sort doesn't take const.
  vector<pair<vector<bool>, int>> permutedWithIndex = FUtil::zipWithIndex(
      permutedDataset);
  const vector<pair<vector<bool>, int>> sortedPermutedData = boost::sort(
      permutedWithIndex);

  return shared_ptr < CharikarStripe
      > (new CharikarStripe(dataset.at(0).size(), numActiveDimensions,
          dataset.size(), permutation, sortedPermutedData));
}

// Multiple CharikarStripes may be combined together to make a more accurate
// LSH system. A system in which memory is scarce but CPU cycles are abundant
// would use fewer stripes and a higher neighbor radius. When memory is
// abundant, more stripes can be used with a smaller neighbor radius. Note
// construction time is linear in the number of stripes.
// This is analogous to a collection of hash functions in other LSH schemes.
vector<shared_ptr<CharikarStripe>> mkCharikarStripes(int numStripes, int numActiveDimensions,
    const vector<vector<bool>>& dataset) {
  const function<shared_ptr<CharikarStripe>(void)> stripe =
      [dataset, numActiveDimensions](void) {return mkCharikarStripe(numActiveDimensions, dataset);};

  return FUtil::fillResult<shared_ptr<CharikarStripe>>(numStripes, stripe);
}

// Returns a set of approximate nearest neighbors.
vector<int> findNearest(const CharikarStripe& stripe, int neighborRadius,
    const vector<bool>& query) {
  const auto permutedSlicedQuery = FUtil::slice(PermutationUtil::permute(stripe.permutation,
      query), 0, stripe.numActiveDimensions);
  const int centerIndex = Util::binarySearch(stripe.sortedPermutedData,
      make_pair(permutedSlicedQuery, 0));
  const int begin = max(centerIndex - neighborRadius, 0);
  const int end = min(centerIndex + neighborRadius,
      static_cast<int>(stripe.sortedPermutedData.size()));
  const vector<pair<vector<bool>, int>> slice = FUtil::slice(
      stripe.sortedPermutedData, begin, end);
  return FUtil::map(slice, Util::takeSecond<vector<bool>, int>);
}

vector<int> findNearest(const vector<shared_ptr<CharikarStripe>>& stripes,
    int neighborRadius, const vector<bool>& query) {
  set<int> uniqueIndices;
  for (auto stripe : stripes) {
    const auto indices = findNearest(*stripe, neighborRadius, query);
    for (auto index : indices)
      uniqueIndices.insert(index);
  }

  vector<int> out;
  for (auto index : uniqueIndices)
    out.push_back(index);
  return out;
}
}

#endif
