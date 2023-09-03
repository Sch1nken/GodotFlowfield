#ifndef FLOWFIELD_CLASS_H
#define FLOWFIELD_CLASS_H

#ifdef WIN32
#include <windows.h>
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <godot_cpp/classes/ref.hpp>

#include <stdint.h>

// Figure out positions where towers can definetly or definetly not be placed.

// WHen placing a tower would split a path, traverse from both neighbours down (dir).
// Traverse both in tandem until
// a) they meet up -> nice, we have another way
// b) nope, no tower here I'm afraid?
// -> one side will never diverge since it would have to count up

// Another idea is to detect loops in undirected graphs (https://www.tutorialspoint.com/Detect-Cycle-in-a-an-Undirected-Graph)
// by only treating entrances/exits of single-wide paths as vertices?
// or this:
// Now, to detect a cycle, we can adjust DFS’s logic a bit: If v_n has a visited neighbor v_{n'} that:

//    is equal to v_n, or
//    is not equal to v_n‘s parent


// CAN be placed:
// - 3 by 3 is empty
//   0  0  0
//   0 [ ] 0
//   0  0  0
// - If cell is not part of flow_field and NOT in non_empty
//   0  X  0
//   x [ ] X
//   0  X  0
// - Can always build if not part of shortest path
// - Can always build if only one neighbour (dead end)

// CANNOT be placed:
// - If is NOT within map_bounds
// - If orthogonally adjacent cells are non_empty (X [ ] X)
// - If something like this:
//      0  X  0
//      X [ ] N
//      0  N  N
// - x = either tower or out of bounds
// - If tower cell cost is unique AND NOT If cell costs is higher than closest to start
// - If flowfield recalculation cant finish (last resort, slow)

// FALLBACK
// If enemies are trapped, destroy last placed tower
// If at least one enemie has not valid flow_field cell

using namespace godot;

class Flowfield : public RefCounted {
  GDCLASS(Flowfield, RefCounted);

  const Vector2i UP{0, -1};
  const Vector2i DOWN{0, 1};
  const Vector2i LEFT{-1, 0};
  const Vector2i RIGHT{1, 0};

  const Vector2i UP_LEFT{-1, -1};
  const Vector2i UP_RIGHT{1, -1};

  const Vector2i DOWN_LEFT{-1, 1};
  const Vector2i DOWN_RIGHT{1, 1};

  const Vector2i three_grid[8] = {UP_LEFT, UP, UP_RIGHT, LEFT, RIGHT, DOWN_LEFT, DOWN, DOWN_RIGHT};
  const Vector2i dirs[5] = {Vector2i(0, 0), RIGHT, DOWN, LEFT, UP};

  std::vector<int> visited_vec_test;
  std::vector<int> queue_dict_vec_test;
  std::vector<int> dir_field_vec_test;

  std::vector<int> visited_vec;
  std::vector<int> queue_dict_vec;
  std::vector<int> dir_field_vec;

  std::vector<int> cell_costs;
  std::vector<int> non_empty;
  // TODO: See if it works now, then optimize by only checking the additional cell in test
  // No need to copy over the whole array
  std::vector<int> non_empty_test;

  std::vector<int> shortest_path;
  std::vector<int> shortest_path_test;

  const int FLOWFIELD_MAX = INT32_MAX;
  const int FLOWFIELD_INACCESSIBLE = INT32_MAX;
  const int EMPTY = 0;
  const int NON_EMPTY = 1;

  int highest_end_cost = 0;

  bool valid = false;
  bool is_valid_test = false;

  enum CAN_BUILD {
    YES,
    NO_FLOWFIELD_INVALID,
    NO_IS_START,
    NO_IS_END,
    NO_OUT_OF_BOUNDS,
    NO_NOT_EMPTY,
    NO_INACCESSIBLE,
    NO_OTHER
  };


  Vector2i map_size{1, 1};
  Vector2i start_cell;
  Vector2i end_cell;

protected:
  static void _bind_methods();

public:
  Flowfield();
  ~Flowfield();

  int get_map_width();
  int get_map_height();

  void add_non_empty(const Vector2i &cell);
  void remove_non_empty(const Vector2i &cell);

  int can_build(const Vector2i &cell);
  inline bool is_empty(const Vector2i &cell);

  const std::vector<Vector2i> get_neighbours(const Vector2i &cell);
  const std::vector<Vector2i> get_neighbours_visited(const Vector2i &cell);

  inline const Vector2i get_smallest_neighbour(const Vector2i &cell);
  bool is_within_map_bounds(const Vector2i &cell);
  bool is_on_shortest_path(const Vector2i &cell);
  void calculate_integration_field();
  void calculate_integration_field_test(const Vector2i &cell);
  void calculate_shortest_path();
  void calculate_dir_field();

  void set_map_size(const Vector2i &size, const Vector2i& start, const Vector2i& end);

  bool is_valid();

  Array get_data();
  Array get_dir_data();
};

#endif // Flowfield_CLASS_H