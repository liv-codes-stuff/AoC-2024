/*
 * day5.cpp
 *
 *  Created on: 09.12.2024
 *      Author: liv
 */

#include "day5.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>

#include <boost/algorithm/string.hpp>

static std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> read_file(std::string in_file)
{
	std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		if(std::find(line.cbegin(), line.cend(), '|') != line.cend())
		{
			// map item
			std::vector<std::string> first_and_second_number;
			boost::split(first_and_second_number, line, boost::is_any_of("|"), boost::token_compress_off);

			ret.first[std::stoul(first_and_second_number[1])].push_back(std::stoul(first_and_second_number[0]));
		}
		else if(std::find(line.cbegin(), line.cend(), ',') != line.cend())
		{
			std::vector<uint32_t> item;

			std::transform(boost::make_split_iterator(line, boost::token_finder(boost::is_any_of(","), boost::token_compress_off)),
							boost::split_iterator<std::string::iterator>(),
							std::back_inserter(item),
							[](auto in)->uint32_t { return std::stoul(boost::copy_range<std::string>(in)); });

			ret.second.push_back(item);
		}
	}

	return ret;
}

uint32_t extract_middle_number(const std::vector<uint32_t> &in)
{
	if(in.size() % 2 != 1)
		throw std::out_of_range("even number of elements, there is no middle");

	uint32_t ret = in[in.size()/2];
	return ret;
}

bool check_if_update_is_correct(const std::unordered_map<uint32_t, std::vector<uint32_t>> &map, const std::vector<uint32_t> &updated_pages)
{
	bool page_correct = true;

	std::vector<uint32_t> updated_pages_sorted = updated_pages;
	std::sort(updated_pages_sorted.begin(), updated_pages_sorted.end());

	for(auto page_it = updated_pages.cbegin(); page_it != updated_pages.cend(); page_it++)
	{
		if(map.count(*page_it) < 1) // this element doesn't exists as key
		{
			// so it is ok
			continue;
		}

		std::vector<uint32_t> dep_updated;
		std::vector<uint32_t> dependencies_sorted = map.at(*page_it);
		std::sort(dependencies_sorted.begin(), dependencies_sorted.end());

		std::set_intersection(dependencies_sorted.cbegin(), dependencies_sorted.cend(),
				updated_pages_sorted.cbegin(), updated_pages_sorted.cend(),
				std::back_inserter(dep_updated));

		// check if the items before contain all dependencies
		std::vector<uint32_t> pages_till_now;
		std::copy(updated_pages.cbegin(), page_it, std::back_inserter(pages_till_now));
		std::sort(pages_till_now.begin(), pages_till_now.end());

		if(!std::includes(pages_till_now.cbegin(), pages_till_now.cend(), dep_updated.cbegin(), dep_updated.cend()))
		{
			page_correct = false;
			break;
		}

	}
	return page_correct;
}

static uint32_t task1(const std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> &input)
{
	return std::transform_reduce(input.second.cbegin(), input.second.cend(), 0, std::plus{},
			                     [&input](const std::vector<uint32_t> &updated_pages)->uint32_t
	{
		if(check_if_update_is_correct(input.first, updated_pages))
			return extract_middle_number(updated_pages);
		else
			return 0;
	});
}

static uint32_t task2(const std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> &input)
{
	return std::transform_reduce(input.second.cbegin(), input.second.cend(), 0, std::plus{},
			                     [&input](std::vector<uint32_t> updated_pages)->uint32_t
	{
		if(!check_if_update_is_correct(input.first, updated_pages))
		{
			// only if it needs reordering

			std::vector<uint32_t> updated_pages_sorted = updated_pages;
			std::sort(updated_pages_sorted.begin(), updated_pages_sorted.end());

			std::vector<uint32_t>::const_iterator elm_moved = updated_pages.cend();
			for(auto page_it = updated_pages.cbegin(); page_it != updated_pages.cend();)
			{
				if(input.first.count(*page_it) < 1) // this element doesn't exists as key
				{
					// so it is ok
					page_it++;
					continue;
				}

				std::vector<uint32_t> dep_updated;
				std::vector<uint32_t> dependencies_sorted = input.first.at(*page_it);
				std::sort(dependencies_sorted.begin(), dependencies_sorted.end());

				std::set_intersection(dependencies_sorted.cbegin(), dependencies_sorted.cend(),
						updated_pages_sorted.cbegin(), updated_pages_sorted.cend(),
						std::back_inserter(dep_updated));

				// check if the items before contain all dependencies
				std::vector<uint32_t> pages_till_now;
				std::copy(updated_pages.cbegin(), page_it, std::back_inserter(pages_till_now));
				std::sort(pages_till_now.begin(), pages_till_now.end());

				if(!std::includes(pages_till_now.cbegin(), pages_till_now.cend(), dep_updated.cbegin(), dep_updated.cend()))
				{
					// this element is at the wrong position
					// so push it one element right and try again

					page_it = updated_pages.insert(std::next(page_it, 2), *page_it) - 2;
					page_it = updated_pages.erase(page_it);
					page_it++; // continue with this element
					elm_moved = page_it;
				}
				else
				{
					if(elm_moved != updated_pages.cend())
					{
						page_it = updated_pages.cbegin();
						elm_moved = updated_pages.cend();
					}
					else
						page_it++;
				}

			}
			return extract_middle_number(updated_pages);
		}
		else
			return 0;
	});
}


void day5()
{
	std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day5_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}


