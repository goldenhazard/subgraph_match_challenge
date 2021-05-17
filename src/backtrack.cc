/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";

  // implement your code here.
  Backtrack::query(0, data, query, cs);
}

bool Backtrack::isConnected(size_t level, const Graph &data, 
                            const Graph &query) {
  // If visited vertex isn't connected while query is connected, return false
  for(size_t i=0;i<level;i++) {
    if(query.IsNeighbor(i,level-1)) {
      std::cout << "[DEBUG] u_" << i+1 << " and u_" <<
          level << " has to be connected -> ";
      if(!data.IsNeighbor(path[i],path[level-1])) {
        std::cout << "v_" << path[i]+1 << " and v_" <<
            path[level-1]+1 << " is not connected -> return FALSE" <<
            std::endl;
        return false;
      } else {
        std::cout << "v_" << path[i]+1 << " and v_" <<
            path[level-1]+1 << " is connected -> CONTINUE" << 
            std::endl;
      }
    } else {
      std::cout << "[DEBUG] Don't care whether u_" << i+1 << 
          " and u_" << level << " is connected -> CONTINUE" <<
          std::endl;
    }
  }
  return true;
}

void Backtrack::query(size_t level, const Graph &data, 
                      const Graph &query, const CandidateSet &cs) {
  
  std::cout << "\n==================== Current Level : " << level <<
              " ====================\n[PATH] ";
  Backtrack::printPath(level);

  // Check if current vertex is connected with previous visited vertex
  if(!isConnected(level, data, query)) {
    return;
  }

  // If current vertex is the last vertex, print the path
  else if(level == query.GetNumVertices()) {
    Backtrack::printPath(query.GetNumVertices());
    return;
  }

  // Check every vertex at current level from candidate set
  for(size_t i=0;i<cs.GetCandidateSize(level);i++) {
    path.push_back(cs.GetCandidate(level, i));
    Backtrack::query(level+1, data, query, cs);
    path.pop_back();
  }
}

void Backtrack::printPath(size_t query_size) {
  std::cout << "a ";
  for(size_t i=0;i<query_size;i++) {
      std::cout << path[i] << " ";
  }
  std::cout << std::endl;
}