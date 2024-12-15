/*
 * day13.cpp
 *
 *  Created on: 14.12.2024
 *      Author: liv
 */

#include "day13.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <fstream>
#include <numeric>

#include <boost/algorithm/string.hpp>
#include <eigen3/Eigen/Dense>
#include <boost/multiprecision/float128.hpp>
#include <boost/math/special_functions/round.hpp>

static std::vector<std::array<Eigen::Matrix<uint64_t, 2, 1>, 3>> read_file(std::string in_file, bool part2)
{
	std::vector<std::array<Eigen::Matrix<uint64_t, 2, 1>, 3>> ret;

	std::array<Eigen::Matrix<uint64_t, 2, 1>, 3> ret_piece;

	std::ifstream file(in_file);
	for(std::string line; std::getline(file, line);)
	{
		std::vector<std::string> caption_and_rest;
		boost::split(caption_and_rest, line, boost::is_any_of(":"));

		if(caption_and_rest.size() < 2)
			continue;
		else if(caption_and_rest[0] == "Button A")
		{
			std::vector<std::string> x_and_y;
			boost::split(x_and_y, caption_and_rest[1], boost::is_any_of(","));

			boost::trim(x_and_y[0]);
			boost::trim(x_and_y[1]);
			boost::erase_all(x_and_y[0], "X+");
			boost::erase_all(x_and_y[1], "Y+");

			ret_piece[0] = Eigen::Matrix<uint64_t, 2, 1>(std::stoul(x_and_y[0]), std::stoul(x_and_y[1]));
		}
		else if(caption_and_rest[0] == "Button B")
		{
			std::vector<std::string> x_and_y;
			boost::split(x_and_y, caption_and_rest[1], boost::is_any_of(","));

			boost::trim(x_and_y[0]);
			boost::trim(x_and_y[1]);
			boost::erase_all(x_and_y[0], "X+");
			boost::erase_all(x_and_y[1], "Y+");

			ret_piece[1] = Eigen::Matrix<uint64_t, 2, 1>(std::stoul(x_and_y[0]), std::stoul(x_and_y[1]));
		}
		else if(caption_and_rest[0] == "Prize")
		{
			std::vector<std::string> x_and_y;
			boost::split(x_and_y, caption_and_rest[1], boost::is_any_of(","));

			boost::trim(x_and_y[0]);
			boost::trim(x_and_y[1]);
			boost::erase_all(x_and_y[0], "X=");
			boost::erase_all(x_and_y[1], "Y=");

			ret_piece[2] = Eigen::Matrix<uint64_t, 2, 1>(std::stoul(x_and_y[0]), std::stoul(x_and_y[1]));
			if(part2)
				ret_piece[2] += Eigen::Matrix<uint64_t, 2, 1>{10000000000000, 10000000000000};
			ret.push_back(ret_piece);
		}
	}

	return ret;
}

static uint64_t task(const std::vector<std::array<Eigen::Matrix<uint64_t, 2, 1>, 3>> &in)
{
	return std::transform_reduce(in.cbegin(), in.cend(), (uint64_t) 0, std::plus{},
								 [](const std::array<Eigen::Matrix<uint64_t, 2, 1>, 3> &game)->uint64_t
	{
		Eigen::Matrix<boost::multiprecision::float128, 2, 1> a_v = game[0].cast<boost::multiprecision::float128>();
		Eigen::Matrix<boost::multiprecision::float128, 2, 1> b_v = game[1].cast<boost::multiprecision::float128>();
		Eigen::Matrix<boost::multiprecision::float128, 2, 1> res = game[2].cast<boost::multiprecision::float128>();

		// check if a and b are linear independent so this can work
		boost::multiprecision::float128 x_scl = a_v.x() / b_v.x();
		boost::multiprecision::float128 y_scl = a_v.y() / b_v.y();
		if(std::abs(static_cast<double>(x_scl - y_scl)) < 1.0e-3)
		{
			// linear dependent, so we have multiple possible results
			// if this appears need to program this
			throw std::exception();
		}
		else
		{
			// solve the equation system
			// a_v * a + b_v * b = res
			//
			// a_v_x	b_v_x	*		a		=	res_x
			// a_v_y	b_v_y			b			res_y
			// double is not enough here since the numbers get really big in part 2
			Eigen::Matrix<boost::multiprecision::float128, 2, 2> A{{a_v.x(), b_v.x()}, {a_v.y(), b_v.y()}};
			Eigen::Matrix<boost::multiprecision::float128, 2, 1> x;

			x = A.colPivHouseholderQr().solve(res);

			boost::multiprecision::float128 a = x(0);
			boost::multiprecision::float128 b = x(1);

			if((std::abs(static_cast<double>(boost::math::round(a) - a)) < 1.0e-9) && (std::abs(static_cast<double>(boost::math::round(b) - b)) < 1.0e-9))
			{
				// check if values are ints
				// -> unique solution

				uint64_t res = 3 * static_cast<uint64_t>(boost::math::round(a)) + 1 * static_cast<uint64_t>(boost::math::round(b));
				return res;
			}
			else
				return 0;
		}
	});
}

void day13()
{
	std::vector<std::array<Eigen::Matrix<uint64_t, 2, 1>, 3>> input;
	std::vector<std::array<Eigen::Matrix<uint64_t, 2, 1>, 3>> input2;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day13_real_input.txt", false);
	input2 = read_file("day13_real_input.txt", true);

	uint64_t task1_res = task(input);
	uint64_t task2_res = task(input2);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << std::to_string(task1_res) << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}


