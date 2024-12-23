/*
 * day14.cpp
 *
 *  Created on: 22.12.2024
 *      Author: liv
 */

#include "day14.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <numeric>
#include <cmath>

#include <eigen3/Eigen/Dense>

#include <boost/algorithm/string.hpp>

static std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> read_file(std::string in_file)
{
	std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> ret;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		std::vector<std::string> pos_and_vel;

		boost::split(pos_and_vel, line, boost::is_space());

		boost::erase_all(pos_and_vel[0], "p=");
		boost::erase_all(pos_and_vel[1], "v=");

		std::vector<std::string> pos_nums;
		boost::split(pos_nums, pos_and_vel[0], boost::is_any_of(","));

		std::vector<std::string> vel_nums;
		boost::split(vel_nums, pos_and_vel[1], boost::is_any_of(","));

		std::pair<Eigen::Vector2i, Eigen::Vector2i> robot;
		robot.first = Eigen::Vector2i(std::stoul(pos_nums[0]), std::stoul(pos_nums[1]));
		robot.second = Eigen::Vector2i(std::stoul(vel_nums[0]), std::stoul(vel_nums[1]));
		ret.push_back(robot);
	}

	return ret;
}

void move_robot(std::pair<Eigen::Vector2i, Eigen::Vector2i> &robot, const Eigen::Vector2i size)
{
	robot.first += robot.second;

	if(robot.first.x() < 0)
		robot.first.x() += size.x();
	if(robot.first.x() >= size.x())
		robot.first.x() -= size.x();
	if(robot.first.y() < 0)
		robot.first.y() += size.y();
	if(robot.first.y() >= size.y())
		robot.first.y() -= size.y();
}

uint32_t calculate_safety_factor(const std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> &robots, const Eigen::Vector2i size)
{
	std::array<uint32_t, 4> quadrant_count{0,0,0,0};
	for(auto robot : robots)
	{
		// top left
		if((robot.first.x() < size.x()/2) && (robot.first.y() < size.y()/2))
		{
			quadrant_count[0]++;
		}
		else if((robot.first.x() < size.x()/2) && (robot.first.y() > size.y()/2))
		{
			quadrant_count[2]++;
		}
		else if((robot.first.x() > size.x()/2) && (robot.first.y() < size.y()/2))
		{
			quadrant_count[1]++;
		}
		else if((robot.first.x() > size.x()/2) && (robot.first.y() > size.y()/2))
		{
			quadrant_count[3]++;
		}
	}
	return std::reduce(quadrant_count.cbegin(), quadrant_count.cend(), 1, std::multiplies{});
}

static double calculate_mean(const std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> &input, bool y)
{
	double ret = 0;

	for(auto robot : input)
	{
		if(!y)
			ret += robot.first.x();
		else
			ret += robot.first.y();
	}

	return ret / input.size();
}

static double calculate_variance_x_y_sum(const std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> &input)
{
	double x_var = 0, y_var = 0;

	double x_mean = calculate_mean(input, false);
	double y_mean = calculate_mean(input, true);

	for(auto robot : input)
	{
		x_var += std::pow(robot.first.x() - x_mean, 2);
		y_var += std::pow(robot.first.y() - y_mean, 2);
	}

	x_var /= input.size();
	y_var /= input.size();

	return x_var + y_var;
}

static uint32_t task1(std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> input, Eigen::Vector2i size)
{
	for(uint32_t t_step = 0; t_step < 100; t_step++)
	{
		for(auto it = input.begin(); it != input.end(); it++)
			move_robot(*it, size);

//		// draw
//		std::vector<std::string> empty_array(size.y(), std::string(size.x(), '.'));
//		for(auto robot : input)
//		{
//			empty_array[robot.first.y()][robot.first.x()] = 'O';
//		}
//		for(auto line : empty_array)
//			std::cout << line << std::endl;
	}

	return calculate_safety_factor(input, size);
}

static uint32_t task2(std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> input, Eigen::Vector2i size)
{
	// find the min of var_x + var_y, since for the tree most robots need to be close to each other

	auto input_org = input;

	std::pair<double, uint32_t> min_var_time = {1e6, 0};

	for(uint32_t t_step = 0; t_step < 100000; t_step++)
	{
		for(auto it = input.begin(); it != input.end(); it++)
			move_robot(*it, size);

		double var = calculate_variance_x_y_sum(input);

		if(var < min_var_time.first)
		{
			min_var_time.first = var;
			min_var_time.second = t_step;

//			// draw
//			std::vector<std::string> empty_array(size.y(), std::string(size.x(), '.'));
//			for(auto robot : input)
//			{
//				empty_array[robot.first.y()][robot.first.x()] = 'O';
//			}
//			for(auto line : empty_array)
//				std::cout << line << std::endl;
		}
	}

	input = input_org;

	for(uint32_t t_step = 0; t_step < min_var_time.second+1; t_step++)
	{
		for(auto it = input.begin(); it != input.end(); it++)
			move_robot(*it, size);
	}

	// draw
	std::vector<std::string> empty_array(size.y(), std::string(size.x(), '.'));
	for(auto robot : input)
	{
		empty_array[robot.first.y()][robot.first.x()] = 'O';
	}
	for(auto line : empty_array)
		std::cout << line << std::endl;

	return min_var_time.second +1;
}

void day14()
{
	std::vector<std::pair<Eigen::Vector2i, Eigen::Vector2i>> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day14_real_input.txt");

	uint32_t task1_res = task1(input, Eigen::Vector2i(101, 103));
	uint32_t task2_res = task2(input, Eigen::Vector2i(101, 103));

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << task2_res << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
