/*
 * day3.cpp
 *
 *  Created on: 04.12.2024
 *      Author: liv
 */

#include "day3.h"

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <array>
#include <vector>
#include <numeric>

static std::string read_file(std::string in_file)
{
	std::string ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
		ret += line + "\n";

	return ret;
}

static uint32_t task1(std::string input)
{
	const std::regex num_reg(R"(mul\((\d+),(\d+)\))");

	std::vector<std::pair<uint32_t, uint32_t>> numbers;

	std::transform(std::sregex_token_iterator(input.begin(), input.end(), num_reg, {1}),
	               std::sregex_token_iterator(),
				   std::sregex_token_iterator(input.begin(), input.end(), num_reg, {2}),
	               std::back_inserter(numbers),
				   [](auto elm1, auto elm2)->std::pair<uint32_t, uint32_t>
	{
		return std::make_pair(std::stoul(elm1), std::stoul(elm2));
	});

	return std::transform_reduce(numbers.cbegin(), numbers.cend(), 0, std::plus{},
			              [](const std::pair<uint32_t, uint32_t> &in)
	{
		return in.first * in.second;
	});
}

static uint32_t task2(std::string input)
{
	const std::regex op_reg(R"((mul\(\d+,\d+\)|do\(\)|don't\(\)))");
	const std::regex num_reg(R"(mul\((\d+),(\d+)\))");

	std::vector<std::tuple<std::string, uint32_t, uint32_t>> interpreted;

	std::transform(std::sregex_token_iterator(input.begin(), input.end(), op_reg, {1}),
				   std::sregex_token_iterator(),
				   std::back_inserter(interpreted),
				   [num_reg](auto elm)->std::tuple<std::string, uint32_t, uint32_t>
	{
		std::tuple<std::string, uint32_t, uint32_t> ret;

		std::get<0>(ret) = elm;

		std::smatch num_match;
		if(std::regex_match(std::get<0>(ret), num_match, num_reg))
		{
			// mul op so store numbers
			std::get<1>(ret) = std::stoul(num_match[1]);
			std::get<2>(ret) = std::stoul(num_match[2]);
		}

		return ret;
	});

	// now execute the program

	// note: transform is ordered, transform reduce is NOT strictly ordered, so that
	// it would result in the wrong result using transform reduce here
	std::vector<uint32_t> mul_res;
	bool shall_exec_mul = true;
	std::transform(interpreted.cbegin(), interpreted.cend(), std::back_inserter(mul_res),
						         [&shall_exec_mul](const std::tuple<std::string, uint32_t, uint32_t> &in)->uint32_t
	{
		if(std::get<0>(in) == "do()")
			shall_exec_mul = true;
		else if(std::get<0>(in) == "don't()")
			shall_exec_mul = false;
		else if(shall_exec_mul)
			return std::get<1>(in) * std::get<2>(in);

		// if no mul then we just add a 0 to the result
		return 0;
	});
	return std::reduce(mul_res.begin(), mul_res.end());
}

void day3()
{
	std::string input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day3_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
