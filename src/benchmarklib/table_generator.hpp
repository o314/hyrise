#pragma once

#include <cmath>
#include <memory>
#include <optional>

#include "storage/encoding_type.hpp"
#include "types.hpp"

namespace opossum {

class Table;

enum class DataDistributionType { Uniform, NormalSkewed, Pareto };

struct ColumnDataDistribution {
  static ColumnDataDistribution make_uniform_config(double min, double max) {
    ColumnDataDistribution c{};
    c.min_value = min;
    c.max_value = max;
    c.num_different_values = static_cast<int>(std::floor(max - min));
    return c;
  }

  static ColumnDataDistribution make_pareto_config(double pareto_scale = 1.0, double pareto_shape = 1.0) {
    ColumnDataDistribution c{};
    c.pareto_scale = pareto_scale;
    c.pareto_shape = pareto_shape;
    c.distribution_type = DataDistributionType::Pareto;
    return c;
  }

  static ColumnDataDistribution make_skewed_normal_config(double skew_location = 0.0, double skew_scale = 1.0,
                                                          double skew_shape = 0.0) {
    ColumnDataDistribution c{};
    c.skew_location = skew_location;
    c.skew_scale = skew_scale;
    c.skew_shape = skew_shape;
    c.distribution_type = DataDistributionType::NormalSkewed;
    return c;
  }

  DataDistributionType distribution_type = DataDistributionType::Uniform;

  int num_different_values = 1'000;

  double pareto_scale = 1.0;
  double pareto_shape = 1.0;

  double skew_location = 0.0;
  double skew_scale = 1.0;
  double skew_shape = 0.0;

  double min_value = 0.0;
  double max_value = 1.0;
};

class TableGenerator {
  // Note: numa_distribute_chunks=true only affects generated tables that use DictionaryCompression,
  // otherwise the chunks are most likely all placed on a single node. This might change in the future.
  // See the discussion here https://github.com/hyrise/hyrise/pull/402
 public:
  // Simple table generation, mainly for simple tests
  std::shared_ptr<Table> generate_table(const size_t num_columns, const size_t num_rows, const ChunkOffset chunk_size,
                                        const SegmentEncodingSpec segment_encoding_spec = {EncodingType::Unencoded});

  std::shared_ptr<Table> generate_table(const std::vector<ColumnDataDistribution>& column_data_distributions,
                                        const std::vector<DataType>& column_data_types, const size_t num_rows,
                                        const ChunkOffset chunk_size,
                                        const std::vector<SegmentEncodingSpec>& segment_encoding_specs,
                                        const bool numa_distribute_chunks = false);

  // Base function that generates the actual data
  std::shared_ptr<Table> generate_table(const std::vector<ColumnDataDistribution>& column_data_distributions,
                                        const std::vector<DataType>& column_data_types, const size_t num_rows,
                                        const ChunkOffset chunk_size, const bool numa_distribute_chunks);

 protected:
  const size_t _num_columns = 10;
  const size_t _num_rows = 40'000;
  const int _max_different_value = 10'000;
};

}  // namespace opossum
