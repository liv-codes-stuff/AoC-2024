/*
 * day15.cpp
 *
 *  Created on: 15.12.2024
 *      Author: liv
 */

#include "day15.h"

#include <iostream>
#include <chrono>
#include <utility>
#include <vector>
#include <string>
#include <fstream>

#include <eigen3/Eigen/Dense>

static std::pair<std::vector<std::string>, std::string> read_file(std::string in_file)
{
	std::pair<std::vector<std::string>, std::string> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		// next after the empty line are the movements
		if(line.size() < 1)
			break;

		ret.first.push_back(line);
	}

	for(std::string line; std::getline(file, line);)
	{
		ret.second += line;
	}

	return ret;
}

static std::vector<std::string> convert_for_part2(const std::vector<std::string> &in_map)
{
	std::vector<std::string> ret;

	for(auto line : in_map)
	{
		std::string ret_line;
		for(auto sym : line)
		{
			if(sym == '#')
				ret_line += "##";
			else if(sym == 'O')
				ret_line += "[]";
			else if(sym == '.')
				ret_line += "..";
			else if(sym == '@')
				ret_line += "@.";
			else
				throw std::exception();
		}
		ret.push_back(ret_line);
	}

	return ret;
}

static Eigen::Vector2i find_robot_start_pos(const std::pair<std::vector<std::string>, std::string> &input)
{
	Eigen::Vector2i start_pos;
	bool found = false;
	for(uint32_t x = 0; x < input.first[0].size(); x++)
	{
		for(uint32_t y = 0; y < input.first.size(); y++)
		{
			if(input.first[y][x] == '@')
			{
				start_pos = Eigen::Vector2i(x, y);

				found = true;
				break;
			}
		}

		if(found)
			break;
	}

	return start_pos;
}

static bool push_if_possible(std::vector<std::string> &map, Eigen::Vector2i pos_to_check, Eigen::Vector2i direction)
{
	Eigen::Vector2i new_pos = pos_to_check + direction;
	char pos_item = map[pos_to_check.y()][pos_to_check.x()];
	if(pos_item == 'O')
	{
		// probably pushable, keep checking
		if(push_if_possible(map, new_pos, direction))
		{
			// pushable so move into the direction
			map[new_pos.y()][new_pos.x()] = pos_item;
			map[pos_to_check.y()][pos_to_check.x()] = '.'; // delete item from here

			return true;
		}
		else
			return false;
	}
	else if(pos_item == '.')
	{
		// nothing to push but possible
		return true;
	}
	else
	{
		// cannot push
		return false;
	}
}

static bool push_big_boxes_unsafe(std::vector<std::string> &map, Eigen::Vector2i pos_to_check, Eigen::Vector2i direction)
{
	// this function checks the second part of the box after it pushed the sub-boxes after. So don't execute this function
	// without checking if the pushing is actually possible

	Eigen::Vector2i new_pos = pos_to_check + direction;
	char pos_item = map[pos_to_check.y()][pos_to_check.x()];
	if(pos_item == '[')
	{
		// probably pushable, keep checking
		if(push_big_boxes_unsafe(map, new_pos, direction))
		{
			// move other part of the box too if direction is up or down (otherwise it works anyway)
			if(direction.y() != 0)
			{
				Eigen::Vector2i new_other_half_pos = new_pos + Eigen::Vector2i{1,0};
				Eigen::Vector2i pos_to_check_other_half_pos = pos_to_check + Eigen::Vector2i{1,0};
				if(push_big_boxes_unsafe(map, new_other_half_pos, direction))
				{
					// pushable so move into the direction
					map[new_pos.y()][new_pos.x()] = pos_item;
					map[pos_to_check.y()][pos_to_check.x()] = '.'; // delete item from here

					map[new_other_half_pos.y()][new_other_half_pos.x()] = ']';
					map[pos_to_check_other_half_pos.y()][pos_to_check_other_half_pos.x()] = '.'; // delete item from here

					return true;
				}
				else
					return false;
			}
			else
			{
				map[new_pos.y()][new_pos.x()] = pos_item;
				map[pos_to_check.y()][pos_to_check.x()] = '.'; // delete item from here

				return true;
			}
		}
		else
			return false;
	}
	else if(pos_item == ']')
	{
		// probably pushable, keep checking
		if(push_big_boxes_unsafe(map, new_pos, direction))
		{
			// move other part of the box too if direction is up or down (otherwise it works anyway)
			if(direction.y() != 0)
			{
				Eigen::Vector2i new_other_half_pos = new_pos + Eigen::Vector2i{-1,0};
				Eigen::Vector2i pos_to_check_other_half_pos = pos_to_check + Eigen::Vector2i{-1,0};
				if(push_big_boxes_unsafe(map, new_other_half_pos, direction))
				{
					// pushable so move into the direction
					map[new_pos.y()][new_pos.x()] = pos_item;
					map[pos_to_check.y()][pos_to_check.x()] = '.'; // delete item from here

					map[new_other_half_pos.y()][new_other_half_pos.x()] = '[';
					map[pos_to_check_other_half_pos.y()][pos_to_check_other_half_pos.x()] = '.'; // delete item from here

					return true;
				}
				else
					return false;
			}
			else
			{
				map[new_pos.y()][new_pos.x()] = pos_item;
				map[pos_to_check.y()][pos_to_check.x()] = '.'; // delete item from here

				return true;
			}
		}
		else
			return false;
	}
	else if(pos_item == '.')
	{
		// nothing to push but possible
		return true;
	}
	else
	{
		// cannot push
		return false;
	}
}

static bool check_push_possible_big_boxes(std::vector<std::string> &map, Eigen::Vector2i pos_to_check, Eigen::Vector2i direction)
{
	Eigen::Vector2i new_pos = pos_to_check + direction;
	char pos_item = map[pos_to_check.y()][pos_to_check.x()];
	if(pos_item == '[')
	{
		// probably pushable, keep checking
		if(check_push_possible_big_boxes(map, new_pos, direction))
		{
			// move other part of the box too if direction is up or down (otherwise it works anyway)
			if(direction.y() != 0)
			{
				Eigen::Vector2i new_other_half_pos = new_pos + Eigen::Vector2i{1,0};
				if(check_push_possible_big_boxes(map, new_other_half_pos, direction))
				{
					return true;
				}
				else
					return false;
			}
			else
			{
				return true;
			}
		}
		else
			return false;
	}
	else if(pos_item == ']')
	{
		// probably pushable, keep checking
		if(check_push_possible_big_boxes(map, new_pos, direction))
		{
			// move other part of the box too if direction is up or down (otherwise it works anyway)
			if(direction.y() != 0)
			{
				Eigen::Vector2i new_other_half_pos = new_pos + Eigen::Vector2i{-1,0};
				if(check_push_possible_big_boxes(map, new_other_half_pos, direction))
				{
					return true;
				}
				else
					return false;
			}
			else
			{
				return true;
			}
		}
		else
			return false;
	}
	else if(pos_item == '.')
	{
		// nothing to push but possible
		return true;
	}
	else
	{
		// cannot push
		return false;
	}
}

static bool push_possible_big_boxes(std::vector<std::string> &map, Eigen::Vector2i pos_to_check, Eigen::Vector2i direction)
{
	if(check_push_possible_big_boxes(map, pos_to_check, direction))
	{
		push_big_boxes_unsafe(map, pos_to_check, direction);
		return true;
	}
	else
		return false;
}

static Eigen::Vector2i get_direction(char symbol)
{
	if(symbol == '^')
		return Eigen::Vector2i{0,-1};
	else if(symbol == 'v')
		return Eigen::Vector2i{0, 1};
	else if(symbol == '<')
		return Eigen::Vector2i{-1, 0};
	else if(symbol == '>')
		return Eigen::Vector2i{1, 0};
	else
		throw std::exception();
}

static uint32_t calculate_gps_position_sum(const std::vector<std::string> &in)
{
	uint32_t ret = 0;

	for(uint32_t x = 0; x < in[0].size(); x++)
	{
		for(uint32_t y = 0; y < in.size(); y++)
		{
			if(in[y][x] == 'O' || in[y][x] == '[')
			{
				ret += 100  * y + x;
			}
		}
	}

	return ret;
}

static uint32_t task1(std::pair<std::vector<std::string>, std::string> input)
{
	Eigen::Vector2i start_pos = find_robot_start_pos(input);

	Eigen::Vector2i act_pos = start_pos;

	for(auto it = input.second.cbegin(); it != input.second.cend(); it++)
	{
		Eigen::Vector2i dir = get_direction(*it);

		if(push_if_possible(input.first, act_pos+dir, dir))
		{
			// move robot
			Eigen::Vector2i new_pos = act_pos + dir;

			input.first[new_pos.y()][new_pos.x()] = '@';
			input.first[act_pos.y()][act_pos.x()] = '.'; // delete robot from here

			act_pos += dir;
		}
	}

	return calculate_gps_position_sum(input.first);
}

static uint32_t task2(std::pair<std::vector<std::string>, std::string> input)
{
	input.first = convert_for_part2(input.first);

	Eigen::Vector2i start_pos = find_robot_start_pos(input);

	Eigen::Vector2i act_pos = start_pos;

	for(auto it = input.second.cbegin(); it != input.second.cend(); it++)
	{
		Eigen::Vector2i dir = get_direction(*it);

		if(push_possible_big_boxes(input.first, act_pos+dir, dir))
		{
			// move robot
			Eigen::Vector2i new_pos = act_pos + dir;

			input.first[new_pos.y()][new_pos.x()] = '@';
			input.first[act_pos.y()][act_pos.x()] = '.'; // delete robot from here

			act_pos += dir;
		}
	}

	return calculate_gps_position_sum(input.first);
}

void day15()
{
	std::pair<std::vector<std::string>, std::string> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day15_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
