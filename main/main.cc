/**
 * @file main.cc
 *
 */

#include "backtrack.h"
#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include <time.h>

void test_all(){
  std::vector<std::string> data_file_names;
  data_file_names.push_back("lcc_hprd");
  data_file_names.push_back("lcc_human"); 
  data_file_names.push_back("lcc_yeast");

  std::vector<std::string> query_types;
  query_types.push_back("n1"); query_types.push_back("n3");
  query_types.push_back("n5"); query_types.push_back("n8");
  query_types.push_back("s1"); query_types.push_back("s3"); query_types.push_back("s5"); query_types.push_back("s8");

  for(auto data_file : data_file_names){
    for(auto query_type : query_types){
      std::string data_file_name = "../data/" + data_file + ".igraph";
      std::string query_file_name = "../query/" + data_file + "_" + query_type + ".igraph";
      std::string candidate_set_file_name = "../candidate_set/" + data_file + "_" + query_type + ".cs";

      std::cout << "Loading " << data_file_name << "|" << query_file_name << "|" << candidate_set_file_name << std::endl;
      Graph data(data_file_name);
      Graph query(query_file_name, true);
      CandidateSet candidate_set(candidate_set_file_name);

      Backtrack backtrack;

      std::cout << "[Loading complete]" << std::endl;

      auto start = clock();
      backtrack.PrintAllMatches(data, query, candidate_set);
      auto end = clock();
      auto summary_pair = backtrack.FinalSummary();
      std::cout << "Embedding #: " << summary_pair.first << " Recursion_call #:" << summary_pair.second << std::endl;
      std::cout << "Time Elapsed: " << end - start << "[ms]" << std::endl;
      std::cout << "=======================================" << std::endl;
    }
  }
}


int main(int argc, char* argv[]) {
  std::string mode = argv[1];
  if(mode == "test_all"){
    test_all();
    return EXIT_SUCCESS;
  }

  if (argc < 4) {
    std::cerr << "Usage: ./program <data graph file> <query graph file> "
                 "<candidate set file>\n";
    return EXIT_FAILURE;
  }

  std::string data_file_name = argv[1];
  //std::cout << "Data file name: " << data_file_name << std::endl;
  std::string query_file_name = argv[2];
  std::string candidate_set_file_name = argv[3];

  Graph data(data_file_name);
  Graph query(query_file_name, true);
  CandidateSet candidate_set(candidate_set_file_name);

  Backtrack backtrack;
  
  clock_t t_start = clock();

  backtrack.PrintAllMatches(data, query, candidate_set);

  clock_t t_finish = clock();
  std::cout << "Elapsed time: " << static_cast<double>(t_finish - t_start) / CLOCKS_PER_SEC << " sec" << std::endl;

  return EXIT_SUCCESS;
}
