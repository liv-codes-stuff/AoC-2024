/*
 * day1.cpp
 *
 *  Created on: 01.12.2024
 *      Author: liv
 */

#include "day1.h"

#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>
#include <map>

#include <boost/algorithm/string.hpp>

static std::pair<std::vector<uint32_t>, std::vector<uint32_t>> read_file(std::string in_file)
{
	std::ifstream file(in_file);

	std::pair<std::vector<uint32_t>, std::vector<uint32_t>> ret;
	for(std::string line; std::getline(file, line);)
	{
		std::vector<std::string> two_numbers;
		boost::split(two_numbers, line, boost::is_space(), boost::token_compress_on);

		ret.first.push_back(std::stoul(two_numbers[0]));
		ret.second.push_back(std::stoul(two_numbers[1]));
	}

	return ret;
}

static uint32_t task1(std::pair<std::vector<uint32_t>, std::vector<uint32_t>> lists)
{
	std::sort(lists.first.begin(), lists.first.end());
	std::sort(lists.second.begin(), lists.second.end());

	return std::transform_reduce(lists.first.begin(), lists.first.end(), lists.second.begin(),
								 0, std::plus{}, [](uint32_t a, uint32_t b)->uint32_t
								 { return std::fabs(static_cast<float>(a) - b); });
}

static uint32_t task2(std::pair<std::vector<uint32_t>, std::vector<uint32_t>> lists)
{
	std::map<uint32_t, uint32_t> right_counts;

	std::for_each(lists.second.cbegin(), lists.second.cend(), [&](uint32_t elm)
	{
		right_counts[elm]++;
	});

	uint32_t res = 0;
	std::for_each(lists.first.cbegin(), lists.first.cend(), [&](uint32_t elm)
	{
		res += elm * right_counts[elm];
	});

	return res;
}

void day1()
{
	std::pair<std::vector<uint32_t>, std::vector<uint32_t>> lists;

	auto t1 = std::chrono::steady_clock::now();

	lists = read_file("day1_real_input.txt");

	uint32_t task1_res = task1(lists);
	uint32_t task2_res = task2(lists);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
