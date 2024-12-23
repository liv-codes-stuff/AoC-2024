/*
 * day8.cpp
 *
 *  Created on: 21.12.2024
 *      Author: liv
 */

#include "day8.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <utility>
#include <unordered_map>
#include <algorithm>

#include <eigen3/Eigen/Dense>

#include <boost/functional/hash.hpp>

// std::hash function for Eigen Matrices
template<typename _Scalar, int _Rows, int _Cols>
struct std::hash<Eigen::Matrix<_Scalar, _Rows, _Cols>>
{
    std::size_t operator()(const Eigen::Matrix<_Scalar, _Rows, _Cols> &v) const noexcept
    {
    	std::size_t ret = 0;

    	for(std::size_t x = 0; x < _Rows; x++)
    	{
    		for(std::size_t y = 0; y < _Cols; y++)
    		{
    			std::size_t h1 = std::hash<_Scalar>{}(v(x, y));
    			boost::hash_combine(ret, h1);
    		}
    	}

        return ret;
    }
};

static std::tuple<std::vector<Eigen::Vector2i>, std::unordered_map<Eigen::Vector2i, char>, Eigen::Vector2i> read_file(std::string in_file)
{
	std::tuple<std::vector<Eigen::Vector2i>, std::unordered_map<Eigen::Vector2i, char>, Eigen::Vector2i> ret;

	std::ifstream file(in_file);
	int y_cntr = 0;
	uint32_t x_size = 0;
	for(std::string line; std::getline(file, line);)
	{
		if(x_size == 0)
			x_size = line.size();

		for(int x = 0; x < (int) line.size(); x++)
		{
			if(line[x] != '.')
			{
				Eigen::Vector2i pos(x, y_cntr);
				std::get<0>(ret).push_back(pos);
				std::get<1>(ret)[pos] = line[x];
			}
		}

		y_cntr++;
	}

	std::get<2>(ret) = Eigen::Vector2i(x_size, y_cntr);

	return ret;
}

static uint32_t task1(const std::tuple<std::vector<Eigen::Vector2i>, std::unordered_map<Eigen::Vector2i, char>, Eigen::Vector2i> &input)
{
	std::vector<Eigen::Vector2i> antinodes;
	std::for_each(std::get<0>(input).cbegin(), std::get<0>(input).cend(), [&](const Eigen::Vector2i &pos)
	{
		// for each antenna search for all antennas of the same type
		// and calculate the antinode for this antenna pair
		std::for_each(std::get<0>(input).cbegin(), std::get<0>(input).cend(), [&](const Eigen::Vector2i &other_pos)
		{
			// skip the element we are testing from
			if(pos != other_pos)
			{
				// skip element if it is not of the same type as our element
				if(std::get<1>(input).at(pos) == std::get<1>(input).at(other_pos))
				{
					Eigen::Vector2i antinode_pos = other_pos + (other_pos - pos) * 1;

					if((antinode_pos.x() < std::get<2>(input).x()) &&
					   (antinode_pos.x() >= 0) &&
					   (antinode_pos.y() < std::get<2>(input).y()) &&
					   (antinode_pos.y() >= 0))
						antinodes.push_back(antinode_pos);
				}
			}
		});
	});

	// sort and remove duplicates (sort needed for std::unique)
	std::sort(antinodes.begin(), antinodes.end(), [](auto a, auto b)
	{
		if(a.y() < b.y())
			return true;
		else if(a.y() > b.y())
			return false;
		else
		{
			if(a.x() < b.x())
				return true;
			else if(a.x() > b.x())
				return false;
			else
				return false;
		}
	});
	antinodes.erase(std::unique(antinodes.begin(), antinodes.end()), antinodes.end());

	return antinodes.size();
}

static uint32_t task2(const std::tuple<std::vector<Eigen::Vector2i>, std::unordered_map<Eigen::Vector2i, char>, Eigen::Vector2i> &input)
{
	std::vector<Eigen::Vector2i> antinodes;
	std::for_each(std::get<0>(input).cbegin(), std::get<0>(input).cend(), [&](const Eigen::Vector2i &pos)
	{
		// for each antenna search for all antennas of the same type
		// and calculate the antinode for this antenna pair
		std::for_each(std::get<0>(input).cbegin(), std::get<0>(input).cend(), [&](const Eigen::Vector2i &other_pos)
		{
			// skip the element we are testing from
			if(pos != other_pos)
			{
				// skip element if it is not of the same type as our element
				if(std::get<1>(input).at(pos) == std::get<1>(input).at(other_pos))
				{
					for(uint32_t harmonics_cntr = 0; harmonics_cntr < 256; harmonics_cntr++)
					{
						Eigen::Vector2i antinode_pos = other_pos + (other_pos - pos) * harmonics_cntr;

						if((antinode_pos.x() < std::get<2>(input).x()) &&
						   (antinode_pos.x() >= 0) &&
						   (antinode_pos.y() < std::get<2>(input).y()) &&
						   (antinode_pos.y() >= 0))
							antinodes.push_back(antinode_pos);
					}
				}
			}
		});
	});

	// sort and remove duplicates (sort needed for std::unique)
	std::sort(antinodes.begin(), antinodes.end(), [](auto a, auto b)
	{
		if(a.y() < b.y())
			return true;
		else if(a.y() > b.y())
			return false;
		else
		{
			if(a.x() < b.x())
				return true;
			else if(a.x() > b.x())
				return false;
			else
				return false;
		}
	});
	antinodes.erase(std::unique(antinodes.begin(), antinodes.end()), antinodes.end());

	return antinodes.size();
}


void day8()
{
	std::tuple<std::vector<Eigen::Vector2i>, std::unordered_map<Eigen::Vector2i, char>, Eigen::Vector2i> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day8_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
