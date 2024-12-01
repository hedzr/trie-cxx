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
// Created by Hedzr Yeh on 2024/9/19.
//

// #include <catch2/catch.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// For more information about catch2, go and surfing:
//   https://github.com/catchorg/Catch2/blob/devel/docs/Readme.md
//   https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md

SCENARIO("vectors can be sized and resized (bdd)", "[vector][bdd-style]") {
	GIVEN("A vector with some items") {
		std::vector<int> v(5);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 5);

		WHEN("the size is increased") {
			v.resize(10);

			THEN("the size and capacity change") {
				REQUIRE(v.size() == 10);
				REQUIRE(v.capacity() >= 10);
			}
		}
		WHEN("the size is reduced") {
			v.resize(0);

			THEN("the size changes but not capacity") {
				REQUIRE(v.size() == 0);
				REQUIRE(v.capacity() >= 5);
			}
		}
		WHEN("more capacity is reserved") {
			v.reserve(10);

			THEN("the capacity changes but not the size") {
				REQUIRE(v.size() == 5);
				REQUIRE(v.capacity() >= 10);
			}
		}
		WHEN("less capacity is reserved") {
			v.reserve(0);

			THEN("neither size nor capacity are changed") {
				REQUIRE(v.size() == 5);
				REQUIRE(v.capacity() >= 5);
			}
		}
	}
}

TEST_CASE("Testcase: vectors can be sized and resized (tdd)", "[vector][tdd-style]") {
	std::vector<int> v(5);

	REQUIRE(v.size() == 5);
	REQUIRE(v.capacity() >= 5);

	SECTION("resizing bigger changes size and capacity") {
		v.resize(10);

		REQUIRE(v.size() == 10);
		REQUIRE(v.capacity() >= 10);
	}
	SECTION("resizing smaller changes size but not capacity") {
		v.resize(0);

		REQUIRE(v.size() == 0);
		REQUIRE(v.capacity() >= 5);
	}
	SECTION("reserving bigger changes capacity but not size") {
		v.reserve(10);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 10);
	}
	SECTION("reserving smaller does not change size or capacity") {
		v.reserve(0);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 5);
	}
}
