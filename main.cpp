#include "include/Analyzer.hpp"
#include "include/EventGenerator.hpp"
#include <iostream>

constexpr int num_of_threads = 8;
constexpr int batch_size = 100'000 / num_of_threads;

// Run an independent Pythia generator and analyzer for one worker thread.
void runWorker(const int thread_id, const int total_events,
               Analyzer &analyzer) {
  EventGenerator eg;
  if (!eg.init(2212, 2212, "MinimumBias", 13600.0, thread_id)) {
    return;
  }

  eg.generate(analyzer, total_events);
}

int main() {
  TH1::AddDirectory(kFALSE);

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
  std::cout << "Scaling results\n";

  for (size_t i = 1; i < thread_analyzer.size(); i++) {
    thread_analyzer[0] += thread_analyzer[i];
  }

  thread_analyzer[0].saveToFile("analysis.root");

  return 0;
}
