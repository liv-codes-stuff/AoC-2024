/*
 * day2.cpp
 *
 *  Created on: 02.12.2024
 *      Author: liv
 */

#include "day2.h"

#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <list>
#include <functional>

#include <boost/algorithm/string.hpp>

static std::vector<std::vector<int32_t>> read_file(std::string in_file)
{
	std::ifstream file(in_file);

	std::vector<std::vector<int32_t>> ret;
	for(std::string line; std::getline(file, line);)
	{
		std::vector<int32_t> vec_line;

		std::transform(boost::make_split_iterator(line, boost::token_finder(boost::is_space(), boost::token_compress_on)),
				boost::split_iterator<std::string::iterator>(),
				std::back_inserter(vec_line),
				[](auto in)->int32_t { return std::stol(boost::copy_range<std::string>(in)); });

		ret.push_back(vec_line);
	}

	return ret;
}

static bool check_if_minmax_is_valid(std::pair<std::list<int32_t>::const_iterator, std::list<int32_t>::const_iterator> minmax_it)
{
	// first check for same sign of all
	if((*minmax_it.first > 0) && (*minmax_it.second > 0) && (*minmax_it.second <= 3))
		return true;
	else if((*minmax_it.second < 0) && (*minmax_it.first < 0) && (*minmax_it.first >= -3))
		return true;
	else
		return false;
}

static bool check_if_report_is_safe(const std::vector<int32_t> &report, bool problem_dampener_enabled)
{
	// either constantly decreasing or increasing and then a difference between 1 and 3
	std::list<int32_t> difference;

	std::adjacent_difference(report.cbegin(), report.cend(), std::back_inserter(difference));

	difference.erase(difference.begin());

	auto minmax_it = std::minmax_element(difference.cbegin(), difference.cend());

	if(check_if_minmax_is_valid(minmax_it))
		return true;
	else if(problem_dampener_enabled)
	{
		// try to delete all levels at once to check if it is valid then
		for(uint32_t i = 0; i < report.size(); i++)
		{
			std::vector<int32_t> diff_report = report;
			std::list<int32_t> diff_mod;

			diff_report.erase(std::next(diff_report.begin(), i));

			std::adjacent_difference(diff_report.cbegin(), diff_report.cend(), std::back_inserter(diff_mod));

			diff_mod.erase(diff_mod.begin());

			auto minmax_it_mod = std::minmax_element(diff_mod.cbegin(), diff_mod.cend());

			if(check_if_minmax_is_valid(minmax_it_mod))
				return true;
		}
		return false;
	}
	else
		return false;
}

uint32_t task(const std::vector<std::vector<int32_t>> &list_of_reports, bool problem_dampener_enabled)
{
	return std::count_if(list_of_reports.cbegin(), list_of_reports.cend(), std::bind(check_if_report_is_safe, std::placeholders::_1, problem_dampener_enabled));
}

void day2()
{
	std::vector<std::vector<int32_t>> list_of_reports;

	auto t1 = std::chrono::steady_clock::now();

	list_of_reports = read_file("day2_real_input.txt");

	uint32_t task1_res = task(list_of_reports, false);
	uint32_t task2_res = task(list_of_reports, true);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
