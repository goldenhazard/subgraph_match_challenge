/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

using VertexPair = std::pair<Vertex, Vertex>;
using Cmu = std::pair<Vertex, std::set<Vertex>>;

// Sorting Criteria
bool VertexCompare(VertexPair& a, VertexPair& b){
  if(a.first < b.first) return true;
  return false;
}

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";

  // implement your code here.
  visited_u.resize(query.GetNumVertices());
  visited_v.resize(data.GetNumVertices());

  std::vector<VertexPair> matchedVertex;
  DoBacktrack(data, query, cs, matchedVertex);
}

void Backtrack::PrintPath(std::vector<VertexPair>& partial_embedding){
  // Sort according to the pair.first
  embedding_number += 1;
  std::sort(partial_embedding.begin(), partial_embedding.end(), VertexCompare);
  std::cout << "[" << embedding_number << "]";
  std::cout << "a ";
  for(auto pair: partial_embedding){
    std::cout << "(" << pair.first << ",";
    std::cout << pair.second << ")" << ",";
  }
  std::cout << std::endl;
}

void Backtrack::DoBacktrack(const Graph& data, const Graph& query, const CandidateSet& cs, 
                            std::vector<VertexPair>& partial_embedding){
  if(partial_embedding.size() == query.GetNumVertices()){
    PrintPath(partial_embedding);
    return;
  }

  else if(partial_embedding.empty()){
    for(size_t idx = 0; idx < cs.GetCandidateSize(0); idx++){
      Vertex next_vertex = cs.GetCandidate(0, idx);
      partial_embedding.push_back(std::make_pair(0, next_vertex));
      visited_v[next_vertex] = true; visited_u[0] = true;
      DoBacktrack(data, query, cs, partial_embedding);
      partial_embedding.pop_back();
      visited_v[next_vertex] = false; visited_u[0] = false;
    }
  }

  else{
    std::vector<Cmu> cmu_heap;
    cmu_heap = BuildCmuHeap(data, query, cs, partial_embedding, cmu_heap);
    if(cmu_heap.empty()) return; // No extendable vertices anymore
    Vertex u_next = NextVertexToExtend(cmu_heap);
    size_t idx = 0;
    for(Vertex v_next: cmu_heap[idx].second){
      partial_embedding.push_back(std::make_pair(u_next, v_next));
      visited_v[v_next] = true; visited_u[u_next] = true;
      DoBacktrack(data, query, cs, partial_embedding);
      partial_embedding.pop_back();
      visited_v[v_next] = false; visited_u[u_next] = false;
    }
  }
}

Vertex Backtrack::NextVertexToExtend(std::vector<Cmu>& cmu_heap){
  // TODO
  /*
  sorting heap by "some criteria"
  */

  return cmu_heap[0].first;
  // std::vector<Cmu> cmu_vector;
  // // Attains cmu from every extendable u
  // for(Vertex u_candidate : u_candidates){
  //   auto v_candidates = GetCmu(data, query, u_candidate, cs, partial_embedding);
  //   if(v_candidates.empty()) continue;
  //   Cmu cmu = std::make_pair(u_candidate, v_candidates);
  //   cmu_vector.push_back(cmu);
  // }

  // return min(cmu_vector);
}

/*
std::vector<Vertex>& Backtrack::GetCmu(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding){
  
}
*/

std::vector<Cmu>& Backtrack::BuildCmuHeap(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding, std::vector<Cmu>& cmu_heap){
  /*
  From data, query, candidate set, partial_embedding

  Procedure)
    1) Find vertex u which parent is in partial_embedding
    2) Find cmu of each extendable u
    3) Add to cmu heap only if cmu's size is not zero

  Returns)
    vector<Cmu> cmu_heap
    <(u2, {v1, v2}), (u3, {v3, v4}), (u4, {v2, v3})>
  */

  // All possible choices
  for(Vertex u = 0; u < int(query.GetNumVertices()); u++){
    
    if(visited_u[u]) continue; // skip if already_visited, u7
    std::set<Vertex> cmu;
    cmu = cs.GetCandidateSet(cmu, u); // cmu of u7 in current state.

    // Checks if all parents are in M.
    std::vector<Vertex> neighbors;
    neighbors = query.GetNeighborVertexes(neighbors, u);
    // Candidate parents
    // ex) [u3, u6, u9]
    // Checks if cmu is alive, parents are in M.
    for(Vertex w : neighbors){
      if(cmu.size() == 0) break;
      // Only parents
      if(w < u){
        std::vector<Vertex> not_connected;
        if(!visited_u[w]) cmu.clear();
        Vertex v_parent = FindVByU(partial_embedding, w);
        for(Vertex v : cmu){
          if(visited_v[v] || !data.IsNeighbor(v_parent, v)) not_connected.push_back(v);
        }
        if(!not_connected.empty()){
          for(Vertex v: not_connected) cmu.erase(v);
        }
      }
    }
    if(!cmu.empty()) cmu_heap.push_back(std::make_pair(u, cmu));
  }

  //if(!cmu_heap.empty()) HeapSummary(cmu_heap);

  return cmu_heap;
}

void Backtrack::HeapSummary(std::vector<Cmu>& cmu_heap){
  // Prints Cmu Heap
  std::cout << "==========================================" << std::endl;
  std::cout << "Heap Summary" << std::endl;
  std::cout << "==========================================" << std::endl;
  std::cout << "Heap Size : " << cmu_heap.size() << std::endl;
  if(cmu_heap.empty()) return;

  for(auto pair : cmu_heap){
    std::cout << "CM(" << pair.first << ") :" << "{";
    for(auto v : pair.second) std::cout << v << "|";
    std::cout << "}" << std::endl;
  }
}

Vertex Backtrack::FindVByU(std::vector<VertexPair>& partial_embedding, Vertex u){
  for(auto pair : partial_embedding){
    if(pair.first == u) return pair.second;
  }
  return -1;
}

Vertex Backtrack::FindUByV(std::vector<VertexPair>& partial_embedding, Vertex v){
  for(auto pair : partial_embedding){
    if(pair.second == v) return pair.first;
  }
  return -1;
}
/*
std::vector<Vertex>& CandidatesInit(const CandidateSet& cs, Vertex u){
  std::vector<Vertex> candidates;
  candidates.resize(cs.GetCandidateSize(u));
  for(size_t idx = 0; idx < cs.GetCandidateSize(u); idx++)
    candidates.push_back(cs.GetCandidate(u, idx));
  return candidates;
}
*/

/*
std::vector<Vertex>& CMU(Vertex u, const std::vector<VertexPair>& partial_embedding){
  // Get cmu from data structure
}
*/