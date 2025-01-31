#include "zonemaps.cpp"
#include "zonemaps.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <vector>

// Specify your path of workload file here
std::string kInputDataPath = "./workload.dat";
std::string kPointQueriesPath = "./point_queries.txt";
std::string kRangeQueriesPath = "./range_queries.txt";
const uint32_t kRuns = 3;
using namespace std;

void loadPointQueries(std::string &input_queries_path, std::vector<int> &queries) {
    queries.clear();
    std::ifstream infile(input_queries_path, ios::in);
    int tmp;
    while (infile >> tmp) {
        queries.push_back(tmp);
    }
    // shuffle indexes
    // uncomment the shuffle used for the required version of cpp.
    // random_shuffle no longer works
    std::random_shuffle(queries.begin(), queries.end());
    // std::shuffle(queries.begin(), queries.end(),
    // std::mt19937{std::random_device{}()});
}

void loadRangeQueries(std::string &input_queries_path, std::vector<std::pair<int, int>> &queries) {
    queries.clear();
    std::ifstream infile(input_queries_path, ios::in);
    // in the range_queries.txt file, there are two values instead of one
    // the first value is the low value and the second is a high value
    // separated by space
    int low, high;
    while (!infile.eof()) {
        infile >> low >> high;
        if (!infile.fail()) {
            // more effective than push_back as per stack overflow article
            // link:
            // https://stackoverflow.com/questions/10890653/why-would-i-ever-use-push-back-instead-of-emplace-back
            queries.emplace_back(low, high);
        }
    }
    // uncomment the shuffle used for the required version of cpp.
    // random_shuffle no longer works
    std::random_shuffle(queries.begin(), queries.end());
    // std::shuffle(queries.begin(), queries.end(),
    // std::mt19937{std::random_device{}()});
}

int main(int argc, char **argv) {
    // read data
    std::ifstream ifs;
    std::vector<int> data;

    ifs.open(kInputDataPath, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    size_t filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    data.resize(filesize / sizeof(int));
    ifs.read((char *) data.data(), filesize);

    // 1. ----------------------------- initialize zonemap and build
    // ----------------------------- build zonemap
    zonemap<int> zones(data, (uint) data.size() / 100);

    // 2. ----------------------------- point queries
    // -----------------------------
    std::vector<int> queries;
    loadPointQueries(kPointQueriesPath, queries);
    auto start_pq = std::chrono::high_resolution_clock::now();
    for (size_t r = 0; r < kRuns; r++) {
        for (int pq : queries) {
            // run query for each entry in the range_query file for kRuns
            zones.query(pq);
        }

        // shuffle indexes
        // uncomment the shuffle used for the required version of cpp.
        // random_shuffle no longer works
        // std::shuffle(queries.begin(), queries.end(),
        // std::mt19937{std::random_device{}()});
        std::random_shuffle(queries.begin(), queries.end());
    }
    auto stop_pq = std::chrono::high_resolution_clock::now();
    auto duration_pq = std::chrono::duration_cast<std::chrono::microseconds>(
        stop_pq - start_pq);
    unsigned long long point_query_time = duration_pq.count();
    std::cout << "Time taken to perform point queries from zonemap = "
              << point_query_time * 1.0 / kRuns << " microseconds" << endl;

    // 3. ----------------------------- range queries
    // -----------------------------
    std::vector<std::pair<int, int>> range_queries;
    // point queries had just one key and range_queries have 2 values
    // this part is exactly the same as point queries since the only difference
    // is that range queries are a pair of values
    loadRangeQueries(kRangeQueriesPath, range_queries);
    //if there is no range query there is no point in doing this
    if(!range_queries.empty()){
      auto start_rq = std::chrono::high_resolution_clock::now();
      for (size_t r = 0; r < kRuns; r++) {
          for (const auto &rq : range_queries) {
              // we need to call the appropriate query function defined in
              // zonemaps
              zones.query(rq.first, rq.second);
          }
          // shuffle indexes
          // uncomment the shuffle used for the required version of cpp.
          // random_shuffle no longer works
          // std::shuffle(range_queries.begin(), range_queries.end(),
          // std::mt19937{std::random_device{}()});
          std::random_shuffle(queries.begin(), queries.end());
      }
      auto stop_rq = std::chrono::high_resolution_clock::now();
      // difference between start and end - same as point queries
      auto duration_rq = std::chrono::duration_cast<std::chrono::microseconds>(
          stop_rq - start_rq);
      unsigned long long range_query_time = duration_rq.count();
      std::cout << "Time taken to perform range query from zonemap = "
                << range_query_time * 1.0 / kRuns << " microseconds" << endl;
    }
    else{
      std::cout << "Time taken to perform range query from zonemap = 0 microseconds" << endl;
    }
    return 0; 
}
