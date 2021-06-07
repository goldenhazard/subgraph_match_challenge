/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "math.h"

#define NOT_FIND 999999

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
    query_size = query.GetNumVertices();
    visited_u.resize(query_size);
    visited_v.resize(data.GetNumVertices());
    extendable_u.resize(query_size+1);
    extendable_v.resize(query_size+1);
    for(int i=0;i<=query_size;i++) {
        extendable_u[i].resize(query_size);
        extendable_v[i].resize(query_size);
    }

    frequency.resize(data.GetNumVertices());
    // Building frequency vector
    for(auto u : cs.cs_){
        for(auto cs_u : u)
            frequency[cs_u]++;
    }

    std::vector<VertexPair> matchedVertex;
    DoBacktrack(data, query, cs, matchedVertex);
}

void Backtrack::RootAnalysis(const Graph& data, const Graph& query, const CandidateSet& cs, Vertex root){
    std::cout << "==================================================" << std::endl;
    std::cout << "Root Analysis[" << root << "]" << std::endl;
    std::cout << "CMU size:" << cs.GetCandidateSize(root) << std::endl;
    std::cout << "Degree in query: " << query.GetDegree(root) << std::endl;
    int degree_in_data = 0;
    for(size_t i = 0; i < cs.GetCandidateSize(root); i++){
        Vertex v = cs.GetCandidate(root, i);
        degree_in_data += data.GetDegree(v);
    }
    std::cout << "Degree in data: " << degree_in_data << std::endl;
    std::cout << "==================================================" << std::endl;
}
    

void Backtrack::DoBacktrack(const Graph& data, const Graph& query, const CandidateSet& cs, 
                            std::vector<VertexPair>& partial_embedding){
    //if(recursion_call % 10000 == 0) PrintEmbedding(partial_embedding);
    //printExtendable();
    //recursion_call += 1;
    //if(recursion_call % 1000000 == 0) std::cout << "Recursion_call: " << recursion_call  << " Embedding_number: " << embedding_number << std::endl;
    if(embedding_number >= 100000) return;
    //if(recursion_call >= 2e6 && embedding_number == 0) return;

    if(partial_embedding.size() == query.GetNumVertices()){
        //Check(data, query, cs, partial_embedding);
        embedding_number += 1;
        PrintPath(partial_embedding);
        return;
    }

    else if(partial_embedding.empty()){
        Vertex root = FindRoot(query, cs);
        // root = 0;
        //RootAnalysis(data, query, cs, root);
        for(size_t idx = 0; idx < cs.GetCandidateSize(root); idx++){
            Vertex v_next = cs.GetCandidate(root, idx);
            pushUV(partial_embedding, root, v_next, data, query, cs);
            DoBacktrack(data, query, cs, partial_embedding);
            popUV(partial_embedding, root, v_next, query);
        }
    }

    else{
        Vertex u_next = FindNextVertex(partial_embedding.size());
        if(u_next == NOT_FIND) return;
        for(Vertex v_next: extendable_v[partial_embedding.size()][u_next]){
            pushUV(partial_embedding, u_next, v_next, data, query, cs);
            DoBacktrack(data, query, cs, partial_embedding);
            popUV(partial_embedding, u_next, v_next, query);
        }
    }
}

void Backtrack::pushUV(std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v, const Graph& data, const Graph& query, const CandidateSet& cs) {
    /*
    Update dynamic programming table extendable_u, extendable_v according to recently pushed (u, v)

    Argument:
        u, v = newly pushed (u, v).
        partial_embedding = vector of matched (u, v) vertex.
        data, query, cs = objects that we are backtracking on.

    Claim:
        We can optimize updating DP table only by comparing with the new vertex u.

    Procedure:
        1) Add new pair(u, v) to partial_embedding
        2) Update visited check array
        3) Iterates through all query vertexes "w" and update DP table's next-level.
            i) If w is already in partial_embedding (visited_u[w]), not in our interest
            ii) If w is already in u_extendable(level-1), update by De-Morgan trick
            iii) If w is not in u_extendable(level-1), update by comparing only with new vertex (u, v).
    
    Time complexity:
        O(query_size) + O(|extendable_v|(level - 1)) + O(|candidate size(u)|)
    */
    
    // Update partial_embedding, visited array
    partial_embedding.push_back(std::make_pair(u, v));
    visited_v[v] = true; visited_u[u] = true;

    int level = partial_embedding.size();
    
    for(int w=0;w<query_size;w++) {
        // If w is already_visited, none of our interest, so continue
        if(visited_u[w]) continue;

        // If w is u-extendable in DP[level - 1],
        if(extendable_u[level-1][w]) {
            extendable_u[level][w] = true; // w is u-extendable in DP[level]
            // Iterate through v-extendable vertexes in DP[level - 1]
            for(Vertex v_cmu : extendable_v[level-1][w]) {
                // If such vertex is still extendable comparing with new (u, v), push_back to v-extendable in DP[level]
                if(!visited_v[v_cmu] && (!query.IsNeighbor(w,u) || data.IsNeighbor(v_cmu,v)))
                    extendable_v[level][w].push_back(v_cmu);
            }
        }

        // If w is not u-extendable in DP[level - 1], but is neighbor with new u 
        else if(query.IsNeighbor(w,u)) {
            extendable_u[level][w] = true; // w is u-extendable in DP[level]
            // *We don't have to compare with already visited u => Optimization! 
            // If such vertex is extendable comparing with new (u, v), push_back to v-extendable in DP[level]
            for(size_t i = 0; i < cs.GetCandidateSize(w); i++){
                Vertex v_cmu = cs.GetCandidate(w, i);
                if(!visited_v[v_cmu] && data.IsNeighbor(v_cmu,v))
                    extendable_v[level][w].push_back(v_cmu);
            }
        }
    }
}

void Backtrack::popUV(std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v, const Graph& query) {
    /*
    Update dynamic programming table extendable_u, extendable_v according to recently popeed (u, v)
    Argument:
        u, v = newly popped (u, v).
        partial_embedding = vector of matched (u, v) vertex.
        data, query, cs = objects that we are backtracking on.

    Claim:
        We can optimize updating DP table only by comparing with the new vertex u.

    Procedure:
        1) Add new pair(u, v) to partial_embedding
        2) Update visited check array
        3) Iterates through all query vertexes "w" and update DP table's next-level.
            i) If w is already in partial_embedding (visited_u[w]), not in our interest
            ii) If w is already in u_extendable(level-1), update by De-Morgan trick
            iii) If w is not in u_extendable(level-1), update by comparing only with new vertex (u, v).
    
    Time Complexity:
        O(query_size)
    */
    int level = partial_embedding.size();
    for(int w=0;w<query_size;w++){
        extendable_u[level][w] = false;
        // When we are done with current level, clear all the logs of previous backtrack: takes O(1)
        // extendable_v[level][w].clear();
        if(!extendable_v[level][w].empty()) extendable_v[level][w].clear();
    }
    partial_embedding.pop_back();
    visited_v[v] = false; visited_u[u] = false;
}

/* 
Finder Function
*/
Vertex Backtrack::FindRoot(const Graph& query, const CandidateSet& cs) {
    Vertex root = 0;
    size_t min_candidate = cs.GetCandidateSize(0);
    for(size_t v=0;v<query.GetNumVertices();v++) {
        if(cs.GetCandidateSize(v) == 1) {
            root = v;
            break;
        }
        if(cs.GetCandidateSize(v) < min_candidate) {
            root = v;
            min_candidate = cs.GetCandidateSize(v);
        }
    }
    //std::cout << "Root is : " << root << std::endl;
    return Vertex(root);
}

Vertex Backtrack::FindNextVertex(size_t level) {
    Vertex returnVertex = NOT_FIND;
    double min_metric = NOT_FIND;
    // double max_branch_metric = -1;

    for(int u=0;u<query_size;u++) {
        if(extendable_u[level][u]) {
            if(extendable_v[level][u].size() == 0) return NOT_FIND;
            auto metric = (double)extendable_v[level][u].size();
            if(metric < min_metric) {
                returnVertex = u;
                min_metric = metric;
            }
            if(min_metric == 1) break;
        }
    }
    //std::cout << "Next vertex is " << returnVertex << std::endl;
    return returnVertex;
}

/*
Metric Function
*/
double Backtrack::BranchFactor(size_t level, Vertex u, double ratio){
    // Returns the average frequency vector of each
    // Ratio is hyperparameter
    int branch_prob = 0;
    int num = 0;
    for(auto v_cand : extendable_v[level][u]){
        num++;
        branch_prob += frequency[v_cand];
    }
    return (double)branch_prob/num * ratio;
}


/*
Checker Function
    Argument: check if (u, v) pair in partial_embedding is valid.
*/
void Backtrack::Check(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding) {
    for(int i=0;i<int(query.GetNumVertices());i++) {
        for(int j=i+1;j<int(query.GetNumVertices());j++) {
            Vertex u1, u2, v1, v2;
            u1 = partial_embedding[i].first;
            u2 = partial_embedding[j].first;
            v1 = partial_embedding[i].second;
            v2 = partial_embedding[j].second;
            if((query.IsNeighbor(u1, u2) && !data.IsNeighbor(v1, v2)) || (v1 == v2)) {
                /*std::cout << "u[" << u1 << "],u[" << u2 << "]=" << query.IsNeighbor(u1, u2)
                    << " v[" << v1 << "],v[" << v2 << "]=" << data.IsNeighbor(v1, v2) << std::endl;*/
                std::cout << "[F] ";
                return;
            }
        }
    }
    std::cout << "[T] ";
    //PrintPath(partial_embedding);
}

/*
Output Functions
    1) PrintPath : Print path according to Assignment's spec
    2) PrintEmbedding
*/

void Backtrack::PrintPath(std::vector<VertexPair> partial_embedding){
    // Sort according to the pair.first
    std::sort(partial_embedding.begin(), partial_embedding.end(), VertexCompare);
    //std::cout << "[" << embedding_number << "]";
    std::cout << "a";
    for(auto pair: partial_embedding){
        std::cout << " ";
        std::cout << pair.second;
    }
    std::cout << std::endl;
}

void Backtrack::PrintEmbedding(std::vector<VertexPair> partial_embedding){
    std::cout << "[PATH] level : " << partial_embedding.size() << " embedding : ";
    std::cout << "(" << recursion_call << ")";
    std::cout << "a";
    for(auto pair: partial_embedding){
        std::cout << " ";
        std::cout << "(" << pair.first << ",";
        std::cout << pair.second;
        std::cout << ")";
    }
    std::cout << std::endl;
}


/*
Debugger Functions
    1) printExtendable(): print DP table of extendable_u and extendable_v
    2) Final Summary(): Prints every dataset's matching results
*/

void Backtrack::printExtendable() {
    std::cout << "{Every Extendable}" << std::endl;
    for(int level=0;level<=query_size;level++) {
        std::cout << "level " << level;
        std::cout << " extendable u :";
        for(int w=0;w<query_size;w++) if(extendable_u[level][w]) std::cout << " " << w;
        std::cout << std::endl;
        for(int w=0;w<query_size;w++) {
            if(extendable_v[level][w].size() > 0) {
                std::cout << "extendable u,v " << w << " " << extendable_v[level][w].size();
                for(Vertex v_cmu : extendable_v[level][w]) std::cout << " " << v_cmu;
                std::cout << std::endl;
            }
        }
    }
}

std::pair<size_t, size_t> Backtrack::FinalSummary(){
    // std::cout << "Embedding #: " << embedding_number << "Recursion call: " << recursion_call << std::endl;
    return std::make_pair(embedding_number, recursion_call);
}




// Extendable Debugging
    /*std::cout << "[EXTENDABLE]";
    for(int w=0;w<query_size;w++) if(extendable_u[level][w]) std::cout << " " << w;
    std::cout << std::endl;
    for(int w=0;w<query_size;w++) {
        if(extendable_v[level][w].size() > 0) {
            std::cout << "[EXTENDABLE] " << w << " " << extendable_v[level][w].size();
            for(Vertex v_cmu : extendable_v[level][w]) std::cout << " " << v_cmu;
            std::cout << std::endl;
        }
    }*/
