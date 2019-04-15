#pragma once

#include <atomic>
#include "sql/sql_pipeline.hpp"

#include "benchmark_config.hpp"

namespace opossum {

// Stores the result of ALL runs of a single benchmark item (e.g., TPC-H query 5).
struct BenchmarkItemResult {
  BenchmarkItemResult();

  // Need to explicitly implement move, because std::atomic implicitly deletes it...
  BenchmarkItemResult(BenchmarkItemResult&& other) noexcept;
  BenchmarkItemResult& operator=(BenchmarkItemResult&& other);

  std::atomic<size_t> num_iterations = 0;

  // Used only for Ordered mode
  uint64_t all_runs_duration_ns{0};

  // Holds one entry per execution of this item. The vector holds one entry per SQL pipeline executed as part of this
  // item. For benchmarks like TPC-H, where each item corresponds to a single TPC-H query, this vector always has a
  // size of 1. For others, like TPC-C, there are multiple SQL queries executed and thus multiple entries in the inner
  // vector.
  tbb::concurrent_vector<std::vector<SQLPipelineMetrics>> metrics;

  // Set if verification was requested and this item was executed
  std::optional<bool> verification_passed;
};

}  // namespace opossum
