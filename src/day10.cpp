/*
 * day10.cpp
 *
 *  Created on: 22.12.2024
 *      Author: liv
 */

#include "day10.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <memory>
#include <array>

#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include <eigen3/Eigen/Dense>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
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

struct data_type_t
{
	std::unique_ptr<lemon::ListDigraph> g;
	std::unique_ptr<lemon::ListDigraph::ArcMap<int>> arc_weight;
	std::unique_ptr<boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>> node_map;
	std::unique_ptr<boost::bimap<boost::bimaps::unordered_set_of<Eigen::Vector2i>, boost::bimaps::unordered_multiset_of<char>>> height_map;
	std::unique_ptr<std::vector<lemon::ListDigraph::Arc>> arc_list;


	typedef boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>::value_type node_map_entry_t;
	typedef boost::bimap<boost::bimaps::unordered_set_of<Eigen::Vector2i>, boost::bimaps::unordered_multiset_of<char>>::value_type height_map_entry_t;

	Eigen::Vector2i dim;
};

static data_type_t read_file(std::string in_file)
{
	data_type_t ret;

	ret.g = std::make_unique<lemon::ListDigraph>();
	ret.arc_weight = std::make_unique<lemon::ListDigraph::ArcMap<int>>(*ret.g);
	ret.node_map = std::make_unique<boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>>();
	ret.height_map = std::make_unique<boost::bimap<boost::bimaps::unordered_set_of<Eigen::Vector2i>, boost::bimaps::unordered_multiset_of<char>>>();
	ret.arc_list = std::make_unique<std::vector<lemon::ListDigraph::Arc>>();

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

				auto node = ret.g->addNode();
				ret.node_map->insert(data_type_t::node_map_entry_t(node, pos));
				ret.height_map->insert(data_type_t::height_map_entry_t(pos, std::stoul(std::string(1, line[x]))));
			}
		}

		y_cntr++;
	}

	for(int x = 0; x < (int) x_size; x++)
	{
		for(int y = 0; y < y_cntr; y++)
		{
			Eigen::Vector2i pos(x, y);

			// check if it is a valid height
			if(ret.height_map->left.count(pos) == 0)
				continue;

			auto node = ret.node_map->right.at(pos);

			int height = ret.height_map->left.at(pos);

			// check for neighbors with a height one greater than this one -> place a connection there
			std::array<Eigen::Vector2i, 4> dirs_to_check{Eigen::Vector2i{-1, 0}, Eigen::Vector2i{1, 0}, Eigen::Vector2i{0, -1}, Eigen::Vector2i{0, 1}};
			for(auto dir : dirs_to_check)
			{
				Eigen::Vector2i neigh_pos = pos + dir;

				// check if in range
				if((neigh_pos.x() >= 0) && (neigh_pos.y() >= 0) && (neigh_pos.x() < (int) x_size) && (neigh_pos.y() < y_cntr))
				{
					// check if it is a valid height
					if(ret.height_map->left.count(neigh_pos) == 0)
						continue;

					int neigh_height = ret.height_map->left.at(neigh_pos);
					auto neigh_node = ret.node_map->right.at(neigh_pos);

					if(neigh_height-height == 1)
					{
						auto arc_con = ret.g->addArc(node, neigh_node);

						ret.arc_list->push_back(arc_con);

						(*ret.arc_weight)[arc_con] = 1;
					}
				}
			}
		}
	}

	ret.dim = Eigen::Vector2i(x_size, y_cntr);

	return ret;
}

// my own sort of depth first search
static uint32_t recursive_count_paths(const data_type_t &input, lemon::ListDigraph::Node start_node)
{
	char start_height = input.height_map->left.at(input.node_map->left.at(start_node));
	if(start_height == 9)
		return 1;

	uint32_t total_path_count = 0;

	auto pos = input.node_map->left.at(start_node);

	// count the ways from here
	std::array<Eigen::Vector2i, 4> dirs_to_check{Eigen::Vector2i{-1, 0}, Eigen::Vector2i{1, 0}, Eigen::Vector2i{0, -1}, Eigen::Vector2i{0, 1}};

	for(auto dir : dirs_to_check)
	{
		Eigen::Vector2i neigh_pos = pos + dir;

		// check if in range
		if((neigh_pos.x() >= 0) && (neigh_pos.y() >= 0) && (neigh_pos.x() < input.dim.x()) && (neigh_pos.y() < input.dim.y()))
		{
			// check if it is a valid height
			if(input.height_map->left.count(neigh_pos) == 0)
				continue;

			auto neigh_node = input.node_map->right.at(neigh_pos);

			// get height from this node
			char neigh_height = input.height_map->left.at(neigh_pos);

			if(neigh_height-start_height == 1)
			{
				// check from this neighbor further
				uint32_t path_count = recursive_count_paths(input, neigh_node);

				// if there is no path, it returns 0
				total_path_count += path_count;
			}
		}
	}

	return total_path_count;
}

static uint32_t task1(const data_type_t &input)
{
	uint32_t score = 0;

	auto range = input.height_map->right.equal_range(0);
	for(auto it = range.first; it != range.second; it++)
	{
		// for all trailheads

		// get node
		auto node = input.node_map->right.at(it->get_left());

		// calculate dijkstra from there
		lemon::Dijkstra<lemon::ListDigraph> dijkstra(*input.g, *input.arc_weight);
		dijkstra.run(node);

		auto dest_range = input.height_map->right.equal_range(9);
		for(auto dest_it = dest_range.first; dest_it != dest_range.second; dest_it++)
		{
			// get node
			auto dest_node = input.node_map->right.at(dest_it->get_left());

			// check if the point is reachable
			if(dijkstra.reached(dest_node))
			{
				score++;
			}
		}
	}
	return score;
}

static uint32_t task2(data_type_t &input)
{
	uint32_t score = 0;

	auto range = input.height_map->right.equal_range(0);

	// for all trailheads
	for(auto it = range.first; it != range.second; it++)
	{
		score += recursive_count_paths(input, input.node_map->right.at(it->get_left()));
	}

	return score;
}

void day10()
{
	data_type_t input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day10_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << task2_res << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
