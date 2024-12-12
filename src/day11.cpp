/*
 * day11.cpp
 *
 *  Created on: 11.12.2024
 *      Author: liv
 */

#include "day11.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>
#include <numeric>
#include <execution>
#include <cmath>

#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include <boost/functional/hash.hpp>

static std::list<uint64_t> read_file(std::string in_file)
{
	std::list<uint64_t> ret;

	std::ifstream file(in_file);
	std::string line;
	std::getline(file, line);

	std::transform(boost::make_split_iterator(line, boost::token_finder(boost::is_space(), boost::token_compress_on)),
					boost::split_iterator<std::string::iterator>(),
					std::back_inserter(ret),
					[](auto in)->uint64_t { return std::stoll(boost::copy_range<std::string>(in)); });

	return ret;
}

static void do_a_blink(std::list<uint64_t> &input)
{
	for(auto it = input.begin(); it != input.end(); )
	{
		if(*it == 0)
		{
			*it = 1;
			it++;
		}
		else if(static_cast<uint64_t>(std::log10(*it)+1) % 2 == 0) // even number of digits
		{
			uint32_t num_len = static_cast<uint64_t>(std::log10(*it)) + 1;

			uint64_t first_num = *it / std::pow(10.0, (double) num_len/2);
			uint64_t second_num = *it - (first_num * std::pow(10.0, (double) num_len/2));

			*it = first_num;
			it++;
			it = std::next(input.insert(it, second_num));
		}
		else
		{
			*it *= 2024;
			it++;
		}
	}
}

static uint64_t task1(std::list<uint64_t> input)
{
	for(uint32_t blinks = 0; blinks < 25; blinks++)
	{
		do_a_blink(input);
	}

	return input.size();
}

// hash algorithm for the custom pair used in the unordered map
template<>
struct std::hash<std::pair<uint64_t, uint32_t>>
{
    std::size_t operator()(const std::pair<uint64_t, uint32_t>& s) const noexcept
    {
    	std::size_t res = 0;
        std::size_t h1 = std::hash<uint64_t>{}(s.first);
        std::size_t h2 = std::hash<uint32_t>{}(s.second);
        boost::hash_combine(res, h1);
        boost::hash_combine(res, h2);
        return res;
    }
};

const uint64_t count_number_of_stones(uint64_t num, uint32_t act_depth, uint32_t max_depth)
{
	static std::unordered_map<std::pair<uint64_t, uint32_t>, uint64_t> stone_cache;

	if(act_depth == max_depth)
		return 1;

	if(stone_cache.count(std::make_pair(num, act_depth)))
		return stone_cache.at(std::make_pair(num, act_depth));
	else
	{
		uint64_t res = 0;

		if(num == 0)
		{
			num = 1;
			res = count_number_of_stones(num, ++act_depth, max_depth);
		}
		else
		{
			uint32_t num_len = static_cast<uint64_t>(std::log10(num)+1);
			if(num_len % 2 == 0) // even number of digits
			{
				uint64_t first_num = num / std::pow(10.0, (double) num_len/2);
				uint64_t second_num = num - (first_num * std::pow(10.0, (double) num_len/2));

				uint64_t count = count_number_of_stones(first_num, ++act_depth, max_depth);

				count += count_number_of_stones(second_num, act_depth, max_depth);
				act_depth--;
				res = count;
			}
			else
			{
				num *= 2024;
				res = count_number_of_stones(num, ++act_depth, max_depth);
			}
		}

		stone_cache[std::make_pair(num, act_depth)] = res;
		return res;
	}
}

static uint64_t task2(std::list<uint64_t> input, uint32_t num_blinks)
{
	const uint32_t num_simple = 25;
	if(num_blinks > num_simple)
	{
		for(uint32_t i = 0; i < num_simple; i++)
			do_a_blink(input);
		num_blinks -= num_simple;
	}

	// std::lists cannot be parallelized, so use a vector instead
	std::vector<uint64_t> input_vec;
	std::copy(input.cbegin(), input.cend(), std::back_inserter(input_vec));

	return std::transform_reduce(input_vec.begin(), input_vec.end(),
								 (uint64_t) 0, std::plus{}, [num_blinks](uint64_t initial_num)
	{ return count_number_of_stones(initial_num, 0, num_blinks); });
}

void day11()
{
	std::list<uint64_t> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day11_real_input.txt");

	uint64_t task1_res = task1(input);
	uint64_t task2_res = task2(input, 75);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}


