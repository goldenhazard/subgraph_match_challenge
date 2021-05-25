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
    query_size = query.GetNumVertices();
    visited_u.resize(query.GetNumVertices());
    visited_v.resize(data.GetNumVertices());

    std::vector<VertexPair> matchedVertex;
    DoBacktrack(data, query, cs, matchedVertex);
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
    PrintEmbedding(partial_embedding);
    //if(embedding_number >= 100000) return;
    if(partial_embedding.size() == query.GetNumVertices()){
        //Check(data, query, cs, partial_embedding);
        embedding_number += 1;
        //PrintPath(partial_embedding);
        return;
    }

    else if(partial_embedding.empty()){
        Vertex root = FindRoot(query, cs);
        for(size_t idx = 0; idx < cs.GetCandidateSize(root); idx++){
            Vertex v_next = cs.GetCandidate(root, idx);
            partial_embedding.push_back(std::make_pair(root, v_next));
            visited_v[v_next] = true; visited_u[root] = true;
            pushUV(partial_embedding.size(), root, v_next, data, query, cs);
            DoBacktrack(data, query, cs, partial_embedding);
            popUV(root, query);
            partial_embedding.pop_back();
            visited_v[v_next] = false; visited_u[root] = false;
        }
    }

    else{
        Vertex u_next = FindNextVertex(partial_embedding.size());
        if(u_next == 99999) return;
        for(Vertex v_next: extendable_v[partial_embedding.size()][u_next]){
            partial_embedding.push_back(std::make_pair(u_next, v_next));
            visited_v[v_next] = true; visited_u[u_next] = true;
            pushUV(partial_embedding.size(), u_next, v_next, data, query, cs);
            DoBacktrack(data, query, cs, partial_embedding);
            popUV(u_next, query);
            partial_embedding.pop_back();
            visited_v[v_next] = false; visited_u[u_next] = false;
        }
    }
}

Vertex Backtrack::FindRoot(const Graph& query, const CandidateSet& cs) {
    Vertex root = 0;
    //size_t min_neighbors = query.GetDegree(0);
    size_t min_candidate = cs.GetCandidateSize(0);
    for(size_t v=1;v<query.GetNumVertices();v++) {
        //std::cout << "[CS SIZE] " << v << " " << cs.GetCandidateSize(v) << std::endl;
        if(cs.GetCandidateSize(v) < min_candidate) { // Find Root that have most candidates in query
            root = v;
            min_candidate = cs.GetCandidateSize(v);
        }
    }
    return Vertex(root);
}

Vertex Backtrack::FindNextVertex(size_t level) {
    Vertex returnVertex = 99999;
    size_t min_extendable = 10000;
    for(int u=0;u<query_size;u++) {
        if(extendable_u[level][u]) {
            if(extendable_v[level][u].size() < min_extendable) {
                returnVertex = u;
                min_extendable = extendable_v[level][u].size();
            }
        }
    }
    return returnVertex;
}

/**
 * @brief Returns the number of neighbors of v with label l.
 *
 * @param level-1 current extendable database.
 * @param u, v partial_embedding에 이제 집어널을 녀석.
 * @return extendable[level] u, v가 추가되었을 때 extendable database.
 */
void Backtrack::pushUV(size_t level, Vertex u, Vertex v, const Graph& data, const Graph& query, const CandidateSet& cs) {
    // 추가할 u의 이웃을 불러온다 inssaPower[u] = 0;
    // 이미 방문한 이웃 u_extend는 건너뛴다
    // u의 이웃을 읽을 때 인싸력을 올려준다 i.e. inssaPower[u_iterate]++;
    // 방문하지 않은 이웃에 대해
    // u_extend의 c_M(u)를 불러온다
    // partial_embedding을 읽으면서 지움
    /*std::set<Vertex> neighbor;
    neighbor = query.GetNeighborSets(neighbor, u);
    for(Vertex u_neighbor : neighbor) {
        inssaPower[u_neighbor]++;
        //extendable_v[level+1][u_neighbor].clear();
        if(!visited_u[u_neighbor]) {
            extendable_u[level+1][u_neighbor] = true;
            for(Vertex v_cmu : extendable_v[level][u_neighbor]) {
                for(VertexPair p : partial_embedding) {
                    if(!visited_v[v_cmu] && (!query.IsNeighbor(p.first, u_neighbor) || data.IsNeighbor(p.second, v_cmu))) {
                        extendable_v[level+1][u_neighbor].insert(v_cmu);
                        break;
                    }
                }
            }
        }
    }*/
    for(int w=0;w<query_size;w++) {
        extendable_v[level+1][w].clear();
        // w == u : 할게 없다
        // w가 visited면 : 할게 없다
        // w가 extendable_u[level][w] : extendable_v[level][w]에서 for loop 돌리면서 채우기(드모르간)
        // w가 신유형이면 : getCandidateSet() 불러와서 여기서 for loop 돌리기
        if(visited_u[w] || w == u) continue;
        if(extendable_u[level][w]) {
            extendable_u[level+1][w] = true;
            for(Vertex v_cmu : extendable_v[level][w]) {
                if(!query.IsNeighbor(w,u) || data.IsNeighbor(v_cmu,v)) extendable_v[level+1][w].insert(v_cmu);
            }
        } else if(query.IsNeighbor(w,u)) {
            extendable_u[level+1][w] = true;
            std::set<Vertex> candidate_set;
            candidate_set = cs.GetCandidateSet(candidate_set, w);
            for(Vertex v_cmu : candidate_set) {
                if(!query.IsNeighbor(w,u) || data.IsNeighbor(v_cmu,v)) extendable_v[level+1][w].insert(v_cmu);
            }
        }
    }
}

void Backtrack::popUV(Vertex u, const Graph& query) {
    // inssaPower만 관리해줌;
    std::set<Vertex> neighbor;
    neighbor = query.GetNeighborSets(neighbor, u);
    for(Vertex u_neighbor : neighbor) {
        inssaPower[u_neighbor]--;
    }
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