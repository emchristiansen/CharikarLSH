#include <vector>
#include <cassert>
#include <boost/range/algorithm/sort.hpp>
#include <boost/algorithm/minmax_element.hpp>

#include "fUtil.hpp"

namespace Brute {
using namespace std;

int l0Distance(const vector<bool>& left, const vector<bool>& right) {
  assert(left.size() == right.size());
  const auto zipped = FUtil::zip(left, right);
  const function<bool(pair<bool, bool>)> notEqual = [](pair<bool, bool> lAndR) {
    return lAndR.first != lAndR.second;
  };
  return FUtil::count(zipped, notEqual);
}

// Returns a tuple with:
// pair.first = the distance to the nearest element
// pair.second = the index of the nearest element
pair<int, int> findNearestBruteForce(const vector<vector<bool>>& dataset,
    const vector<bool>& query) {
  // Omit the & for type reasons.
  const function<int(const vector<bool>)> distanceFromQuery =
      [query](const vector<bool> entry) {
        return l0Distance(query, entry);
      };
  const auto distances = FUtil::map(dataset, distanceFromQuery);
  const auto zipped = FUtil::zipWithIndex(distances);
  return *boost::minmax_element(zipped.begin(), zipped.end()).first;
}
}
