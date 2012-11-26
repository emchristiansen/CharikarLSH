#include "util.hpp"
#include "fUtil.hpp"
#include "permutationUtil.hpp"
#include "brute.hpp"
#include "charikar.hpp"

using namespace std;

// For testing: Generate a random dataset entry.
vector<bool> generateEntry(int dimension) {
  const auto range = Util::range(0, dimension);
  const function<bool(int)> randomBool =
      [](int ignore) {return Util::randomBool();};
  return FUtil::map(range, randomBool);
}

// For testing: Generate a random dataset.
vector<vector<bool>> generateDataset(int dimension, int size) {
  const auto range = Util::range(0, size);
  const function<vector<bool>(int)> entry =
      [dimension](int ignore) {return generateEntry(dimension);};
  return FUtil::map(range, entry);
}

// Tests the Charikar implementation. Returns |true| if the set
// of approximate nearest neighbors includes the true nearest
// neighbor.
bool testCharikar(int datasetDimension, int datasetActiveDimension,
    int datasetSize, int numStripes, int neighborRadius) {
  const vector<vector<bool>> dataset = generateDataset(datasetDimension,
      datasetSize);

  const vector<bool> query = generateEntry(datasetDimension);

  auto stripes = CharikarUtil::mkCharikarStripes(numStripes,
      datasetActiveDimension, dataset);
  auto indices = CharikarUtil::findNearest(stripes, neighborRadius, query);
  auto neighbors = Util::matlabSlice(dataset, indices);

  // The nearest from the Charikar subset.
  auto nearest = Brute::findNearestBruteForce(neighbors, query);
  // The brute force nearest from the entire dataset.
  auto nearestBrute = Brute::findNearestBruteForce(dataset, query);
  return nearest.first == nearestBrute.first;
}

// Tests a baseline method (random subset selection). Returns |true|
// if the random subset includes the true nearest neighbor.
bool testRandom(int datasetDimension, int datasetSize, int subsetSize) {
  const vector<vector<bool>> dataset = generateDataset(datasetDimension,
      datasetSize);

  const vector<bool> query = generateEntry(datasetDimension);

  // The nearest from the random subset.
  auto nearest = Brute::findNearestBruteForce(
      FUtil::slice(dataset, 0, subsetSize), query);
  // The brute force nearest from the entire dataset.
  auto nearestBrute = Brute::findNearestBruteForce(dataset, query);
  return nearest.first == nearestBrute.first;
}

int main() {
  // The higher the dimension, the worse LSH does, assuming the bits
  // are independent. If they are dependent (ie the data lays on some
  // unknown low-dimensional subspace) the scheme should be independent (?)
  // of the dimension of the representation.
  const int datasetDimension = 32;
  const int datasetSize = 1000;

  // The larger this number, the slower the algorithm, but the better
  // the best possible accuracy. If |datasetActiveDimension| is |n|, then
  // the hashing scheme cannot be more than 2^|n| faster than brute force.
  const int datasetActiveDimension = 8;
  // The more stripes, the more accurate the scheme, but the more memory and
  // CPU are used.
  const int numStripes = 16;
  // The larger the radius, the more accurate the scheme, but the more CPU
  // is used. When |numStripes| is low and |neighorRadius| is very high, this
  // is like random subset selection. Query performance is best when
  // |numStripes| is high and |neighborRadius| is low, but this will use
  // lots of memory. A good balance is to set |numStripes| == |neighborRadius|.
  const int neighborRadius = 16;

  // The number of tests to run comparing Charikar to random. The numbers
  // returned slightly favor random because the random scheme selects a
  // subset that is at least as big as the Charikar subset (it's the tight
  // upper bound).
  const int numTests = 100;

  const int numCharikarRetrieved = numStripes * 2 * neighborRadius;

  const function<bool()> runCharikar =
      [&]() {
        return testCharikar(datasetDimension, datasetActiveDimension, datasetSize, numStripes, neighborRadius);
      };
  const vector<bool> charikarPerformance = FUtil::fillResult(numTests,
      runCharikar);
  const int charikarRight = accumulate(charikarPerformance.begin(),
      charikarPerformance.end(), 0);
  printf("charikar num right: %d\n", charikarRight);

  const function<bool()> runRandom = [&]() {
    return testRandom(datasetDimension, datasetSize, numCharikarRetrieved);
  };
  const vector<bool> randomPerformance = FUtil::fillResult(numTests, runRandom);
  const int randomRight = accumulate(randomPerformance.begin(),
      randomPerformance.end(), 0);
  printf("random num right: %d\n", randomRight);
}
