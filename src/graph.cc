/**
 * @file graph.cc
 *
 */

#include "graph.h"

namespace {
std::vector<Label> transferred_label;
void TransferLabel(const std::string &filename) {
  /*
  Constructs transferred_label from label_set
  (Label jump solved(?))
  e.g)
    label set = {0,1,2,5,8}
    transferred_label = [0|1|2|0|0|3|0|0|4]
  */
  std::ifstream fin(filename);

  if (!fin.is_open()) {
    std::cout << "Graph file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;
  int32_t graph_id_;
  size_t num_vertices_;

  std::set<Label> label_set;

  fin >> type >> graph_id_ >> num_vertices_;

  // preprocessing
  while (fin >> type) {
    if (type == 'v') {
      Vertex id;
      Label l;
      fin >> id >> l;

      label_set.insert(l);
    } else if (type == 'e') {
      Vertex v1, v2;
      Label l;
      fin >> v1 >> v2 >> l;
    }
  }

  fin.close();

  transferred_label.resize(
      *std::max_element(label_set.begin(), label_set.end()) + 1, -1);

  Label new_label = 0;
  for (Label l : label_set) {
    transferred_label[l] = new_label;
    new_label += 1;
  }
}
}  // namespace

Graph::Graph(const std::string &filename, bool is_query) {
  // If data graph, transfer label
  if (!is_query) {
    TransferLabel(filename);
  }
  // Adjacency List
  // <<2,3,4,5,6>, <7,8>, ... >
  std::vector<std::vector<Vertex>> adj_list;

  // Load Graph
  std::ifstream fin(filename);
  std::set<Label> label_set;

  if (!fin.is_open()) {
    std::cout << "Graph file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;

  fin >> type >> graph_id_ >> num_vertices_;

  adj_list.resize(num_vertices_);

  start_offset_.resize(num_vertices_ + 1);
  label_.resize(num_vertices_);

  num_edges_ = 0;

  // preprocessing
  // label : len(vertexes) array
  // label_set : set of labels
  // adj_list
  while (fin >> type) {
    if (type == 'v') {
      Vertex id;
      Label l;
      fin >> id >> l;

      if (static_cast<size_t>(l) >= transferred_label.size())
        l = -1;
      else
        l = transferred_label[l];

      label_[id] = l;
      label_set.insert(l);
    } else if (type == 'e') {
      Vertex v1, v2;
      Label l;
      fin >> v1 >> v2 >> l;

      adj_list[v1].push_back(v2);
      adj_list[v2].push_back(v1);

      num_edges_ += 1;
    }
  }

  fin.close();

  adj_array_.resize(num_edges_ * 2);

  num_labels_ = label_set.size();

  max_label_ = *std::max_element(label_set.begin(), label_set.end());

  label_frequency_.resize(max_label_ + 1);

  start_offset_by_label_.resize(num_vertices_ * (max_label_ + 1));

  // start_offset : adjacency array start index
  start_offset_[0] = 0;
  for (size_t i = 0; i < adj_list.size(); ++i) {
    start_offset_[i + 1] = start_offset_[i] + adj_list[i].size();
  }

  // iterte by all vertices
  for (size_t i = 0; i < adj_list.size(); ++i) {
    label_frequency_[GetLabel(i)] += 1;

    auto &neighbors = adj_list[i];

    if (neighbors.size() == 0) continue;

    // sort neighbors by ascending order of label first, and descending order of
    // degree second
    std::sort(neighbors.begin(), neighbors.end(), [this](Vertex u, Vertex v) {
      if (GetLabel(u) != GetLabel(v))
        return GetLabel(u) < GetLabel(v);
      else if (GetDegree(u) != GetDegree(v))
        return GetDegree(u) > GetDegree(v);
      else
        return u < v;
    });

    Vertex v = neighbors[0];
    Label l = GetLabel(v);

    start_offset_by_label_[i * (max_label_ + 1) + l].first = start_offset_[i];

    for (size_t j = 1; j < neighbors.size(); ++j) {
      v = neighbors[j];
      Label next_l = GetLabel(v);

      if (l != next_l) {
        start_offset_by_label_[i * (max_label_ + 1) + l].second =
            start_offset_[i] + j;
        start_offset_by_label_[i * (max_label_ + 1) + next_l].first =
            start_offset_[i] + j;
        l = next_l;
      }
    }

    start_offset_by_label_[i * (max_label_ + 1) + l].second =
        start_offset_[i + 1];

    std::copy(adj_list[i].begin(), adj_list[i].end(),
              adj_array_.begin() + start_offset_[i]);
  }
}

std::vector<Vertex>& Graph::GetNeighborVertexes(std::vector<Vertex>& neighbors, Vertex v) const{
  for(auto idx = GetNeighborStartOffset(v); idx < GetNeighborEndOffset(v); idx++)
    neighbors.push_back(adj_array_[idx]);
  return neighbors;
}

std::set<Vertex>& Graph::GetNeighborSets(std::set<Vertex>& neighbors, Vertex v) const{
  for(auto idx = GetNeighborStartOffset(v); idx < GetNeighborEndOffset(v); idx++)
    neighbors.insert(adj_array_[idx]);
  return neighbors;
}

Graph::~Graph() {}

std::ostream& operator<<(std::ostream& os, Graph& graph){
  os << "Graph Summary" << std::endl;
  os << "======================" << std::endl;
  os << graph.num_vertices_ << " vertices," << graph.num_edges_ << " edges," << graph.num_labels_ << " labels" << std::endl;
  os << "Frequency" << std::endl;
  for(auto frequency : graph.label_frequency_) os << frequency << " ";
  os << std::endl;
  os << "offset" << std::endl;
  for(auto offset : graph.start_offset_) os << offset << " ";
  os << std::endl;
  os << "Label" << std::endl;
  for(Label label: graph.label_) os << label << " ";
  os << std::endl;
  return os;
}