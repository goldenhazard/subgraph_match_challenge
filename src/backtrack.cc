/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include "math.h"

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

void PrintEmbedding(std::vector<VertexPair> partial_embedding){
    std::cout << "[PATH] level : " << partial_embedding.size() << " embedding : ";
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
        //Check(data, query, cs, partial_embedding);
        PrintPath(partial_embedding);
        return;
    }

    else if(partial_embedding.empty()){
        Vertex root = FindRoot(query, cs);
        for(size_t idx = 0; idx < cs.GetCandidateSize(root); idx++){
            Vertex next_vertex = cs.GetCandidate(root, idx);
            partial_embedding.push_back(std::make_pair(root, next_vertex));
            visited_v[next_vertex] = true; visited_u[root] = true;
            DoBacktrack(data, query, cs, partial_embedding);
            partial_embedding.pop_back();
            visited_v[next_vertex] = false; visited_u[root] = false;
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
        /*Vertex root = partial_embedding.back().first + 1;
        for(size_t idx = 0; idx < cs.GetCandidateSize(root); idx++){
            Vertex next_vertex = cs.GetCandidate(root, idx);
            partial_embedding.push_back(std::make_pair(root, next_vertex));
            visited_v[next_vertex] = true; visited_u[root] = true;
            DoBacktrack(data, query, cs, partial_embedding);
            partial_embedding.pop_back();
            visited_v[next_vertex] = false; visited_u[root] = false;
        }*/
    }
}

Vertex Backtrack::FindRoot(const Graph& query, const CandidateSet& cs) {
    Vertex root = 0;
    //size_t min_neighbors = query.GetDegree(0);
    size_t min_neighbors = cs.GetCandidateSize(0);
    for(size_t v=1;v<query.GetNumVertices();v++) {
        if(cs.GetCandidateSize(v) < min_neighbors) {
            root = v;
            min_neighbors = cs.GetCandidateSize(v);
        }
    }
    return Vertex(root);
}

Cmu& Backtrack::FindNextVertex(const Graph& data, const Graph& query, const CandidateSet& cs, std::vector<VertexPair>& partial_embedding, Cmu& cmu_next) {
    // Not using DAG
    std::set<Vertex> cmu;
    std::set<Vertex> neighbors;
    std::vector<Vertex> not_connected;
    Vertex embedded_v;

    size_t theta = size_t(query.GetNumVertices() - log2(query.GetNumVertices()));

    // q means query.size() && p means partial_embedding.size()
    // If p < q - log2(q) find neighbor by adding set
    // time complexity : q log(q)
    if(partial_embedding.size() <= theta) {
        for(VertexPair p : partial_embedding) {
            neighbors = query.GetNeighborSets(neighbors, p.first);
        }
    }
    // If p > q - log2(q) finding neighbor by comparing every member in partial_embedding
    // time complexity : (q-p) * p
    else {
        for(size_t u=0;u<query.GetNumVertices();u++) {
            if(visited_u[u]) continue;
            for(auto p : partial_embedding) {
                if(query.IsNeighbor(p.first, u)) {
                    neighbors.insert(u);
                    break;
                }
            }
        }
    }

    /*std::cout << "[Neighbors]";
    for(Vertex u : neighbors) std::cout << " " << u;
    std::cout << std::endl;*/

    for(Vertex u : neighbors) {
        if(visited_u[u]) continue;
        cmu.clear();
        cmu = cs.GetCandidateSet(cmu, u);
        /*std::cout << "[b_B(k)] " << u << " " << cmu.size();
        for(Vertex v : cmu) std::cout << " " << v;
        std::cout << std::endl;*/
        for(VertexPair p : partial_embedding) {
            not_connected.clear();
            if(!query.IsNeighbor(p.first, u)) continue;
            if(cmu.size() == 0) break;
            embedded_v = p.second;
            for(Vertex curr_v : cmu) {
                if(visited_v[curr_v] || !data.IsNeighbor(embedded_v, curr_v)) not_connected.push_back(curr_v);
                /*if(visited_v[curr_v]) {
                    std::cout << "[NOT_CONNECTED] " << curr_v << " is visited" << std::endl;
                }
                if(!data.IsNeighbor(embedded_v, curr_v)) {
                    std::cout << "[NOT_CONNECTED] " << embedded_v << "," << curr_v << " isn't connected while "
                                << p.first << "," << u << " is connected" << std::endl;
                }*/
            }
            for(Vertex v: not_connected) cmu.erase(v);
        }

        // Find possible u that has minimum |C_m(u)|
        /*std::cout << "[c_M(u)] " << u << " " << cmu.size();
        for(Vertex v : cmu) std::cout << " " << v;
        std::cout << std::endl;*/
        if(!cmu.empty()) {
            if(cmu_next.second.size() == 0) {
                cmu_next = std::make_pair(u,cmu);
            }
            if(cmu.size() == 1) {
                cmu_next.first = u;
                cmu_next.second = cmu;
                break;
            }
            // CHANGE HERE TO CHANGE CRITERION!!!!!!!!!!
            if(cmu.size() < cmu_next.second.size()) {
                cmu_next.first = u;
                cmu_next.second = cmu;
            }
        }
    }

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
            if((query.IsNeighbor(u1, u2) && !data.IsNeighbor(v1, v2)) || (v1 == v2)) {
                /*std::cout << "u[" << u1 << "],u[" << u2 << "]=" << query.IsNeighbor(u1, u2)
                    << " v[" << v1 << "],v[" << v2 << "]=" << data.IsNeighbor(v1, v2) << std::endl;*/
                std::cout << "[F] ";
                return;
            }
        }
    }
    std::cout << "[T] ";
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