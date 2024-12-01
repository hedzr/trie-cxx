/*
 * @copy Copyright © 2016 - 2024 Hedzr Yeh.
 *
 * trie - C++17/C++20 Text Difference Utilities Library
 *
 * This file is part of trie.
 *
 * trie is free software: you can redistribute it and/or modify
 * it under the terms of the Apache 2.0 License.
 * Read /LICENSE for more information.
 */

//
// Created by Hedzr Yeh on 2023/11/8.
//

#include <iomanip>
#include <iostream>

#include "trie-cxx.hh"

// https://leetcode.com/problems/strong-password-checker/
class Solution {
public:
	int strongPasswordChecker(std::string password) {
		(void) password;
		return 0;
	}
};


int main(int argc, char *argv[]) {
	std::cout << "Hello, World!" << '\n';

	if (argc > 1) {
		Solution solve;
		std::cout << solve.strongPasswordChecker(std::string((const char *) (argv[1]))) << '\n';
	}
}