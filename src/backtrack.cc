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
    extendable_u.resize(query.GetNumVertices());
    extendable_v.resize(data.GetNumVertices());

    std::vector<VertexPair> matchedVertex;
    DoBacktrack(data, query, cs, matchedVertex);
}

void Backtrack::PrintPath(std::vector<VertexPair> partial_embedding){
    // Sort according to the pair.first
    embedding_number += 1;
    //std::sort(partial_embedding.begin(), partial_embedding.end(), VertexCompare);
    std::cout << "[" << embedding_number << "]";
    std::cout << "a";
    for(auto pair: partial_embedding){
        std::cout << " ";
        std::cout << "(" << pair.first << ",";
        std::cout << pair.second;
        std::cout << ")";
    }
    std::cout << std::endl;
}

void PrintEmbedding(std::vector<VertexPair> partial_embedding){
    std::cout << "[DEBUG] current embedding : ";
    std::cout << "a";
    for(auto pair: partial_embedding){
        std::cout << " ";
        std::cout << "(" << pair.first << ",";
        std::cout << pair.second;
        std::cout << ")";
    }
    std::cout << std::endl;
}

void Backtrack::DoBacktrack(const Graph& data, const Graph& query, const CandidateSet& cs, 
                            std::vector<VertexPair>& partial_embedding){
    //PrintEmbedding(partial_embedding);
    if(embedding_number >= 100000) return;
    if(partial_embedding.size() == query.GetNumVertices()){
        Check(data, query, cs, partial_embedding);
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
        Cmu cmu_next;
        cmu_next = FindNextVertex(data, query, cs, partial_embedding, cmu_next);
        if(cmu_next.second.empty()) return;
        Vertex u_next = cmu_next.first;

        for(Vertex v_next: cmu_next.second){
            partial_embedding.push_back(std::make_pair(u_next, v_next));
            visited_v[v_next] = true; visited_u[u_next] = true;
            DoBacktrack(data, query, cs, partial_embedding);
            partial_embedding.pop_back();
            visited_v[v_next] = false; visited_u[u_next] = false;
        }
    }
}

Cmu& Backtrack::FindNextVertex(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding, Cmu& cmu_next) {
  // Not using DAG
  std::set<Vertex> cmu;
  std::set<Vertex> neighbors;
  std::vector<Vertex> not_connected;
  Vertex embedded_v;

  size_t method_threshold = size_t(query.GetNumVertices() - log2(query.GetNumVertices()));

  for(VertexPair p : partial_embedding){
    if(partial_embedding.size() < method_threshold)
      neighbors = query.GetNeighborSets(neighbors, p.first); // O(Neighbor size)

    else{
      for(Vertex u = 0; u < int(query.GetNumVertices()); u++){
        if(!visited_u[u]){
          for(auto pair : partial_embedding){
            if(query.IsNeighbor(pair.first, u)){
              neighbors.insert(u);
              break;
            }
          }
        }
      }
    }
  }
  
  for(Vertex u : neighbors) {
      if(visited_u[u]) continue;
      cmu.clear();
      cmu = cs.GetCandidateSet(cmu, u);
      // check valid iterating through partial embedding
      for(VertexPair p : partial_embedding) {
        //if(p.first <= u) continue;
        // O(M)
        if(!query.IsNeighbor(p.first, u)) continue;
        if(cmu.size() == 0) break;
        embedded_v = p.second;
        for(Vertex curr_v : cmu) {
            if(visited_v[curr_v] || !data.IsNeighbor(embedded_v, curr_v)) not_connected.push_back(curr_v);
        }
        for(Vertex v: not_connected) cmu.erase(v);
      }

      // Find possible u that has minimum |C_m(u)|
      //std::cout << "[DEBUG] " << u << " " << cmu.size() << std::endl;
      if(!cmu.empty()) {
          if(cmu_next.second.size() == 0) {
              cmu_next = std::make_pair(u,cmu);
          }
          if(cmu.size() < cmu_next.second.size()) {
              cmu_next.first = u;
              cmu_next.second = cmu;
          }
      }
  }
  
  // Using rooted DAG
  /*std::set<Vertex> cmu; // Real c_M(u)
  std::vector<Vertex> neighbors;
  std::vector<Vertex> not_connected;
  Vertex v_parent;
  // All possible choices
  for(Vertex u = 0; u < int(query.GetNumVertices()); u++){
      cmu.clear();
      neighbors.clear();
      
      if(visited_u[u]) continue; // skip if already_visited, u7
      cmu = cs.GetCandidateSet(cmu, u); // cmu of u7 in current state.
      // Checks if all parents are in M.
      neighbors = query.GetNeighborVertexes(neighbors, u);
      // Candidate parents
      // ex) [u3, u6, u9]
      // Checks if cmu is alive, parents are in M.
      for(Vertex w : neighbors){
          if(cmu.size() == 0) break;
          // Only parents
          if(w < u){
              not_connected.clear();
              if(!visited_u[w]) cmu.clear();
              v_parent = FindVByU(partial_embedding, w);
              for(Vertex v : cmu){
                  if(visited_v[v] || !data.IsNeighbor(v_parent, v)) not_connected.push_back(v);
              }
              for(Vertex v: not_connected) cmu.erase(v);
          }
      }
      // Find possible u that has minimum |C_m(u)|
      //std::cout << "[DEBUG] " << u << " " << cmu.size() << std::endl;
      if(!cmu.empty()) {
          if(cmu_next.second.size() == 0) {
              cmu_next = std::make_pair(u,cmu);
          }
          if(cmu.size() < cmu_next.second.size()) {
              cmu_next.first = u;
              cmu_next.second = cmu;
          }
      }
  }*/

  return cmu_next;
}

void Backtrack::Check(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding) {
    for(int i=0;i<int(query.GetNumVertices());i++) {
        for(int j=i+1;j<int(query.GetNumVertices());j++) {
            Vertex u1, u2, v1, v2;
            u1 = partial_embedding[i].first;
            u2 = partial_embedding[j].first;
            v1 = partial_embedding[i].second;
            v2 = partial_embedding[j].second;
            if(query.IsNeighbor(u1, u2) && !data.IsNeighbor(v1, v2)) {
                std::cout << "u[" << u1 << "],u[" << u2 << "]=" << query.IsNeighbor(u1, u2)
                    << " v[" << v1 << "],v[" << v2 << "]=" << data.IsNeighbor(v1, v2) << std::endl;
                std::cout << "[FALSE]";
                return;
            }
        }
    }
    std::cout << "[TRUE]";
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

/*void Backtrack::pushU(const Graph& query, Vertex u) {
    std::vector<Vertex> neighbors;
    neighbors = query.GetNeighborVertexes(neighbors, u);
    for(Vertex w : neighbors) {
        if(!visited_u[w]) extendable_u[w] = true;
    }
    extendable_u[u] = false;
}

void Backtrack::popU(const Graph& query, Vertex u) {
    std::vector<Vertex> neighbors;
    neighbors = query.GetNeighborVertexes(neighbors, u);
    for(Vertex w : neighbors) {
        if(!visited_u[w]) extendable_u[w] = false;
    }
    extendable_u[u] = true;
}

void Backtrack::pushUV(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v) {
    std::set<Vertex> cmu;
    std::vector<Vertex> neighbors;
    neighbors = query.GetNeighborVertexes(neighbors, u);
    for(Vertex w : neighbors) {
        if(!visited_u[w]) {
            extendable_u[w] = false;
            cmu.clear();
            cmu = cs.GetCandidateSet(cmu, w);
            for(Vertex t : cmu) {
                for(VertexPair p : partial_embedding) {
                    if(!query.IsNeighbor(p.first, w) || data.IsNeighbor(p.second, t)) extendable_v[w].push_back(t);
                }
            }
        }
    }
    extendable_u[u] = false;
}

void Backtrack::popUV(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v) {

}*/




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

