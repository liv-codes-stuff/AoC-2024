/*
 * day9.cpp
 *
 *  Created on: 10.12.2024
 *      Author: liv
 */

#include "day9.h"

#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

static std::vector<int32_t> read_file(std::string in_file)
{
	std::vector<int32_t> ret;

	std::ifstream file(in_file);
	std::string line;
	std::getline(file, line);

	bool now_file = true;
	uint32_t file_num = 0;
	for(auto it = line.cbegin(); it != line.cend(); it++)
	{
		uint32_t num = std::stoul(std::string(1, *it));

		if(now_file)
		{
			for(uint32_t i = 0; i < num; i++)
			{
				ret.push_back(file_num);
			}
			file_num++;
		}
		else
		{
			for(uint32_t i = 0; i < num; i++)
			{
				ret.push_back(-1);
			}
		}

		now_file ^= true;
	}

	return ret;
}

std::vector<int32_t> compact_array(const std::vector<int32_t> &in)
{
	std::vector<int32_t> ret = in;

	for(auto it = ret.begin(); it < ret.end(); it++)
	{
		if(*it == -1)
		{
			// free space, move most right file piece to this space

			// first delete last item in vector while its just free space
			while(ret[ret.size()-1] == -1)
				ret.pop_back();

			if(it < ret.end())
			{
				int32_t file = ret[ret.size()-1];
				ret.pop_back();

				*it = file;
			}
		}
	}

	// trim end
	while(ret[ret.size()-1] == -1)
		ret.pop_back();

	return ret;
}

std::vector<int32_t> compact_nofrag_array(const std::vector<int32_t> &in)
{
	std::vector<int32_t> ret = in;

	// search for the first number from the end
	for(auto it = ret.rbegin(); it != ret.rend();)
	{
		// get end of number
		auto next_it = std::find_if_not(it, ret.rend(), [&](auto item)->bool {return item == *it;});

		// length of number -1* since its reverse iterators
		auto dist = -1 * std::distance(next_it, it);

		// now search for a free space in ret for this from left
		std::vector<int32_t> free_space(dist, -1);
		auto search_it = std::search(ret.begin(), ret.end(), free_space.begin(), free_space.end());

		if(search_it != ret.end())
		{
			// check if the free space is actually before our number
			if(std::distance(ret.begin(), std::next(it).base()) > std::distance(ret.begin(), search_it))
			{
				// move the number there and free the old space
				std::copy(it, next_it, search_it);
				std::fill(it, next_it, -1);
			}
		}
		// continue after the old position of the number
		it = next_it;
		// skip empty space
		while(*it == -1)
			it++;
	}

	return ret;
}

uint64_t checksum_filesystem(const std::vector<int32_t> &in)
{
	uint64_t ret = 0;

	for(uint64_t i = 0; i < in.size(); i++)
	{
		if(in[i] != -1)
			ret += i * in[i];
	}
	return ret;
}

static uint64_t task1(const std::vector<int32_t> &in)
{
	auto defrg_arr = compact_array(in);

	return checksum_filesystem(defrg_arr);
}

static uint64_t task2(const std::vector<int32_t> &in)
{
	auto defrg_arr = compact_nofrag_array(in);

	return checksum_filesystem(defrg_arr);
}

void day9()
{
	std::vector<int32_t> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("paula_day9_realinput.txt");

	uint64_t task1_res = task1(input);
	uint64_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
