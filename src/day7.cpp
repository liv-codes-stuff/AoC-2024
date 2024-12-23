/*
 * day7.cpp
 *
 *  Created on: 08.12.2024
 *      Author: liv
 */

#include "day7.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

#include <boost/algorithm/string.hpp>

static std::vector<std::pair<int64_t, std::vector<int64_t>>> read_file(std::string in_file)
{
	std::vector<std::pair<int64_t, std::vector<int64_t>>> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		std::pair<int64_t, std::vector<int64_t>> item;

		std::vector<std::string> result_and_rest;
		boost::split(result_and_rest, line, boost::is_any_of(":"), boost::token_compress_on);

		item.first = std::stoll(result_and_rest[0]);

		boost::trim(result_and_rest[1]);

		std::transform(boost::make_split_iterator(result_and_rest[1], boost::token_finder(boost::is_space(), boost::token_compress_on)),
				boost::split_iterator<std::string::iterator>(),
				std::back_inserter(item.second),
				[](auto in)->int64_t { return std::stoll(boost::copy_range<std::string>(in)); });

		ret.push_back(item);
	}

//	std::ofstream ofile("out.txt");
//
//	for(auto item : ret)
//	{
//		ofile << item.first << ":";
//		for(auto num : item.second)
//			ofile << " " << num;
//		ofile << std::endl;
//	}
//
//	ofile.flush();
//	ofile.close();

	return ret;
}

bool try_to_eval(const std::vector<int64_t> &number, uint64_t act_number, int64_t shall_result, int64_t tmp_res, bool scnd_part)
{
	if(act_number >= number.size())
	{
		// evaluate and abort
		if(tmp_res == shall_result)
			return true;
		else
			return false;
	}

	tmp_res += number[act_number++];
	if(try_to_eval(number, act_number, shall_result, tmp_res, scnd_part))
		return true;

	tmp_res -= number[--act_number];

	tmp_res *= number[act_number++];
	if(try_to_eval(number, act_number, shall_result, tmp_res, scnd_part))
		return true;

	if(!scnd_part)
		return false;

	tmp_res /= number[--act_number];

	tmp_res = std::stoll(std::to_string(tmp_res) + std::to_string(number[act_number++]));
	return try_to_eval(number, act_number, shall_result, tmp_res, scnd_part);
}

static int64_t task(const std::vector<std::pair<int64_t, std::vector<int64_t>>> &input, bool scnd_part)
{
	// the type of the 0 is the type of transform reduce operations -> needs to be 64bit
	return std::transform_reduce(input.cbegin(), input.cend(), (int64_t) 0, std::plus{}, [=](const std::pair<int64_t, std::vector<int64_t>> &item)->int64_t
	{
		if(try_to_eval(item.second, 1, item.first, item.second[0], scnd_part))
			return item.first;
		else
			return 0;
	});
}

void day7()
{
	std::vector<std::pair<int64_t, std::vector<int64_t>>> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day7_real_input.txt");

	int64_t task1_res = task(input, false);
	int64_t task2_res = task(input, true);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
