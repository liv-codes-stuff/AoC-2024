/*
 * day20.cpp
 *
 *  Created on: 23.12.2024
 *      Author: liv
 */

#include "day20.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <set>

#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include <boost/functional/hash.hpp>

#include <eigen3/Eigen/Dense>

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

struct day20_data_type_t
{
	std::vector<std::string> map;

	std::unique_ptr<lemon::ListDigraph> g;
	std::unique_ptr<lemon::ListDigraph::ArcMap<int>> arc_weight;
	std::unique_ptr<boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>> node_map;
	std::unique_ptr<std::vector<lemon::ListDigraph::Arc>> arc_list;

	lemon::ListDigraph::Node start_node, end_node;

	typedef boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>::value_type node_map_entry_t;

	Eigen::Vector2i dim;
};

static day20_data_type_t read_file(std::string in_file)
{
	day20_data_type_t ret;

	ret.g = std::make_unique<lemon::ListDigraph>();
	ret.arc_weight = std::make_unique<lemon::ListDigraph::ArcMap<int>>(*ret.g);
	ret.node_map = std::make_unique<boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>>();
	ret.arc_list = std::make_unique<std::vector<lemon::ListDigraph::Arc>>();

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		ret.map.push_back(line);
	}

	ret.dim.x() = ret.map[0].size();
	ret.dim.y() = ret.map.size();

	for(uint32_t x = 0; x < (uint32_t) ret.dim.x(); x++)
	{
		for(uint32_t y = 0; y < (uint32_t) ret.dim.y(); y++)
		{
			Eigen::Vector2i pos(x, y);

			auto node = ret.g->addNode();
			ret.node_map->insert(day20_data_type_t::node_map_entry_t(node, pos));

			if(ret.map[y][x] == 'S')
				ret.start_node = node;
			else if(ret.map[y][x] == 'E')
				ret.end_node = node;
		}
	}

	for(int x = 0; x < ret.dim.x(); x++)
	{
		for(int y = 0; y < ret.dim.y(); y++)
		{
			Eigen::Vector2i pos(x, y);

			if(ret.map[y][x] == '#')
				continue;

			auto node = ret.node_map->right.at(pos);

			// check for neighbors with a height one greater than this one -> place a connection there
			std::array<Eigen::Vector2i, 4> dirs_to_check{Eigen::Vector2i{-1, 0}, Eigen::Vector2i{1, 0}, Eigen::Vector2i{0, -1}, Eigen::Vector2i{0, 1}};
			for(auto dir : dirs_to_check)
			{
				Eigen::Vector2i neigh_pos = pos + dir;

				// check if in range
				if((neigh_pos.x() >= 0) && (neigh_pos.y() >= 0) && (neigh_pos.x() < ret.dim.x()) && (neigh_pos.y() < ret.dim.y()))
				{
					// check if neighbor is way
					if(ret.map[neigh_pos.y()][neigh_pos.x()] == '#')
						continue;

					auto neigh_node = ret.node_map->right.at(neigh_pos);
					auto arc_con = ret.g->addArc(node, neigh_node);
					auto arc_con_r = ret.g->addArc(neigh_node, node);

					ret.arc_list->push_back(arc_con);
					ret.arc_list->push_back(arc_con_r);

					(*ret.arc_weight)[arc_con] = 1;
					(*ret.arc_weight)[arc_con_r] = 1;
				}
			}
		}
	}

	return ret;
}

static uint32_t task(day20_data_type_t &input, uint32_t short_cut_min_len, int max_cheat_len)
{
	uint32_t short_cntr = 0;

	// calculate the shortest path
	lemon::Dijkstra<lemon::ListDigraph> dijkstra(*input.g, *input.arc_weight);
	dijkstra.run(input.start_node);

	auto last_node = input.end_node;
	auto last_arc = dijkstra.predMap()[last_node];

	// create a list of nodes in the shortest path
	std::set<lemon::ListDigraph::Node> node_list;
	node_list.insert(last_node);
	while(last_arc != lemon::INVALID)
	{
		// for all arcs in the shortest path
		last_node = input.g->source(last_arc);
		last_arc = dijkstra.predMap()[last_node];

		node_list.insert(last_node);
	}

	// for all nodes of the shortest path
	// -> for all nodes in the path -> check if dijkstra diff - the new path length > short_cut_min_length and air distance <= 20
	// if so this is a shortcut
	for(auto it1 = node_list.begin(); it1 != node_list.cend(); it1++)
	{
		Eigen::Vector2i pos = input.node_map->left.at(*it1);

		int dist_pos = dijkstra.dist(*it1);

		for(auto it2 = node_list.begin(); it2 != node_list.cend(); it2++)
		{
			Eigen::Vector2i neigh_pos = input.node_map->left.at(*it2);

			// check if in range
			Eigen::Vector2i diff = neigh_pos - pos;
			int diff_norm_rounded = std::abs(diff.x()) + std::abs(diff.y());
			if(diff_norm_rounded <= max_cheat_len)
			{
				// this position is worth to check
				int dist_neigh = dijkstra.dist(*it2);

				if(dist_neigh-dist_pos-diff_norm_rounded >= (int) short_cut_min_len)
				{
					// is a shortcut

//					// and draw it
//					auto draw_map = input.map;
//					draw_map[pos.y()][pos.x()] = 'C';
//					draw_map[neigh_pos.y()][neigh_pos.x()] = 'T';
//					for(auto line : draw_map)
//						std::cout << line << std::endl;
//
//					std::cout << "shortcut saves: " << dist_neigh-dist_pos-diff_norm_rounded << std::endl;

					short_cntr++;
				}
			}
		}
	}

	return short_cntr;
}


void day20()
{
	day20_data_type_t input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day20_real_input.txt");
	auto task1_res = task(input, 100, 2);
	auto task2_res = task(input, 100, 20);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << task2_res << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
