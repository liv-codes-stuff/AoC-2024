/*
 * day16.cpp
 *
 *  Created on: 16.12.2024
 *      Author: liv
 */

#include "day16.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <array>

#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/core.h>

const int weight_scale = 10000; // for part 2

// To express the rotation costs in the graph use one level of nodes per direction (n, e, w, s)
// see https://stackoverflow.com/questions/78208324/shortest-path-finding-in-grid-with-turn-cost

static std::tuple<std::vector<std::string>, std::unique_ptr<lemon::ListDigraph>, std::unique_ptr<lemon::ListDigraph::NodeMap<std::tuple<uint32_t, uint32_t, char>>>, std::unique_ptr<lemon::ListDigraph::ArcMap<int>>, lemon::ListDigraph::Node, lemon::ListDigraph::Node> read_file(std::string in_file)
{
	std::tuple<std::vector<std::string>, std::unique_ptr<lemon::ListDigraph>, std::unique_ptr<lemon::ListDigraph::NodeMap<std::tuple<uint32_t, uint32_t, char>>>, std::unique_ptr<lemon::ListDigraph::ArcMap<int>>, lemon::ListDigraph::Node, lemon::ListDigraph::Node> ret;

	std::get<1>(ret) = std::make_unique<lemon::ListDigraph>();
	std::get<2>(ret) = std::make_unique<lemon::ListDigraph::NodeMap<std::tuple<uint32_t, uint32_t, char>>>(*std::get<1>(ret));
	std::get<3>(ret) = std::make_unique<lemon::ListDigraph::ArcMap<int>>(*std::get<1>(ret));

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		std::get<0>(ret).push_back(line);
	}

	std::vector<std::vector<std::array<lemon::ListDigraph::Node, 4>>> node_matrix(std::get<0>(ret).size(), std::vector<std::array<lemon::ListDigraph::Node, 4>>(std::get<0>(ret)[0].size(), std::array<lemon::ListDigraph::Node, 4>()));

	for(uint32_t x = 0; x < std::get<0>(ret)[0].size(); x++)
	{
		for(uint32_t y = 0; y < std::get<0>(ret).size(); y++)
		{
			if(std::get<0>(ret)[y][x] == '.' || std::get<0>(ret)[y][x] == 'S' || std::get<0>(ret)[y][x] == 'E')
			{
				// space in lab, so add as node
				const std::array<char, 4> orientation{'n', 'e', 's', 'w'};
				for(uint32_t i = 0; i < orientation.size(); i++)
				{
					node_matrix[y][x][i] = std::get<1>(ret)->addNode();
					(*std::get<2>(ret))[node_matrix[y][x][i]] = std::tuple<uint32_t, uint32_t, char>(x, y, orientation[i]);
				}

				auto arc_ne = std::get<1>(ret)->addArc(node_matrix[y][x][0], node_matrix[y][x][1]);
				auto arc_en = std::get<1>(ret)->addArc(node_matrix[y][x][1], node_matrix[y][x][0]);
				auto arc_nw = std::get<1>(ret)->addArc(node_matrix[y][x][0], node_matrix[y][x][3]);
				auto arc_wn = std::get<1>(ret)->addArc(node_matrix[y][x][3], node_matrix[y][x][0]);

				auto arc_se = std::get<1>(ret)->addArc(node_matrix[y][x][2], node_matrix[y][x][1]);
				auto arc_es = std::get<1>(ret)->addArc(node_matrix[y][x][1], node_matrix[y][x][2]);
				auto arc_sw = std::get<1>(ret)->addArc(node_matrix[y][x][2], node_matrix[y][x][3]);
				auto arc_ws = std::get<1>(ret)->addArc(node_matrix[y][x][3], node_matrix[y][x][2]);

				if(std::get<0>(ret)[y][x] != 'E')
				{
					(*std::get<3>(ret))[arc_ne] = 1000*weight_scale;
					(*std::get<3>(ret))[arc_en] = 1000*weight_scale;
					(*std::get<3>(ret))[arc_nw] = 1000*weight_scale;
					(*std::get<3>(ret))[arc_wn] = 1000*weight_scale;

					(*std::get<3>(ret))[arc_se] = 1000*weight_scale;
					(*std::get<3>(ret))[arc_es] = 1000*weight_scale;
					(*std::get<3>(ret))[arc_sw] = 1000*weight_scale;
					(*std::get<3>(ret))[arc_ws] = 1000*weight_scale;
				}
				else
				{
					(*std::get<3>(ret))[arc_ne] = 0;
					(*std::get<3>(ret))[arc_en] = 0;
					(*std::get<3>(ret))[arc_nw] = 0;
					(*std::get<3>(ret))[arc_wn] = 0;

					(*std::get<3>(ret))[arc_se] = 0;
					(*std::get<3>(ret))[arc_es] = 0;
					(*std::get<3>(ret))[arc_sw] = 0;
					(*std::get<3>(ret))[arc_ws] = 0;
				}

				if(std::get<0>(ret)[y][x] == 'S')
				{
					// start node is always in east node group
					std::get<4>(ret) = node_matrix[y][x][1];
				}
				else if(std::get<0>(ret)[y][x] == 'E')
				{
					std::get<5>(ret) = node_matrix[y][x][0]; // rotation costs here have to be zero
				}
			}
		}
	}

	for(uint32_t x = 0; x < std::get<0>(ret)[0].size(); x++)
	{
		for(uint32_t y = 0; y < std::get<0>(ret).size(); y++)
		{
			if(std::get<0>(ret)[y][x] == '.' || std::get<0>(ret)[y][x] == 'E' || std::get<0>(ret)[y][x] == 'S')
			{
				// for all neighbors check if they are '.' too and then add the connections to the graph
				if(x > 0)
				{
					if(std::get<0>(ret)[y][x-1] == '.' || std::get<0>(ret)[y][x-1] == 'E' || std::get<0>(ret)[y][x-1] == 'S')
					{
						auto arc_con = std::get<1>(ret)->addArc(node_matrix[y][x][3], node_matrix[y][x-1][3]);
						(*std::get<3>(ret))[arc_con] = weight_scale;
					}
				}
				if(x < std::get<0>(ret)[0].size()-1)
				{
					if(std::get<0>(ret)[y][x+1] == '.' || std::get<0>(ret)[y][x+1] == 'E' || std::get<0>(ret)[y][x+1] == 'S')
					{
						auto arc_con = std::get<1>(ret)->addArc(node_matrix[y][x][1], node_matrix[y][x+1][1]);
						(*std::get<3>(ret))[arc_con] = weight_scale;
					}
				}
				if(y > 0)
				{
					if(std::get<0>(ret)[y-1][x] == '.' || std::get<0>(ret)[y-1][x] == 'E' || std::get<0>(ret)[y-1][x] == 'S')
					{
						auto arc_con = std::get<1>(ret)->addArc(node_matrix[y][x][0], node_matrix[y-1][x][0]);
						(*std::get<3>(ret))[arc_con] = weight_scale;
					}
				}
				if(y < std::get<0>(ret).size()-1)
				{
					if(std::get<0>(ret)[y+1][x] == '.' || std::get<0>(ret)[y+1][x] == 'E' || std::get<0>(ret)[y+1][x] == 'S')
					{
						auto arc_con = std::get<1>(ret)->addArc(node_matrix[y][x][2], node_matrix[y+1][x][2]);
						(*std::get<3>(ret))[arc_con] = weight_scale;
					}
				}
			}
		}
	}

	return ret;
}

static uint32_t task1(std::tuple<std::vector<std::string>, std::unique_ptr<lemon::ListDigraph>, std::unique_ptr<lemon::ListDigraph::NodeMap<std::tuple<uint32_t, uint32_t, char>>>, std::unique_ptr<lemon::ListDigraph::ArcMap<int>>, lemon::ListDigraph::Node, lemon::ListDigraph::Node> &input)
{
	lemon::Dijkstra<lemon::ListDigraph> dijkstra(*std::get<1>(input), *std::get<3>(input));
	dijkstra.run(std::get<4>(input));

	auto dist = dijkstra.dist(std::get<5>(input));

	return dist / weight_scale;
}

static uint32_t task2(std::tuple<std::vector<std::string>, std::unique_ptr<lemon::ListDigraph>, std::unique_ptr<lemon::ListDigraph::NodeMap<std::tuple<uint32_t, uint32_t, char>>>, std::unique_ptr<lemon::ListDigraph::ArcMap<int>>, lemon::ListDigraph::Node, lemon::ListDigraph::Node> &input)
{
	lemon::Dijkstra<lemon::ListDigraph> dijkstra(*std::get<1>(input), *std::get<3>(input));
	dijkstra.run(std::get<4>(input));

	int32_t shortest_dist = dijkstra.dist(std::get<5>(input));

	// store all nodes from this dijkstra into a list
	std::vector<lemon::ListDigraph::Node> node_list;
	for(auto last_node = std::get<5>(input); last_node != lemon::INVALID; last_node = dijkstra.predNode(last_node))
	{
		node_list.push_back(last_node);
	}
	std::sort(node_list.begin(), node_list.end());

	std::vector<lemon::ListDigraph::Node> unified_node_list;
	std::copy(node_list.cbegin(), node_list.cend(), std::back_inserter(unified_node_list));

	while(true)
	{
		auto last_node = std::get<5>(input);
		auto last_arc = dijkstra.predMap()[last_node];

		bool found_another_path = false;

		// increase the last path weights by just a bit (see scale), so that dijkstra finds another path with the same cost but not visited yet
		while(last_arc != lemon::INVALID)
		{
			// for all arcs in the shortest path

			// first increase arc distance
			(*std::get<3>(input))[last_arc] += 1;

			last_node = std::get<1>(input)->source(last_arc);
			last_arc = dijkstra.predMap()[last_node];
		}

		// then search for shortest path with these new arc weights
		dijkstra = lemon::Dijkstra<lemon::ListDigraph>(*std::get<1>(input), *std::get<3>(input));
		dijkstra.run(std::get<4>(input));

		if(dijkstra.dist(std::get<5>(input)) < shortest_dist+weight_scale) // just below weight_scale is ok since this is still the shortest path
		{
			// so this is an alternative

			// store all nodes from this dijkstra into a list
			std::vector<lemon::ListDigraph::Node> new_node_list;
			for(auto last_node = std::get<5>(input); last_node != lemon::INVALID; last_node = dijkstra.predNode(last_node))
			{
				new_node_list.push_back(last_node);
			}

			std::sort(new_node_list.begin(), new_node_list.end());

			// now check for difference of the two node lists
			if(!std::includes(unified_node_list.begin(), unified_node_list.end(), new_node_list.begin(), new_node_list.end()))
			{
				std::copy(new_node_list.cbegin(), new_node_list.cend(), std::back_inserter(unified_node_list));

				node_list.clear();
				std::copy(new_node_list.cbegin(), new_node_list.cend(), std::back_inserter(node_list));

				found_another_path = true;
			}
		}

		// abort if there is no new shortest path
		if(!found_another_path)
			break;
	}

	std::sort(unified_node_list.begin(), unified_node_list.end());
	unified_node_list.erase(std::unique(unified_node_list.begin(), unified_node_list.end()), unified_node_list.end());

	// for all of these only take the coordinates
	std::vector<std::pair<uint32_t, uint32_t>> locations;
	std::transform(unified_node_list.cbegin(), unified_node_list.cend(), std::back_inserter(locations),
			       [&](const lemon::ListDigraph::Node &node)
	{
		auto tuple = (*std::get<2>(input))[node];
		return std::make_pair(std::get<0>(tuple), std::get<1>(tuple));
	});

	std::sort(locations.begin(), locations.end());
	locations.erase(std::unique(locations.begin(), locations.end()), locations.end());

//	// draw for debug
//	for(auto loc : locations)
//	{
//		std::get<0>(input)[loc.second][loc.first] = 'O';
//	}
//
//	for(auto line : std::get<0>(input))
//		std::cout << line << std::endl;

	return locations.size();
}

void day16()
{
	std::tuple<std::vector<std::string>, std::unique_ptr<lemon::ListDigraph>, std::unique_ptr<lemon::ListDigraph::NodeMap<std::tuple<uint32_t, uint32_t, char>>>, std::unique_ptr<lemon::ListDigraph::ArcMap<int>>, lemon::ListDigraph::Node, lemon::ListDigraph::Node> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day16_real_input.txt");

	uint32_t task1_res = task1(input);
	uint32_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
