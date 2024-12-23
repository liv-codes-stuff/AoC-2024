/*
 * day19.cpp
 *
 *  Created on: 22.12.2024
 *      Author: liv
 */

#include "day19.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <utility>
#include <execution>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include <re2/re2.h>

static std::pair<std::vector<std::string>, std::vector<std::string>> read_file(std::string in_file)
{
	std::pair<std::vector<std::string>, std::vector<std::string>> ret;

	std::ifstream file(in_file);
	std::string line;
	std::getline(file, line);
	boost::split(ret.first, line, boost::is_any_of(","));
	std::for_each(ret.first.begin(), ret.first.end(), [](auto &elm)
	{
		boost::trim(elm);
	});

	// skip empty line
	std::getline(file, line);

	for(std::string line; std::getline(file, line);)
	{
		ret.second.push_back(line);
	}

	return ret;
}

static uint32_t string_view_begins_with(const std::string &big_str, const std::string &str, uint32_t offs)
{
	if(big_str.size() < str.size())
		return 0;

	for(uint32_t i = 0; i < str.size(); i++)
	{
		if(big_str[i+offs] != str[i])
			return 0;
	}

	return str.size();
}

static uint64_t recursive_match(const std::unordered_map<char, std::vector<std::string>> &towel_lut, const std::string &in, uint32_t str_offs)
{
	static std::unordered_map<std::string, uint64_t> cache;

	uint64_t cntr = 0;

	if(str_offs == in.size())
		return 1;

	std::string to_test_str = std::string(in.c_str()+str_offs);

	if(cache.count(to_test_str))
		return cache.at(to_test_str);

	if(towel_lut.count(in[str_offs]) == 0)
		return 0;

	for(const std::string &towel : towel_lut.at(in[str_offs]))
	{
		uint32_t offs = string_view_begins_with(in, towel, str_offs);
		if(offs != 0)
		{
			// string can match with this as the beginning, so continue to search
			cntr += recursive_match(towel_lut, in, str_offs + offs);
		}
	}

	cache[to_test_str] = cntr;
	return cntr;
}

static uint32_t task1(std::pair<std::vector<std::string>, std::vector<std::string>> input)
{
	// first sort the towels after the length descending
	std::sort(input.first.rbegin(), input.first.rend(), [](const std::string& in1, const std::string& in2)->uint32_t
	{
		return in1.size() < in2.size();
	});

	std::string regex_str = "(";
	for(auto str : input.first)
		regex_str += str + "|";
	regex_str.erase(regex_str.size()-1); // delete last |
	regex_str += ")+";

//	std::cout << regex_str << std::endl;

	// std::regex is too slow and doesnt execute (at least not in <20s), google re2 takes just milliseconds...
	RE2 re(regex_str);

	uint32_t cntr = 0;
	for(const auto& design : input.second)
	{
		if(RE2::FullMatch(design, re))
			cntr++;
	}

	return cntr;
}

static uint64_t task2(const std::pair<std::vector<std::string>, std::vector<std::string>> &input)
{
	std::unordered_map<char, std::vector<std::string>> first_char_lut;

	for(const auto &str : input.first)
	{
		first_char_lut[str[0]].push_back(str);
	}

	return std::transform_reduce(input.second.cbegin(), input.second.cend(), (uint64_t) 0, std::plus{},
						  [&](const std::string &design)->uint64_t
	{
		uint64_t cnt = recursive_match(first_char_lut, design, 0);
//		std::cout << cnt << " " << design << std::endl;
		return cnt;
	});
}

void day19()
{
	std::pair<std::vector<std::string>, std::vector<std::string>> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day19_real_input.txt");
	auto task1_res = task1(input);
	auto task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << task2_res << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
