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

#include "trie-cxx.hh"

#include <iomanip>
#include <iostream>

namespace {
} // namespace

int main() {
	std::cout << "Hello, World!" << '\n'
			// << "I was built by " << trie::cross::compiler_name() << '\n'
			<< "__cplusplus = 0x" << std::hex << std::setfill('0') << std::setw(8) << __cplusplus << ' ' << '(' << std::dec <<
			__cplusplus << ')' << '\n';

	// test_hamming_code();
	// test_levenshtein();
	// test_jaro_winkler();
}
