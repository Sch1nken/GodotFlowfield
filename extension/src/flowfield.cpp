// © Copyright 2014-2022, Juan Linietsky, Ariel Manzur and the Godot community
// (CC-BY 3.0)
#include "flowfield.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Flowfield::Flowfield() {}

Flowfield::~Flowfield() {}

const std::vector<Vector2i> Flowfield::get_neighbours(const Vector2i &cell) {
  return std::vector<Vector2i>{UP + cell, DOWN + cell, LEFT + cell,
                               RIGHT + cell};
}

const std::vector<Vector2i>
Flowfield::get_neighbours_visited(const Vector2i &cell) {
  const std::vector<Vector2i> neighbours = get_neighbours(cell);
  std::vector<Vector2i> out;

  for (int i = 0; i < 4; i++) {
    if(!is_within_map_bounds(neighbours[i])) {
      continue;
    }

    if (visited_vec[neighbours[i].y + neighbours[i].x * map_size.y] == FLOWFIELD_INACCESSIBLE) {
      continue;
    }

    out.push_back(neighbours[i]);
  }

  return out;
}

bool Flowfield::is_within_map_bounds(const Vector2i &cell) {
  return cell.x >= 0 && cell.y >= 0 && cell.x < map_size.x &&
         cell.y < map_size.y;
}

void Flowfield::calculate_integration_field_test(const Vector2i& cell) {
  is_valid_test = false;
  visited_vec_test.resize(map_size.x * map_size.y);
  queue_dict_vec_test.resize(map_size.x * map_size.y);
  shortest_path_test.resize(map_size.x * map_size.y);

  std::fill(visited_vec_test.begin(), visited_vec_test.end(), FLOWFIELD_INACCESSIBLE);
  std::fill(queue_dict_vec_test.begin(), queue_dict_vec_test.end(), 0);
  std::fill(shortest_path_test.begin(), shortest_path_test.end(), 0);

  non_empty_test = non_empty;
  non_empty_test[cell.y+cell.x*map_size.y] = 1;

  visited_vec_test[end_cell.y + end_cell.x * map_size.y] = 0;

  int queue_index = 0;
  int queue_index_last = 0;
  int queue_size = map_size.x * map_size.y;
  std::vector<Vector2i> queue_arr;

  Vector2i cur_cell;

  queue_arr.resize(queue_size);
  queue_arr[queue_index] = end_cell;

  queue_dict_vec_test[end_cell.y + end_cell.x * map_size.y] = 1;

  int i = 1;
  int end_cost = 0;

  while (queue_index <= queue_index_last) {
    i++;
    cur_cell = queue_arr[queue_index];
    queue_index++;

    std::vector<Vector2i> neighbours = get_neighbours(cur_cell);

    for (int i = 0; i < 4; i++) {
      Vector2i cur_neighbour = neighbours[i];
      bool is_walkable =
          is_within_map_bounds(cur_neighbour) && non_empty_test[cur_neighbour.y+cur_neighbour.x*map_size.y] == 0;

      if (!is_walkable) {
        continue;
      }

      end_cost = visited_vec_test[cur_cell.y + cur_cell.x * map_size.y];

      if (end_cost >=
          visited_vec_test[cur_neighbour.y + cur_neighbour.x * map_size.y]) {
        continue;
      }

      if (queue_dict_vec_test[cur_neighbour.y + cur_neighbour.x * map_size.y] == 0) {
        queue_dict_vec_test[cur_neighbour.y + cur_neighbour.x * map_size.y] = 1;
        queue_index_last = queue_index_last + 1;
        queue_arr[queue_index_last] = cur_neighbour;
      }

      visited_vec_test[cur_neighbour.y + cur_neighbour.x * map_size.y] =
          end_cost + 1;
    }
  }

  if(visited_vec_test[start_cell.y+start_cell.x*map_size.y] == FLOWFIELD_INACCESSIBLE) {
    return;
  }

  is_valid_test = true;
}

void Flowfield::calculate_integration_field() {
  visited_vec.resize(map_size.x * map_size.y);
  queue_dict_vec.resize(map_size.x * map_size.y);
  non_empty.resize(map_size.x * map_size.y);
  cell_costs.resize(map_size.x * map_size.y);
  shortest_path.resize(map_size.x * map_size.y);

  std::fill(visited_vec.begin(), visited_vec.end(), INT32_MAX);
  std::fill(queue_dict_vec.begin(), queue_dict_vec.end(), 0);
  std::fill(cell_costs.begin(), cell_costs.end(), 0);
  std::fill(shortest_path.begin(), shortest_path.end(), 0);

  visited_vec[end_cell.y + end_cell.x * map_size.y] = 0;

  int queue_index = 0;
  int queue_index_last = 0;
  int queue_size = map_size.x * map_size.y;
  std::vector<Vector2i> queue_arr;

  Vector2i cur_cell;

  queue_arr.resize(queue_size);
  queue_arr[queue_index] = end_cell;

  queue_dict_vec[end_cell.y + end_cell.x * map_size.y] = 1;

  int i = 1;
  int end_cost = 0;

  while (queue_index <= queue_index_last) {
    i++;
    cur_cell = queue_arr[queue_index];
    queue_index++;

    std::vector<Vector2i> neighbours = get_neighbours(cur_cell);

    for (int i = 0; i < 4; i++) {
      Vector2i cur_neighbour = neighbours[i];
      bool is_walkable =
          is_within_map_bounds(cur_neighbour) && non_empty[cur_neighbour.y+cur_neighbour.x*map_size.y] == 0;

      if (!is_walkable) {
        continue;
      }

      end_cost = visited_vec[cur_cell.y + cur_cell.x * map_size.y];

      if (end_cost >=
          visited_vec[cur_neighbour.y + cur_neighbour.x * map_size.y]) {
        continue;
      }

      if (queue_dict_vec[cur_neighbour.y + cur_neighbour.x * map_size.y] == 0) {
        queue_dict_vec[cur_neighbour.y + cur_neighbour.x * map_size.y] = 1;
        queue_index_last = queue_index_last + 1;
        queue_arr[queue_index_last] = cur_neighbour;
      }

      visited_vec[cur_neighbour.y + cur_neighbour.x * map_size.y] =
          end_cost + 1;
    }
  }

  if(visited_vec[start_cell.y+start_cell.x*map_size.y] == FLOWFIELD_INACCESSIBLE) {
    return;
  }

  calculate_shortest_path();
  calculate_dir_field();

  valid = true;
}

void Flowfield::calculate_dir_field() {
  dir_field_vec.resize(map_size.x*map_size.y);

  std::fill(dir_field_vec.begin(), dir_field_vec.end(), 0);

  // 0 = invalid
  // 1 = RIGHT
  // 2 = DOWN
  // 3 = LEFT
  // 4 = UP

  for(int y = 0; y < map_size.y; y++) {
    for(int x = 0; x < map_size.x; x++) {
      if(visited_vec[y+x*map_size.y] == FLOWFIELD_INACCESSIBLE) {
        continue;
      }

      Vector2i smallest = get_smallest_neighbour(Vector2i(x, y));
      int value = 0;
      
      if(Vector2i(x, y) + RIGHT == smallest) {
        value = 1;
      }
      else if(Vector2i(x, y) + DOWN == smallest) {
        value = 2;
      }
      else if(Vector2i(x, y) + LEFT == smallest) {
        value = 3;
      }
      else if(Vector2i(x, y) + UP == smallest) {
        value = 4;
      }

      dir_field_vec[y+x*map_size.y] = value;
    }
  }
}

inline const Vector2i Flowfield::get_smallest_neighbour(const Vector2i &cell) {
  Vector2i smallest = cell;
  std::vector<Vector2i> neighbours_visited = get_neighbours_visited(cell);

  for(const auto& it: neighbours_visited) {
    if(visited_vec[it.y+it.x*map_size.y] < visited_vec[smallest.y+smallest.x*map_size.y]) {
      smallest = it;
    }
  }

  return smallest;
}

bool Flowfield::is_on_shortest_path(const Vector2i &cell) {
  return shortest_path[cell.y+cell.x*map_size.y] == 1;
}

void Flowfield::calculate_shortest_path() {
  Vector2i current = start_cell;
  Vector2i next = current;
  do
  {
    current = next;
    shortest_path[current.y+current.x*map_size.y] = 1;
    next = get_smallest_neighbour(current);
  } while(current != next);
}

void Flowfield::add_non_empty(const Vector2i &cell) {
  non_empty[cell.y+cell.x*map_size.y] = NON_EMPTY;
}

void Flowfield::remove_non_empty(const Vector2i &cell) {
  non_empty[cell.y+cell.x*map_size.y] = EMPTY;
}

inline bool Flowfield::is_empty(const Vector2i &cell) {
  if(!is_within_map_bounds(cell))
  {
    return false;
  }

  return non_empty[cell.y+cell.x*map_size.y] == EMPTY;
}

int Flowfield::can_build(const Vector2i &cell) {
  if(!is_valid()) {
    return CAN_BUILD::NO_FLOWFIELD_INVALID;
  }

  if(!is_within_map_bounds(cell)) {
    return CAN_BUILD::NO_OUT_OF_BOUNDS;
  }

  if(cell == start_cell) {
    return CAN_BUILD::NO_IS_START;
  }

  if(cell == end_cell) {
    return CAN_BUILD::NO_IS_END;
  }

  // If not empty
  if(!is_empty(cell)) {
    return CAN_BUILD::NO_NOT_EMPTY;
  }

  // Is not part of flowfield
  if(visited_vec[cell.y+cell.x*map_size.y] == FLOWFIELD_INACCESSIBLE) {
    return CAN_BUILD::YES;
  }


  // Is in empty 3x3 grid or only one cell non_empty
  int is_three_grid = 0;
  for(int i = 0; i < 8; i++) {
    if(is_empty(cell + three_grid[i])) {
      is_three_grid++;
    }
  }

  if(is_three_grid >= 7) {
    return CAN_BUILD::YES;
  }

  std::vector<Vector2i> neighbours = get_neighbours_visited(cell);
  int empty_neighbours = 0;
  for(const auto &it: neighbours) {
    if(is_empty(it)) {
      empty_neighbours++;
    }
  }

  if(empty_neighbours == 1) {
    return CAN_BUILD::YES;
  }

  calculate_integration_field_test(cell);

  if(is_valid_test) {
    return CAN_BUILD::YES;
  }

  // If is on edge of map, we need 5 free spaces, corners are safe when the one diagonally inset towards the middle is free?

  // More generally: If in corner (detect if two orthogonal cells are not empty (i.e. no 0 [] 0))
  // Then we can figure out if its safe
  // IF LEFT, TOP_LEFT, TOP empty -> okay
  // IF TOP, TOP_RIGHT, RIGHT empty -> okay
  // IF RIGHT, DOWN_RIGHT, DOWN empty -> okay
  // IF DOWN, DOWN_LEFT, LEFT empty -> okay
/*
  if (is_empty(cell + LEFT) && is_empty(cell + UP_LEFT) && is_empty(cell + UP)
  || is_empty(cell + UP) && is_empty(cell + UP_RIGHT) && is_empty(cell + RIGHT)
  || is_empty(cell + RIGHT) && is_empty(cell + DOWN_RIGHT) && is_empty(cell + DOWN)
  || is_empty(cell + DOWN) && is_empty(cell + DOWN_LEFT) && is_empty(cell + LEFT)) {
    return CAN_BUILD::YES;
  }
  TODO: NOT WORKING /\

*/
  // Also for two wide pathways


  return CAN_BUILD::NO_OTHER;

  // Is not part of shortest path -> can definetly build
  /*if(shortest_path[cell.y+cell.x*map_size.y] == 0) {
    return true;
  }*/

  // Is unique and costs <= highest start cost

 

  // If a
  // -1|-1 0|-1 1|-1
  // -1|0   []  1|0
  // -1|1  0|1  1|1

  // A B C
  // D   E
  // F G H

  // D+B+H = 0|0
  // B+E+F = 0|0
  // D+G+C = 0|0
  // G+R+A = 0|0

  // X 0 X
  // 0   0
  // 0 X 0

  /*Vector2i neighbour_left = cell + LEFT;
  Vector2i neighbour_right = cell + RIGHT;
  if(visited_vec[neighbour_left.y+neighbour_left.x*map_size.y] == 0 && visited_vec[neighbour_right.y+neighbour_right.x*map_size.y] == 0) {
    return false;
  }

  Vector2i neighbour_up = cell + UP;
  Vector2i neighbour_down = cell + DOWN;
  if(visited_vec[neighbour_up.y+neighbour_up.x*map_size.y] == 0 && visited_vec[neighbour_down.y+neighbour_down.x*map_size.y] == 0) {
    return false;
  }*/

  // Check triangle configurations + mirrored ones
  // Check highest cell cost

  /*if(cell_costs[cell.y+cell.x*map_size.y] == 1) {
    return false;
  }*/

  return true;
}

int Flowfield::get_map_width() { return map_size.x; }

int Flowfield::get_map_height() { return map_size.y; }

void Flowfield::set_map_size(const Vector2i &size, const Vector2i& start, const Vector2i &end) {
  map_size = size;
  visited_vec.resize(map_size.x * map_size.y);
  queue_dict_vec.resize(map_size.x * map_size.y);
  non_empty.resize(map_size.x * map_size.y);
  cell_costs.resize(map_size.x * map_size.y);
  shortest_path.resize(map_size.x * map_size.y);

  this->start_cell = start;
  this->end_cell = end;

  valid = false;
}

bool Flowfield::is_valid() { return valid; }

Array Flowfield::get_data() {
  Array a;
  a.resize(map_size.x * map_size.y);
  for (int i = 0; i < a.size(); i++) {
    a[i] = visited_vec[i];
  }

  return a;
}

Array Flowfield::get_dir_data() {
  Array a;
  a.resize(map_size.x * map_size.y);
  for (int i = 0; i < a.size(); i++) {
    a[i] = dir_field_vec[i];
  }

  return a;
}

void Flowfield::_bind_methods() {

  // Get
  ClassDB::bind_method(D_METHOD("is_on_shortest_path", "cell"), &Flowfield::is_on_shortest_path);
  ClassDB::bind_method(D_METHOD("is_valid"), &Flowfield::is_valid);
  ClassDB::bind_method(D_METHOD("get_data"), &Flowfield::get_data);
  ClassDB::bind_method(D_METHOD("get_dir_data"), &Flowfield::get_dir_data);
  ClassDB::bind_method(D_METHOD("get_map_width"), &Flowfield::get_map_width);
  ClassDB::bind_method(D_METHOD("get_map_height"), &Flowfield::get_map_height);
  ClassDB::bind_method(D_METHOD("can_build"), &Flowfield::can_build);
  ClassDB::bind_method(D_METHOD("is_within_map_bounds", "cell"), &Flowfield::is_within_map_bounds);

  // Do
  ClassDB::bind_method(D_METHOD("calculate_integration_field"),
                       &Flowfield::calculate_integration_field);

  // Set
  ClassDB::bind_method(D_METHOD("set_map_size", "size", "start", "end"),
                       &Flowfield::set_map_size);
  ClassDB::bind_method(D_METHOD("add_non_empty", "cell"), &Flowfield::add_non_empty);
  ClassDB::bind_method(D_METHOD("remove_non_empty", "cell"), &Flowfield::remove_non_empty);

  // Constants
  // TODO -> Flowfield::FLOWFIELD_INACCESSIBLE use that
  ClassDB::bind_integer_constant("Flowfield", "", "FLOWFIELD_INACCESSIBLE", INT32_MAX);
}