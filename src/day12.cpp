/*
 * day12.cpp
 *
 *  Created on: 12.12.2024
 *      Author: liv
 */

#include "day12.h"

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <utility>
#include <list>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <array>

static std::vector<std::string> read_file(std::string in_file)
{
	std::vector<std::string> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
		ret.push_back(line);

	return ret;
}

// type -> area, perimeter, discounted
std::vector<std::tuple<char, uint32_t, uint32_t, uint32_t>> calculate_area_and_perimeter(const std::vector<std::string> &in)
{
	std::vector<std::tuple<char, uint32_t, uint32_t, uint32_t>> ret;

	std::vector<std::pair<int32_t, int32_t>> points_already_checked;
	std::vector<std::pair<int32_t, int32_t>> points_already_checked_sub;

	for(uint32_t x = 0; x < in[0].size(); x++)
	{
		for(uint32_t y = 0; y < in.size(); y++)
		{
			std::pair<int32_t, int32_t> point_to_check{x, y};

			if(std::find(points_already_checked.cbegin(), points_already_checked.cend(), point_to_check) == points_already_checked.cend())
			{
				// fresh point so search for area from here
				points_already_checked.push_back(point_to_check);
				points_already_checked_sub.push_back(point_to_check);

				uint32_t border_count = 0;
				std::vector<std::pair<char, std::pair<int32_t, int32_t>>> orientated_borders;
				uint32_t elm_count = 1;

				char area_type = in[y][x];

				// NOT:// check only neighbors on  the right and down to not double check them later on
				// check all four neighbors
				std::list<std::pair<std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>>> points_to_check;
				if(x > 0)
					points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(x-1, y), point_to_check));
				else // automatically a border
				{
					border_count+= 1;
					orientated_borders.push_back(std::make_pair('l', point_to_check));
				}
				if(y > 0)
					points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(x, y-1), point_to_check));
				else // automatically a border
				{
					border_count += 1;
					orientated_borders.push_back(std::make_pair('t', point_to_check));
				}
				if(x < in[0].size()-1)
					points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(x+1, y), point_to_check));
				else // automatically a border
				{
					border_count += 1;
					orientated_borders.push_back(std::make_pair('r', point_to_check));
				}
				if(y < in.size()-1)
					points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(x, y+1), point_to_check));
				else // automatically a border
				{
					border_count += 1;
					orientated_borders.push_back(std::make_pair('b', point_to_check));
				}

				while(points_to_check.size() > 0)
				{
					std::pair<std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>> sub_point_and_origin = points_to_check.front();
					points_to_check.pop_front();
					if(std::find(points_already_checked_sub.cbegin(), points_already_checked_sub.cend(), sub_point_and_origin.first) == points_already_checked_sub.cend())
					{
						// fresh point
						if(area_type != in[sub_point_and_origin.first.second][sub_point_and_origin.first.first])
						{
							border_count += 1;
							if(std::round(std::abs((double) sub_point_and_origin.first.first - sub_point_and_origin.second.first)
									+ std::abs((double) sub_point_and_origin.first.second - sub_point_and_origin.second.second)) > 1)
								throw std::exception();

							if(sub_point_and_origin.first.first - sub_point_and_origin.second.first < 0)
								orientated_borders.push_back(std::make_pair('l', sub_point_and_origin.second));
							if(sub_point_and_origin.first.first - sub_point_and_origin.second.first > 0)
								orientated_borders.push_back(std::make_pair('r', sub_point_and_origin.second));
							if(sub_point_and_origin.first.second - sub_point_and_origin.second.second < 0)
								orientated_borders.push_back(std::make_pair('t', sub_point_and_origin.second));
							if(sub_point_and_origin.first.second - sub_point_and_origin.second.second > 0)
								orientated_borders.push_back(std::make_pair('b', sub_point_and_origin.second));
						}
						else
						{
							points_already_checked.push_back(sub_point_and_origin.first);
							points_already_checked_sub.push_back(sub_point_and_origin.first);

							// check neighbors too
							if(sub_point_and_origin.first.first > 0)
								points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(sub_point_and_origin.first.first-1, sub_point_and_origin.first.second), sub_point_and_origin.first));
							else // automatically a border
							{
								border_count+= 1;
								orientated_borders.push_back(std::make_pair('l', sub_point_and_origin.first));
							}
							if(sub_point_and_origin.first.second > 0)
								points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(sub_point_and_origin.first.first, sub_point_and_origin.first.second-1), sub_point_and_origin.first));
							else // automatically a border
							{
								border_count+= 1;
								orientated_borders.push_back(std::make_pair('t', sub_point_and_origin.first));
							}
							if(sub_point_and_origin.first.first < (int32_t) in[0].size()-1)
								points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(sub_point_and_origin.first.first+1, sub_point_and_origin.first.second), sub_point_and_origin.first));
							else // automatically a border
							{
								border_count+= 1;
								orientated_borders.push_back(std::make_pair('r', sub_point_and_origin.first));
							}
							if(sub_point_and_origin.first.second < (int32_t) in.size()-1)
								points_to_check.emplace_back(std::make_pair(std::pair<int32_t, int32_t>(sub_point_and_origin.first.first, sub_point_and_origin.first.second+1), sub_point_and_origin.first));
							else // automatically a border
							{
								border_count+= 1;
								orientated_borders.push_back(std::make_pair('b', sub_point_and_origin.first));
							}

							// add this point to count
							elm_count++;
						}
					}
				}

				// now count the number of discounted sides

				std::array<std::vector<std::pair<char, std::pair<int32_t, int32_t>>>, 4> orientated_borders_sorted;
				std::copy_if(orientated_borders.begin(), orientated_borders.end(), std::back_inserter(orientated_borders_sorted[0]),
						     [](auto a)->bool { return a.first=='t'; });
				std::copy_if(orientated_borders.begin(), orientated_borders.end(), std::back_inserter(orientated_borders_sorted[1]),
							 [](auto a)->bool { return a.first=='b'; });
				std::copy_if(orientated_borders.begin(), orientated_borders.end(), std::back_inserter(orientated_borders_sorted[2]),
							 [](auto a)->bool { return a.first=='l'; });
				std::copy_if(orientated_borders.begin(), orientated_borders.end(), std::back_inserter(orientated_borders_sorted[3]),
							 [](auto a)->bool { return a.first=='r'; });

				auto x_sort = [](auto a, auto b)->bool
				{
					if(a.first < b.first)
						return true;
					else if(a.first > b.first)
						return false;
					else
					{
						if(a.second.first < b.second.first)
							return true;
						else if(a.second.first > b.second.first)
							return false;
						else if(a.second.second < b.second.second)
							return true;
						else
							return false;
					}
				};
				auto y_sort = [](auto a, auto b)->bool
				{
					if(a.first < b.first)
						return true;
					else if(a.first > b.first)
						return false;
					else
					{
						if(a.second.second < b.second.second)
							return true;
						else if(a.second.second > b.second.second)
							return false;
						else if(a.second.first < b.second.first)
							return true;
						else
							return false;
					}
				};

				std::sort(orientated_borders_sorted[0].begin(), orientated_borders_sorted[0].end(), y_sort);
				std::sort(orientated_borders_sorted[1].begin(), orientated_borders_sorted[1].end(), y_sort);
				std::sort(orientated_borders_sorted[2].begin(), orientated_borders_sorted[2].end(), x_sort);
				std::sort(orientated_borders_sorted[3].begin(), orientated_borders_sorted[3].end(), x_sort);

				uint32_t orientated_borders_count = 0;
				for(auto border_list : orientated_borders_sorted)
				{
					// on l and r : all x need to be constant, y only max diff of 1
					// on b and t: all y need to be constant, x only max diff of 1

					orientated_borders_count++;

					int32_t border_x = border_list[0].second.first, border_y = border_list[0].second.second;
					for(uint32_t i = 1; i < border_list.size(); i++)
					{
						if(border_list[i].first == 't' || border_list[i].first == 'b')
						{
							if(border_y != border_list[i].second.second)
							{
								orientated_borders_count++;
								border_y = border_list[i].second.second;
							}
							else if(border_list[i].second.first - border_x > 1)
							{
								orientated_borders_count++;
							}
							border_x = border_list[i].second.first;
						}
						else if(border_list[i].first == 'l' || border_list[i].first == 'r')
						{
							if(border_x != border_list[i].second.first)
							{
								orientated_borders_count++;
								border_x = border_list[i].second.first;
							}
							else if(border_list[i].second.second - border_y > 1)
							{
								orientated_borders_count++;
							}
							border_y = border_list[i].second.second;
						}
					}
				}

				ret.push_back(std::tuple<char, uint32_t, uint32_t, uint32_t>(area_type, elm_count, border_count, orientated_borders_count));
			}
			points_already_checked_sub.clear();
		}
	}

	return ret;
}

static uint32_t task1(const std::vector<std::string> &in)
{
	std::vector<std::tuple<char, uint32_t, uint32_t, uint32_t>> map;

	map = calculate_area_and_perimeter(in);

	return std::transform_reduce(map.cbegin(), map.cend(), (uint32_t) 0, std::plus{},
								 [](auto map_entry)->uint32_t
	{
		return std::get<1>(map_entry) * std::get<2>(map_entry);
	});
}

static uint32_t task2(const std::vector<std::string> &in)
{
	std::vector<std::tuple<char, uint32_t, uint32_t, uint32_t>> map;

	map = calculate_area_and_perimeter(in);

	return std::transform_reduce(map.cbegin(), map.cend(), (uint32_t) 0, std::plus{},
								 [](auto map_entry)->uint32_t
	{
		return std::get<1>(map_entry) * std::get<3>(map_entry);
	});
}

void day12()
{
	std::vector<std::string> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day12_real_input.txt");

	uint64_t task1_res = task1(input);
	uint64_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}


