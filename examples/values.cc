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

auto build_trie() -> trie::trie_t<trie::value_t> {
	trie::trie_t<trie::value_t> tt;

	tt.set("app.debug", true);
	tt.set("app.verbose", true);
	tt.set("app.dump", 3);
	tt.set("app.logging.file", "~/.trie.log");
	tt.set("app.server.start", 5);
	tt.set("app.logging.rotate", (unsigned long) (6));
	// tt.set("app.logging.words", std::vector<std::string>{"a", 1, false});
	tt.set("app.logging.words", std::vector<std::string>{"a", "1", "false"});
	tt.set("app.server.sites", 1);
	float f = 2.718f;
	tt.set("app.ref-types.float", std::move(f));
	tt.set("app.ref-types.double", M_PI);
	tt.set("app.ref-types.string", std::string("hello, trie."));
	tt.set("app.ref-types.intvec", std::vector<int>{3, 5, 7, 9});

	return tt;
}

auto test_value_t() -> void {
#define TRY_VALUE_T(expr)                                                                                            \
	{                                                                                                                  \
		trie::value_t v1{expr};                                                                                          \
		std::cout << ".. assigning " << std::setfill(' ') << std::right << std::setw(21) << #expr << ": " << v1 << '\n'; \
	}

	using namespace std::literals::chrono_literals; // c++14 or later

	// uint u1 = 7;
	TRY_VALUE_T(true);
	TRY_VALUE_T(false);
	TRY_VALUE_T('z');
	TRY_VALUE_T('9');
	TRY_VALUE_T(5);
	// TRY_VALUE_T(u1);
	// TRY_VALUE_T((unsigned int) 7);
	TRY_VALUE_T((int8_t) 21);
	TRY_VALUE_T((int16_t) 22);
	TRY_VALUE_T((int32_t) 23);
	TRY_VALUE_T((int64_t) 24);
	TRY_VALUE_T((uint8_t) 25);
	TRY_VALUE_T((uint16_t) 26);
	// TRY_VALUE_T((uint32_t) 27);
	// TRY_VALUE_T((uint64_t) 28);
	TRY_VALUE_T((long) 129);
	TRY_VALUE_T((long long) 130);
	TRY_VALUE_T((unsigned long) 131);
	// TRY_VALUE_T((unsigned long long) 136);
	TRY_VALUE_T((float) M_PI);
	TRY_VALUE_T((double) M_PI);
	TRY_VALUE_T((long double) M_PI);
	TRY_VALUE_T((double) M_PI);

	TRY_VALUE_T(5129ns);
	TRY_VALUE_T(59.739us);
	TRY_VALUE_T(5.625min);
	TRY_VALUE_T(89.843204843s);
	TRY_VALUE_T(5min);
	TRY_VALUE_T(1024h);

	TRY_VALUE_T("string");
	const char *s1 = "hello";
	TRY_VALUE_T(s1);
	std::string s2{s1};
	TRY_VALUE_T(s2);
}

int main(__unused int argc, __unused char *argv[]) {
	test_value_t();
}