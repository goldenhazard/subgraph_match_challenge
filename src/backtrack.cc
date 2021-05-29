/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "math.h"

#define NOT_FIND 99999

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
    inssaPower.resize(query_size);
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

    std::cout << "Recursion_call: " << recursion_call  << " Embedding_number: " << embedding_number << std::endl;
}

void Backtrack::PrintPath(std::vector<VertexPair> partial_embedding){
    // Sort according to the pair.first
    std::sort(partial_embedding.begin(), partial_embedding.end(), VertexCompare);
    std::cout << "[" << embedding_number << "]";
    std::cout << "a";
    for(auto pair: partial_embedding){
        std::cout << " ";
        //std::cout << "(" << pair.first << ",";
        std::cout << pair.second;
        //std::cout << ")";
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

void Backtrack::DoBacktrack(const Graph& data, const Graph& query, const CandidateSet& cs, 
                            std::vector<VertexPair>& partial_embedding){
    //PrintEmbedding(partial_embedding);
    //printExtendable();
    recursion_call += 1;
    //if(recursion_call % 1000000 == 0) std::cout << recursion_call << std::endl;
    //if(embedding_number >= 100000) return;
    if(partial_embedding.size() == query.GetNumVertices()){
        //Check(data, query, cs, partial_embedding);
        embedding_number += 1;
        PrintPath(partial_embedding);
        return;
    }

    else if(partial_embedding.empty()){
        Vertex root = FindRoot(query, cs);
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

Vertex Backtrack::FindRoot(const Graph& query, const CandidateSet& cs) {
    Vertex root = 0;
    double max = double(cs.GetCandidateSize(0)) / query.GetDegree(0);
    for(int v=1;v<query_size;v++) {
        //std::cout << "[CS SIZE] " << v << " " << cs.GetCandidateSize(v) << std::endl;
        if(double(cs.GetCandidateSize(v)) / query.GetDegree(v) < max) {
            root = v;
            max = double(cs.GetCandidateSize(v)) / query.GetDegree(v);
        }
    }
    std::cout << "Root is : " << root << std::endl;
    root = 0;
    return root;
}

double Backtrack::BranchFactor(size_t level, Vertex u, double ratio){
    // Returns the average frequency vector of each
    // Ratio is hyperparameter
    int branch_prob = 0;
    int num = 0;
    for(auto v_cand : extendable_v[level][u]){
        num++;
        branch_prob += frequency[v_cand];
    }
    return (double)branch_prob/num;
}



Vertex Backtrack::FindNextVertex(size_t level) {
    Vertex returnVertex = NOT_FIND;
    double min_metric = NOT_FIND;

    for(int u=0;u<query_size;u++) {
        if(extendable_u[level][u]) {
            if(extendable_v[level][u].size() == 0) continue;
            auto metric = (double)extendable_v[level][u].size();
            if(metric < min_metric) {
                returnVertex = u;
                min_metric = metric;
            }
        }
    }
    //std::cout << "Next vertex is " << returnVertex << std::endl;
    return returnVertex;
}

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

void Backtrack::pushUV(std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v, const Graph& data, const Graph& query, const CandidateSet& cs) {
    partial_embedding.push_back(std::make_pair(u, v));
    int level = partial_embedding.size();
    //std::cout << "[LEVEL] " << level << " (" << u << "," << v << ")" << std::endl;
    for(int w=0;w<query_size;w++) {
        extendable_v[level][w].clear();
        // w == u : 할게 없다
        // w가 visited면 : 할게 없다
        // w가 extendable_u[level][w] : extendable_v[level][w]에서 for loop 돌리면서 채우기(드모르간)
        // w가 신유형이면 : getCandidateSet() 불러와서 여기서 for loop 돌리기
        if(visited_u[w] || w == u) continue;
        if(extendable_u[level-1][w]) {
            extendable_u[level][w] = true;
            for(Vertex v_cmu : extendable_v[level-1][w]) {
                if((v_cmu != v) && !visited_v[v_cmu] && (!query.IsNeighbor(w,u) || data.IsNeighbor(v_cmu,v)))
                    extendable_v[level][w].push_back(v_cmu);
            }
        } else if(query.IsNeighbor(w,u)) {
            //std::cout << w << " is new extendable neighbor!" << std::endl;
            extendable_u[level][w] = true;
            std::set<Vertex> candidate_set;
            candidate_set = cs.GetCandidateSet(candidate_set, w);
            for(Vertex v_cmu : candidate_set) {
                if((v_cmu != v) && !visited_v[v_cmu] && data.IsNeighbor(v_cmu,v))
                    extendable_v[level][w].push_back(v_cmu);
            }
        }
    }
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
    visited_v[v] = true; visited_u[u] = true;
}

void Backtrack::popUV(std::vector<VertexPair>& partial_embedding, Vertex u, Vertex v, const Graph& query) {
    for(int i=0;i<query_size;i++) extendable_u[partial_embedding.size()][i] = false;
    partial_embedding.pop_back();

    std::set<Vertex> neighbor;
    neighbor = query.GetNeighborSets(neighbor, u);
    for(Vertex u_neighbor : neighbor) {
        inssaPower[u_neighbor]--;
    }
    visited_v[v] = false; visited_u[u] = false;
}

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

std::pair<size_t, size_t> Backtrack::FinalSummary(){
    // std::cout << "Embedding #: " << embedding_number << "Recursion call: " << recursion_call << std::endl;
    return std::make_pair(embedding_number, recursion_call);
}
