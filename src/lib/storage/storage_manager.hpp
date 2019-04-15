#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#include "lqp_view.hpp"
#include "prepared_plan.hpp"
#include "types.hpp"
#include "utils/singleton.hpp"

namespace opossum {

class Table;
class AbstractLQPNode;

// The StorageManager is a singleton that maintains all tables
// by mapping table names to table instances.
class StorageManager : public Singleton<StorageManager> {
 public:
  /**
   * @defgroup Manage Tables
   * @{
   */
  void add_table(const std::string& name, std::shared_ptr<Table> table);
  void drop_table(const std::string& name);
  std::shared_ptr<Table> get_table(const std::string& name) const;
  bool has_table(const std::string& name) const;
  std::vector<std::string> table_names() const;
  const std::map<std::string, std::shared_ptr<Table>>& tables() const;
  /** @} */

  /**
   * @defgroup Manage SQL VIEWs
   * @{
   */
  void add_view(const std::string& name, const std::shared_ptr<LQPView>& view);
  void drop_view(const std::string& name);
  std::shared_ptr<LQPView> get_view(const std::string& name) const;
  bool has_view(const std::string& name) const;
  std::vector<std::string> view_names() const;
  /** @} */

  /**
   * @defgroup Manage prepared plans - comparable to SQL PREPAREd statements
   * @{
   */
  void add_prepared_plan(const std::string& name, const std::shared_ptr<PreparedPlan>& prepared_plan);
  std::shared_ptr<PreparedPlan> get_prepared_plan(const std::string& name) const;
  bool has_prepared_plan(const std::string& name) const;
  void drop_prepared_plan(const std::string& name);
  /** @} */

  // prints information about all tables in the storage manager (name, #columns, #rows, #chunks)
  void print(std::ostream& out = std::cout) const;

  // deletes the entire StorageManager and creates a new one, used especially in tests
  // This can lead to a lot of issues if there are still running tasks / threads that
  // want to access a resource. You should be very sure that this is what you want.
  // Have a look at base_test.hpp to see the correct order of resetting things.
  static void reset();

  // For debugging purposes mostly, dump all tables as csv
  void export_all_tables_as_csv(const std::string& path);

  StorageManager(StorageManager&&) = delete;

 protected:
  StorageManager() {}

  friend class Singleton;

  const StorageManager& operator=(const StorageManager&) = delete;
  StorageManager& operator=(StorageManager&&) = default;

  // Tables can currently not be modified concurrently
  std::map<std::string, std::shared_ptr<Table>> _tables;

  // The map of views is locked because views are created dynamically, e.g., in TPC-H 15
  std::map<std::string, std::shared_ptr<LQPView>> _views;
  mutable std::unique_ptr<std::shared_mutex> _view_mutex = std::make_unique<std::shared_mutex>();

  std::map<std::string, std::shared_ptr<PreparedPlan>> _prepared_plans;
};
}  // namespace opossum
