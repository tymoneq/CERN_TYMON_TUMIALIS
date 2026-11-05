#include "include/Analyzer.hpp"
#include "include/EventGenerator.hpp"
#include <TROOT.h>
#include <iostream>
#include <unistd.h>

// Run an independent Pythia generator and analyzer for one worker thread.
void runWorker(const int thread_id, const int total_events,
               Analyzer &analyzer) {
  EventGenerator eg;
  if (!eg.init(2212, 2212, "MinimumBias", 13600.0, thread_id)) {
    std::cerr << "[Critical Error] thread " << std::this_thread::get_id()
              << ": pythia.init() ended with error" << std::endl;
    throw std::runtime_error("Pythia initialization failed in thread");
  }

  eg.generate(analyzer, total_events);
}

void usage() {

  std::cout << "Usage -n [number of simulations] -t [number of threads]"
            << std::endl;
}

int main(int argc, char *argv[]) {

  int num_of_threads = 1;
  int total_size = 100'000;

  int opt;
  while ((opt = getopt(argc, argv, "v:t:")) != -1) {
    switch (opt) {
    case 'n':
      total_size = strtol(optarg, (char **)NULL, 10);
      break;
    case 't':
      num_of_threads = strtol(optarg, (char **)NULL, 10);
      break;
    default:
      break;
    }
  }

  int batch_size = (total_size + (num_of_threads - 1)) / num_of_threads;

  TH1::AddDirectory(kFALSE);
  ROOT::EnableThreadSafety();

  // Keep histogram state separate per thread, then merge after joining.
  std::vector<Analyzer> thread_analyzer(num_of_threads);
  std::vector<std::thread> threads_jobs;
  threads_jobs.reserve(num_of_threads);

  std::cout << "Starting generating events\n";

  for (size_t i = 0; i < thread_analyzer.size(); i++) {
    threads_jobs.emplace_back(runWorker, i, batch_size,
                              std::ref(thread_analyzer[i]));
  }

  // Wait until every worker has finished before combining its histograms.
  for (auto &thread : threads_jobs) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  // Combine worker histograms and write the final ROOT file.
  std::cout << "Merging results\n";

  for (size_t i = 1; i < thread_analyzer.size(); i++) {
    thread_analyzer[0] += thread_analyzer[i];
  }

  thread_analyzer[0].saveToFile("analysis.root");

  return 0;
}
