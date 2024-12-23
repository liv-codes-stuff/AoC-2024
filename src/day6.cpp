/*
 * day6.cpp
 *
 *  Created on: 07.12.2024
 *      Author: liv
 */

#include "day6.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>

#include <eigen3/Eigen/Dense>

static std::vector<std::string> read_file(std::string in_file)
{
	std::vector<std::string> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
		ret.push_back(line);

	return ret;
}

Eigen::Vector2i find_guard_start_pos(const std::vector<std::string> &input)
{
	Eigen::Vector2i guard_start_pos;
	bool found = false;
	for(uint32_t x = 0; x < input[0].size(); x++)
	{
		for(uint32_t y = 0; y < input.size(); y++)
		{
			if(input[y][x] == '^')
			{
				guard_start_pos = Eigen::Vector2i(x, y);

				found = true;
				break;
			}
		}

		if(found)
			break;
	}

	return guard_start_pos;
}

std::pair<std::vector<std::string>, bool> walk_guard(std::vector<std::string> input, Eigen::Vector2i guard_start_pos)
{
	Eigen::Vector2i guard_act_pos = guard_start_pos;
	Eigen::Vector2i guard_act_dir(0,-1);

	uint32_t timeout = 10000;
	while(true)
	{
		// loop while guard is in the field

		// walk forward (one step)

		// first check for obstacles
		Eigen::Vector2i pos_to_check = guard_act_pos + guard_act_dir;

		if(pos_to_check(0) >= (int32_t) input[0].size() || pos_to_check(1) >= (int32_t) input.size() || pos_to_check(0) < 0 || pos_to_check(1) < 0)
		{
			// guard will walk out of space, exit loop
			break;
		}
		else if(input[pos_to_check(1)][pos_to_check(0)] == '#')
		{
			// obstacle, turn right
			const Eigen::Matrix2i rot_mat_90deg {{0, -1}, {1, 0}};

			guard_act_dir = rot_mat_90deg * guard_act_dir;
		}
		else
		{
			// space, so actually walk forward and mark that
			guard_act_pos += guard_act_dir;
			input[guard_act_pos(1)][guard_act_pos(0)] = 'X';
		}

		timeout--;

		if(timeout == 0)
			break;
	}

	bool endless_loop = false;
	if(timeout == 0)
		endless_loop = true;

	return std::make_pair(input, endless_loop);
}

static uint32_t task1(std::vector<std::string> input)
{
	// first find the guards start
	Eigen::Vector2i guard_start_pos = find_guard_start_pos(input);
	// mark this position right away
	input[guard_start_pos(1)][guard_start_pos(0)] = 'X';

	auto walking = walk_guard(input, guard_start_pos);

	input = walking.first;

	// now count the X in the array
	std::vector<uint32_t> res;
	std::transform(input.cbegin(), input.cend(), std::back_inserter(res), [](const std::string &in)->uint32_t
	{
		return std::count(in.cbegin(), in.cend(), 'X');
	});
	return std::reduce(res.cbegin(), res.cend());
}

static uint32_t task2(std::vector<std::string> input)
{
	// first find the guards start
	Eigen::Vector2i guard_start_pos = find_guard_start_pos(input);

	uint32_t loop_cnt = 0;

	// for all points that are not start pos and not obstacle try to put a new obstacle there
	for(uint32_t x = 0; x < input[0].size(); x++)
	{
		for(uint32_t y = 0; y < input.size(); y++)
		{
			if((guard_start_pos(0) == (int32_t) x && guard_start_pos(1) == (int32_t) y) || (input[y][x] == '#'))
			{
				continue;
			}
			else
			{
				// temporarily add the obstacle
				input[y][x] = '#';

				auto walking = walk_guard(input, guard_start_pos);

				if(walking.second)
					loop_cnt++;

				// remove obstacle for next try
				input[y][x] = '.';
			}
		}
	}

	return loop_cnt;
}

void day6()
{
	std::vector<std::string> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day6_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}


