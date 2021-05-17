/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void PrintAllMatches(const Graph &data, const Graph &query,
                       const CandidateSet &cs);

 private:
  std::vector<Vertex> path;

  bool isConnected(size_t level, const Graph &data, const Graph &query);
  void query(size_t level, const Graph &data, const Graph &query, 
                const CandidateSet &cs);
  void printPath(size_t query_size);
};

#endif  // BACKTRACK_H_
