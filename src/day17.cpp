/*
 * day17.cpp
 *
 *  Created on: 18.12.2024
 *      Author: liv
 */

#include "day17.h"

#include <chrono>
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <execution>
#include <array>
#include <algorithm>

#include <boost/algorithm/string.hpp>

static std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> read_file(std::string in_file)
{
	std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> ret;

	std::ifstream file(in_file);

	std::string line;

	std::getline(file, line);
	boost::erase_all(line, "Register A: ");
	std::get<0>(ret.first) = std::stoul(line);

	std::getline(file, line);
	boost::erase_all(line, "Register B: ");
	std::get<1>(ret.first) = std::stoul(line);

	std::getline(file, line);
	boost::erase_all(line, "Register C: ");
	std::get<2>(ret.first) = std::stoul(line);

	std::getline(file, line); // empty line
	std::getline(file, line);
	boost::erase_all(line, "Program: ");
	std::vector<uint32_t> list_of_nums;
	std::transform(boost::make_split_iterator(line, boost::token_finder(boost::is_any_of(","), boost::token_compress_off)),
					boost::split_iterator<std::string::iterator>(),
					std::back_inserter(list_of_nums),
					[](auto in)->uint32_t { return std::stoul(boost::copy_range<std::string>(in)); });
	for(uint32_t i = 0; i < list_of_nums.size(); i ++)
	{
		ret.second.push_back(list_of_nums[i]);
	}

	return ret;
}

class handheld_computer
{
public:
	handheld_computer(const std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> &input);

	void execute_program();

	std::string get_output();
	std::vector<uint_fast8_t> get_vec_output() { return out; };

	bool compare_output_with_program();

	void set_pc_and_a(uint_fast32_t pc_, uint_fast64_t a_);
	void clear_out();
private:
	void execute_instruction();

	uint_fast32_t pc;
	uint_fast64_t a;
	uint_fast64_t b;
	uint_fast64_t c;

	std::vector<uint_fast8_t> opcode_and_operand;

	std::vector<uint_fast8_t> out;
};

handheld_computer::handheld_computer(const std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> &input)
{
	pc = 0;
	a = std::get<0>(input.first);
	b = std::get<1>(input.first);
	c = std::get<2>(input.first);
	opcode_and_operand = input.second;
	out.reserve(32);
}

void handheld_computer::execute_instruction()
{
	uint_fast8_t opcode = opcode_and_operand[pc];
	uint_fast8_t literal_operand = opcode_and_operand[pc+1];

	uint_fast64_t combo_operand;
	if(literal_operand <= 3)
		combo_operand = literal_operand;
	else if(literal_operand == 4)
		combo_operand = a;
	else if(literal_operand == 5)
		combo_operand = b;
	else if(literal_operand == 6)
		combo_operand = c;
	else
		combo_operand = 0; // should never be reached for a combo operand

	bool increase_pc = true;

	if(opcode == 0) // adv
	{
		a = a / (1<<combo_operand);
	}
	else if(opcode == 1) // bxl
	{
		b = b ^ literal_operand;
	}
	else if(opcode == 2) // bst
	{
		b = combo_operand&0x07;
	}
	else if(opcode == 3) // jnz
	{
		if(a != 0)
		{
			pc = literal_operand;
			increase_pc = false;
		}
	}
	else if(opcode == 4) // bxc
	{
		b = b ^ c;
	}
	else if(opcode == 5) // out
	{
		uint_fast32_t res = combo_operand&0x07;
		out.push_back(res);
	}
	else if(opcode == 6) // bdv
	{
		b = a / (1<<combo_operand);
	}
	else if(opcode == 7) // cdv
	{
		c = a / (1<<combo_operand);
	}
	else
		throw std::exception();

	if(increase_pc)
		pc += 2;
}

void handheld_computer::execute_program()
{
	while(pc < opcode_and_operand.size())
		execute_instruction();
}

std::string handheld_computer::get_output()
{
	std::string ret;
	for(auto num : out)
	{
		ret += std::to_string(num) + ",";
	}
	ret.pop_back(); // remove last comma

	return ret;
}

bool handheld_computer::compare_output_with_program()
{
	if(opcode_and_operand == out)
		return true;
	else
		return false;
}

void handheld_computer::set_pc_and_a(uint_fast32_t pc_, uint_fast64_t a_)
{
	pc = pc_;
	a = a_;
}

void handheld_computer::clear_out()
{
	out.clear();
}

uint64_t calc_next_step(const std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> &input, uint64_t a, const std::vector<uint_fast8_t> &expected_out)
{
	a = a<<6;

	for(uint32_t i = 0; i < 16384; i++)
	{
		handheld_computer computer(input);

		computer.set_pc_and_a(0, a+i);

		computer.execute_program();

		auto out = computer.get_vec_output();

		auto mis = std::mismatch(out.cbegin(), out.cend(), expected_out.cbegin(), expected_out.cend());
		if(mis.first == out.cend() && mis.second == expected_out.cend())
			return a+i;
	}
	throw std::exception(); // should not happen
	return 0;
}

static std::string task1(const std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> &input)
{
	handheld_computer computer(input);

	computer.execute_program();

	return computer.get_output();
}

static uint64_t task2(const std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> &input)
{
	uint64_t a = 0;
	for(int32_t i = input.second.size()-1; i > 0; i -= 2)
	{
		std::vector<uint8_t> out_to_test;
		std::copy(input.second.begin()+i-1, input.second.end(), std::back_inserter(out_to_test));
		a = calc_next_step(input, a, out_to_test);
//		std::cout << "a = " << a << std::endl;

//		handheld_computer computer(input);
//		computer.set_pc_and_a(0, a);
//		computer.execute_program();
//		std::cout << computer.get_output() << std::endl;
	}

	return a;
}

void day17()
{
	std::pair<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<uint_fast8_t>> input;

	auto t1 = std::chrono::steady_clock::now();

	input = read_file("day17_real_input.txt");

	std::string task1_res = task1(input);
	uint64_t task2_res = task2(input);

	auto t2 = std::chrono::steady_clock::now();

	std::cout << "result task 1: " << task1_res << std::endl;
	std::cout << "result task 2: " << std::to_string(task2_res) << std::endl;
	std::cout << "took: " << std::chrono::duration<double>(t2-t1).count() << "s" << std::endl;
}
