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

    // Output Functions
    void PrintAllMatches(const Graph &data, const Graph &query,
                        const CandidateSet &cs);
    void PrintPath(std::vector<VertexPair> partial_embedding);
    void PrintEmbedding(std::vector<VertexPair> partial_embedding);

    // Backtrack Functions
    void DoBacktrack(const Graph& graph, const Graph& query, const CandidateSet& cs,
                      std::vector<VertexPair>& partial_embedding);
    void pushUV(std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v, const Graph& data, const Graph& query, const CandidateSet& cs);
    void popUV(std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v, const Graph& query);

    // Finder Functions
    Vertex FindRoot(const Graph& query, const CandidateSet& cs);
    Vertex FindNextVertex(size_t level);

    // Debugger Functions
    void Check(const Graph& data, const Graph& query, const CandidateSet& cs,
                      std::vector<VertexPair>& partial_embedding);
    std::pair<size_t, size_t> FinalSummary();
    void printExtendable();
    void RootAnalysis(const Graph& data, const Graph& query, const CandidateSet& cs, Vertex root);

    // Metric Functions
    double BranchFactor(size_t level, Vertex u, double ratio);
    Vertex GetInssaPower(const Graph& query, Vertex u);

  private:
    // Checks if u, v is visited(in partial_embedding)
    std::vector<bool> visited_u;
    std::vector<bool> visited_v;
    //   level       u      isExtendable
    std::vector<std::vector<bool>> extendable_u; // 모든 u에 대해 extendable인지 boolean
    //   level       u           extendable_v
    std::vector<std::vector<std::vector<Vertex>>> extendable_v; // u에 대해 extendable한 v만 저장
    std::vector<size_t> inssaPower;
    std::vector<size_t> frequency;

    int query_size;
    size_t embedding_number = 0;
    size_t recursion_call = 0;
};

#endif  // BACKTRACK_H_