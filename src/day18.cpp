/*
 * day18.cpp
 *
 *  Created on: 22.12.2024
 *      Author: liv
 */

#include "day18.h"

#include <iostream>
#include <chrono>
#include <fstream>

#include <eigen3/Eigen/Dense>

#include <boost/algorithm/string.hpp>

#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

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

static std::vector<Eigen::Vector2i> read_file(std::string in_file)
{
	std::vector<Eigen::Vector2i> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		std::vector<std::string> pos_nums;
		boost::split(pos_nums, line, boost::is_any_of(","));

		ret.emplace_back(std::stoul(pos_nums[0]), std::stoul(pos_nums[1]));
	}

	return ret;
}

static std::pair<bool, int32_t> calc_dijkstra(const std::vector<Eigen::Vector2i> &input, const Eigen::Vector2i size, const uint32_t bytes)
{
	// create field
	std::vector<std::string> field(size.y(), std::string(size.x(), '.'));

	for(uint32_t b = 0; b < bytes; b++)
		field[input[b].y()][input[b].x()] = '#';

	// now create graph from that
	lemon::ListDigraph g;
	lemon::ListDigraph::ArcMap<int> arc_map(g);
	std::unordered_map<Eigen::Vector2i, lemon::ListDigraph::Node> node_map;

	for(uint32_t x = 0; x < (uint32_t) size.x(); x++)
	{
		for(uint32_t y = 0; y < (uint32_t) size.y(); y++)
		{
			if(field[y][x] == '.')
			{
				Eigen::Vector2i pos(x, y);

				auto node = g.addNode();
				node_map[pos] = node;
			}
		}
	}

	for(int x = 0; x < size.x(); x++)
	{
		for(int y = 0; y < size.y(); y++)
		{
			Eigen::Vector2i pos(x, y);

			// check for walkable field
			if(field[y][x] == '.')
			{
				auto node = node_map.at(pos);

				// check for neighbors with a height one greater than this one -> place a connection there
				std::array<Eigen::Vector2i, 4> dirs_to_check{Eigen::Vector2i{-1, 0}, Eigen::Vector2i{1, 0}, Eigen::Vector2i{0, -1}, Eigen::Vector2i{0, 1}};
				for(auto dir : dirs_to_check)
				{
					Eigen::Vector2i neigh_pos = pos + dir;

					// check if in range
					if((neigh_pos.x() >= 0) && (neigh_pos.y() >= 0) && (neigh_pos.x() < size.x()) && (neigh_pos.y() < size.y()))
					{
						// check if it is a walkable field
						if(field[neigh_pos.y()][neigh_pos.x()] != '.')
							continue;

						auto neigh_node = node_map.at(neigh_pos);

						auto arc_con = g.addArc(node, neigh_node);

						arc_map[arc_con] = 1;
					}
				}
			}
		}
	}

	// shortest path
	lemon::Dijkstra<lemon::ListDigraph> dijkstra(g, arc_map);
	dijkstra.run(node_map.at(Eigen::Vector2i(0,0)));

	return std::make_pair(dijkstra.reached(node_map.at(size-Eigen::Vector2i(1,1))), dijkstra.dist(node_map.at(size-Eigen::Vector2i(1,1))));
}

static uint32_t task1(const std::vector<Eigen::Vector2i> &input, const Eigen::Vector2i size, const uint32_t bytes)
{
	return calc_dijkstra(input, size, bytes).second;
}

static Eigen::Vector2i task2(const std::vector<Eigen::Vector2i> &input, const Eigen::Vector2i size)
{
	Eigen::Vector2i last_byte;

	for(uint32_t b = 1; b < input.size(); b++)
	{
		auto ret = calc_dijkstra(input, size, b);

		if(!ret.first)
			return last_byte;

		last_byte = input[b];
	}

	return Eigen::Vector2i(-1, -1);
}


void day18()
{
	std::vector<Eigen::Vector2i> input;

	auto t1 = std::chrono::steady_clock::now();

//	input = read_file("day18_test_input.txt");
//	uint32_t task1_res = task1(input, Eigen::Vector2i(7, 7), 12);
//	auto task2_res = task2(input, Eigen::Vector2i(7, 7));

	input = read_file("day18_real_input.txt");
	uint32_t task1_res = task1(input, Eigen::Vector2i(71, 71), 1024);
	auto task2_res = task2(input, Eigen::Vector2i(71, 71));

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << task2_res << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
