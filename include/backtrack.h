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
    void PrintPath(std::vector<VertexPair> partial_embedding);

    void DoBacktrack(const Graph& graph, const Graph& query, const CandidateSet& cs,
                      std::vector<VertexPair>& partial_embedding);
    
    Vertex FindRoot(const Graph& query, const CandidateSet& cs);
    
    Cmu& FindNextVertex(const Graph& data, const Graph& query, const CandidateSet& cs,
                      std::vector<VertexPair>& partial_embedding, Cmu& cmu_next);

    std::vector<Cmu>& BuildCmuHeap(const Graph& data, const Graph& query, const CandidateSet& cs, 
                      std::vector<VertexPair>& partial_embedding, std::vector<Cmu>& cmu_heap);
    Vertex NextVertexToExtend(std::vector<Cmu>& cmu_heap);
    Vertex FindVByU(std::vector<VertexPair>& partial_embedding, Vertex u);
    Vertex FindUByV(std::vector<VertexPair>& partial_embedding, Vertex u);
    //void pushU();
    //void popU();
    //void pushUV();
    //void popUV();

    // Debugger Functions
    void Check(const Graph& data, const Graph& query, const CandidateSet& cs,
                      std::vector<VertexPair>& partial_embedding);
    std::pair<size_t, size_t> FinalSummary();

  private:
    std::vector<bool> visited_u;
    std::vector<bool> visited_v;
    std::vector<bool> extendable_u;
    std::vector<std::vector<Vertex>> extendable_v;
    size_t embedding_number = 0;
    size_t recursion_call = 0;
};

#endif  // BACKTRACK_H_



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////