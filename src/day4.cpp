/*
 * day4.cpp
 *
 *  Created on: 06.12.2024
 *      Author: liv
 */

#include "day4.h"

#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <map>

static std::vector<std::string> read_file(std::string in_file)
{
	std::vector<std::string> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
		ret.push_back(line);

	return ret;
}

uint32_t count_forward_reverse_xmas(const std::string &in)
{
	const std::string match = "XMAS";

	uint32_t ret = 0;

	for(auto it = std::search(in.cbegin(), in.cend(), match.cbegin(), match.cend()); it != in.cend();
		it = std::search(std::next(it), in.cend(), match.cbegin(), match.cend()))
	{
		ret++;
	}

	for(auto it = std::search(in.crbegin(), in.crend(), match.cbegin(), match.cend()); it != in.crend();
		it = std::search(std::next(it), in.crend(), match.cbegin(), match.cend()))
	{
		ret++;
	}

	return ret;
}

std::vector<uint32_t> extract_mas_positions(const std::string &in)
{
	std::vector<uint32_t> pos;

	const std::string match = "MAS";

	for(auto it = std::search(in.cbegin(), in.cend(), match.cbegin(), match.cend()); it != in.cend();
		it = std::search(std::next(it), in.cend(), match.cbegin(), match.cend()))
	{
		pos.push_back(std::distance(in.cbegin(), std::next(it)));
	}

	for(auto it = std::search(in.crbegin(), in.crend(), match.cbegin(), match.cend()); it != in.crend();
		it = std::search(std::next(it), in.crend(), match.cbegin(), match.cend()))
	{
		pos.push_back(std::distance(in.cbegin(), std::next(it, 2).base()));
	}

	return pos;
}

// tuple of string, start pos, movement vector
std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> extract_diagonal_string_with_start_pos(const std::vector<std::string> &input)
{
	std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> ret;

	// mov = (1,1)
	// for x=0 count y from 0
	for(uint32_t y = 0; y < input.size(); y++)
	{
		std::string add_str;

		uint32_t i = 0;
		while(true)
		{
			if(i >= input[0].size() || i >= input.size()-y)
				break;

			add_str += input[y+i][0+i];

			i++;
		}

		ret.push_back(std::make_tuple(add_str, std::make_pair(0, y), std::make_pair(1, 1)));
	}

	// mov = (1,1)
	// for y=0 count x from 1
	for(uint32_t x = 1; x < input[0].size(); x++)
	{
		std::string add_str;

		uint32_t i = 0;
		while(true)
		{
			if(i >= input.size() || i >= input[0].size()-x)
				break;

			add_str += input[0+i][x+i];

			i++;
		}

		ret.push_back(std::make_tuple(add_str, std::make_pair(x, 0), std::make_pair(1, 1)));
	}

	// mov = (-1,1)
	// for y=0 count x from size-1
	for(int32_t x = input[0].size()-1; x >= 0 ; x--)
	{
		std::string add_str;

		uint32_t i = 0;
		while(true)
		{
			if(i >= input.size() || (int32_t) i > x)
				break;

			add_str += input[0+i][x-i];

			i++;
		}

		ret.push_back(std::make_tuple(add_str, std::make_pair(x, 0), std::make_pair(-1, 1)));
	}

	// mov = (-1,1)
	// for x=size-1 count y from 1
	for(int32_t y = 1; y < (int32_t) input.size(); y++)
	{
		std::string add_str;

		uint32_t i = 0;
		while(true)
		{
			if(i >= input[0].size() || i >= input.size()-y)
				break;

			add_str += input[y+i][input[0].size()-1-i];

			i++;
		}

		ret.push_back(std::make_tuple(add_str, std::make_pair(input[0].size()-1, y), std::make_pair(-1, 1)));
	}

	return ret;
}

static uint32_t task1(const std::vector<std::string> &input)
{
	std::vector<std::string> strings_to_check;

	// horizontal
	std::copy(input.cbegin(), input.cend(), std::back_inserter(strings_to_check));

	// vertical
	for(uint32_t i = 0; i < input[0].size(); i++)
	{
		std::string add_str;

		for(uint32_t j = 0; j < input.size(); j++)
		{
			add_str += input[j][i];
		}

		strings_to_check.push_back(add_str);
	}

	auto diagonal = extract_diagonal_string_with_start_pos(input);

	std::transform(diagonal.cbegin(), diagonal.cend(), std::back_inserter(strings_to_check),
				   [](auto elm) { return std::get<0>(elm); });

	return std::transform_reduce(strings_to_check.cbegin(), strings_to_check.cend(), 0, std::plus{}, count_forward_reverse_xmas);
}

static uint32_t task2(const std::vector<std::string> &input)
{
	std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> diagonal;

	diagonal = extract_diagonal_string_with_start_pos(input);

	std::vector<std::pair<uint32_t, uint32_t>> mas_pos;

	std::for_each(diagonal.cbegin(), diagonal.cend(), [&mas_pos](auto diag)
	{
		auto positions = extract_mas_positions(std::get<0>(diag));

		std::for_each(positions.cbegin(), positions.cend(), [&mas_pos,diag](auto pos)
		{
			// better use a library with vector math here
			uint32_t x_pos = std::get<1>(diag).first + std::get<2>(diag).first * pos;
			uint32_t y_pos = std::get<1>(diag).second + std::get<2>(diag).second * pos;

			mas_pos.push_back(std::make_pair(x_pos, y_pos));
		});
	});

	std::map<std::pair<uint32_t, uint32_t>, uint32_t> pos_count;
	std::for_each(mas_pos.cbegin(), mas_pos.cend(), [&pos_count](auto pos)
	{
		pos_count[pos]++;
	});

	std::vector<uint32_t> elm_cnt;
	std::transform(pos_count.cbegin(), pos_count.cend(), std::back_inserter(elm_cnt), [](auto a)->uint32_t
	{
		if(a.second == 2)
			return 1;
		else
			return 0;
	});

	return std::reduce(elm_cnt.cbegin(), elm_cnt.cend());
}

void day4()
{
	std::vector<std::string> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day4_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
