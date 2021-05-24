/**
 * @file main.cc
 *
 */

#include "backtrack.h"
#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include <ctime>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: ./program <data graph file> <query graph file> "
                 "<candidate set file>\n";
    return EXIT_FAILURE;
  }

  std::string data_file_name = argv[1];
  std::string query_file_name = argv[2];
  std::string candidate_set_file_name = argv[3];

  Graph data(data_file_name);
  Graph query(query_file_name, true);
  CandidateSet candidate_set(candidate_set_file_name);

  Backtrack backtrack;
  
  //clock_t t_start = clock();

  backtrack.PrintAllMatches(data, query, candidate_set);

  //clock_t t_finish = clock();
  //std::cout << "Elapsed time: " << static_cast<double>(t_finish - t_start) / CLOCKS_PER_SEC << " sec" << std::endl;

  return EXIT_SUCCESS;
}
