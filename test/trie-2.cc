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
// Created by Hedzr Yeh on 2024/9/16.
//

#include <random>
#include <cstdint>
#include <cstddef>
#include <stdint.h>

// #include "trie-cxx/trie-base.hh"
// #include "trie-cxx/trie-chrono.hh"
#include "trie-cxx/trie-core.hh"


// #include <catch2/catch.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// For more information about catch2, go and surfing:
//   https://github.com/catchorg/Catch2/blob/devel/docs/Readme.md
//   https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md
// About its commandline arguments, see:
//   https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md
// About BDD style, see:
//   https://dannorth.net/introducing-bdd/


#if !defined(M_PI)
#define M_PI        3.14159265358979323846264338327950288   /* pi             */
#endif

namespace trie::tests {
	inline auto build_minimal_trie() -> trie::trie_t<trie::value_t> {
		trie::trie_t<trie::value_t> tt;

		tt.insert("app.debug", true);
		tt.insert("app.verbose", true);
		tt.insert("app.dump", 3);
		tt.insert("app.logging.file", "~/.trie.log");
		tt.insert("app.server.start", 5);
		tt.insert("app.logging.rotate", 6);
		// tt.insert("app.logging.words", std::vector<std::string>{"a", 1, false});
		tt.insert("app.logging.words", std::vector<std::string>{"a", "1", "false"});
		tt.insert("app.server.sites", 1);

		return tt;
	}

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

	auto get_trie(int choice = 0) {
		switch (choice) {
			case 0:
				return build_minimal_trie();
			case 1:
				return build_trie();
			default:
				return build_trie();
		}
	}

	auto test_value_t() -> void {
#define TRY_VALUE_T(expr)                                                                                            \
	{                                                                                                                  \
		trie::ios_fmt_saver sav{std::cout};                                                                              \
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

	void test1(int choice = 1) {
		auto tt = get_trie(choice);

		tt.remove("app.logging.words");
		tt.dump(std::cout);

		tt.insert("app.logging.words", std::vector<std::string>{"a", "1", "false"});

		auto val = std::vector<std::string>{"b", "2", "true"};
		tt.insert("app.logging.words", std::move(val));

		tt.dump(std::cout);
	}

	void test2(int choice = 1) {
		auto tt = get_trie(choice);

		tt.remove("app.logging.");
		tt.dump(std::cout);
	}

	void test3(int choice = 1) {
		auto tt = get_trie(choice);

		tt.remove("app.logging.");
		tt.dump(std::cout);
	}

	void test5_bench_insert(int choice = 1) {
		trie::trie_t<trie::value_t> tt;

		// const int MAX_INSERTS = 10000000;
		const int MAX_INSERTS = 2000;
		const int MAX_INSERTS_FLAG = MAX_INSERTS - 100;
		char kp[128] = "app.logging.file.interval";
		auto size = std::strlen(kp);
		char const *alphabet = "abcdefghijklmnopqrstuvwxyz.";
		auto sizea = std::strlen(alphabet);

		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist6(4, (unsigned int) size);
		// distribution in range [4, size]
		std::uniform_int_distribution<std::mt19937::result_type> alpha(0, (unsigned int) sizea);
		std::uniform_int_distribution<std::mt19937::result_type> value(1, 32767);

		std::vector<std::string> keys;
		keys.reserve(100);
		(void) (choice); // UNUSED(choice);

		{
			// trie::chrono::high_res_duration hrd([&tt, &keys](auto duration) -> bool {
			// 	auto const tt_size = tt.size();
			// 	auto const keys_size = keys.size();
			// 	auto const op = duration / tt_size;
			// 	std::cout << "tt contains " << tt_size << " leaves." << '\n';
			// 	std::cout << "   keys.tt_size() = " << keys_size << '\n';
			// 	std::cout << "test5_bench_insert<1>: It took " << duration << ", each insert takes " << op << "." << '\n';
			// 	return false;
			// });

			trie::chrono::timer tr([&tt, &keys](auto duration) -> bool {
				auto const tt_size = tt.size();
				auto const keys_size = keys.size();
				auto const dur = duration * 1000 * 1000;
				auto const op = dur / tt_size;
				std::cout << "tt contains " << tt_size << " leaves." << '\n';
				std::cout << "   keys.tt_size() = " << keys_size << '\n';
				std::cout << "test5_bench_insert<1.ns>: It took " << dur << "ns, each insert takes " << op << "ns." << '\n';
				return false;
			});

			for (int i = 0; i < MAX_INSERTS; i++) {
				auto const ix1 = dist6(rng);
				// while (kp[ix1] == '.') ix1 = dist6(rng);
				auto const ix2 = alpha(rng);
				kp[ix1] = alphabet[ix2];

				tt.insert(kp, (int) value(rng));

				if (i >= MAX_INSERTS_FLAG)
					keys.push_back(std::string(kp));
			}
		}

		const int MAX_FINDS = 3000;
		const bool find_test{false};
		if (find_test) {
			trie::chrono::high_res_duration hrd([MAX_FINDS, &keys](auto duration) -> bool {
				std::cout << "test5_bench_find<2>: It took " << duration << ", each find/locate takes " << (duration / MAX_FINDS / keys.size() / 2) << "." << '\n';
				return false;
			});
			for (int i = 0; i < MAX_FINDS; i++) {
				for (auto const &key : keys) {
					auto ret = tt.locate(key.c_str());
					auto ret1 = tt.find(key.c_str());
					(void) ret;
					(void) ret1; // UNUSED(ret, ret1);
				}
			}
		}
		{
			int failed_fast_find{0};

			// trie::chrono::high_res_duration hrd([MAX_FINDS, &keys, failed_fast_find](auto duration) -> bool {
			// 	auto const keys_size = keys.size();
			// 	auto const op = duration / MAX_FINDS / keys_size;
			// 	std::cout << "test5_bench_fast_find<2>: It took " << duration << ", each fast_find() takes " << op << ". (failed: " << failed_fast_find << ")" << '\n';
			// 	return false;
			// });

			trie::chrono::timer tr([failed_fast_find, &keys](auto duration) -> bool {
				auto const keys_size = keys.size();
				auto const dur = duration * 1000 * 1000;
				auto const op = dur / MAX_FINDS / keys_size;
				std::cout << "test5_bench_fast_find<2.ns>: It took " << dur << "ns, each fast_find() takes " << op << "ns.";
				if (failed_fast_find > 0)
					std::cout << " (failed: " << failed_fast_find << ")";
				std::cout << '\n';
				return false;
			});

			for (int i = 0; i < MAX_FINDS; i++) {
				for (auto const &key : keys) {
					auto ret = tt.fast_find(key.c_str());
					(void) ret;
					if (!ret.matched)
						failed_fast_find++;
				}
			}
		}
	}
} // namespace trie::tests

int main_fn(int argc, char *argv[]) {
	auto choice{1};

	if (argc > 1) {
		auto t = argv[1];
		if (*t >= '0' && *t < '9') choice = *t - '0';
	}

	using namespace trie::tests;
	test_value_t();
	test1(choice);
	test2(choice);
	test3(choice);
	test5_bench_insert(choice);
	return 0;
}

SCENARIO("trie/store: initial", "[trie][.initial]") {
	// initial tests here

	// using trie::terminal::colors::colorize;
	// colorize c;
	// std::cout << c.bold().s("test_btree_2") << '\n';

	trie::chrono::high_res_duration hrd([](auto duration) -> bool {
		static_assert(trie::chrono::is_duration<decltype(duration)>::value == true);
		std::cout << "It took " << duration << '\n';
		return false;
	});
}

SCENARIO("trie/store: build a basic store", "[trie][normal]") {
	using strvec = std::vector<std::string>;
	using namespace trie::tests;
	auto tt = build_minimal_trie(); // these common codes will be re-exec'd before each GIVEN starting

	GIVEN("do sth, dump it, 1") {
		auto rt = tt.remove("app.logging.");
		REQUIRE(rt.ok == true);
		// std::cout << "---- GIVEN(\"do sth, dump it, 1\")" << '\n';
		// tt.dump(std::cout);
	}

	GIVEN("a test store, normal tests") {
		// in each GIVEN, tt will be reset to original.
		// tt.dump(std::cout);
		REQUIRE(tt.size() >= 8);
		REQUIRE(tt.has("app.logging.words") == true);

		WHEN("remove a leaf node") {
			tt.remove("app.logging.words");
			// tt.dump(std::cout);
			REQUIRE(tt.has("app.logging.words") == false);
		}

		WHEN("reinsert it and read it") {
			tt.insert("app.logging.words", strvec{"a", "1", "false"});
			REQUIRE(tt.has("app.logging.words") == true);
		}

		WHEN("update it") {
			{
				auto &v = tt.get<strvec>("app.logging.words");
				REQUIRE(v[0] == "a");
				REQUIRE(v[1] == "1");
				REQUIRE(v[2] == "false");
			}

			auto val = strvec{"b", "2", "true"};
			tt.insert("app.logging.words", std::move(val));
			// std::cout << "---- after re-inserted 'app.logging.words'" << '\n';
			// tt.dump(std::cout);

			auto &v = tt.get<strvec>("app.logging.words");
			REQUIRE(v[0] == "b");
			REQUIRE(v[1] == "2");
			REQUIRE(v[2] == "true");
		}

		WHEN("remove a branch node") {
			tt.remove("app.logging");
			// std::cout << "---- after removed 'app.logging'" << '\n';
			// tt.dump(std::cout);
			REQUIRE(tt.has("app.logging.words") == false);
			REQUIRE(tt.has("app.logging.file") == false);
			REQUIRE(tt.has("app.logging") == false);
		}

		// tt.dump(std::cout);
	}

	GIVEN("do sth, dump it, 2") {
		auto rt = tt.remove("app.logging.");
		REQUIRE(rt.ok == true);
		// std::cout << "---- GIVEN(\"do sth, dump it, 2\")" << '\n';
		// tt.dump(std::cout);
	}
}

SCENARIO("trie/store: remove a branch node", "[trie][remove][remove-branch]") {
	using namespace trie::tests;
	auto tt = build_minimal_trie(); // these common codes will be re-exec'd before each GIVEN starting

	GIVEN("a test store") {
		// std::printf("tt.size = %zu\n", tt.size());
		std::cout << "tt.size = " << tt.size() << '\n';
		REQUIRE(tt.size() >= 8);

		// std::printf("tt.has(app.logging) = %d\n", tt.has("app.logging"));
		std::cout << "tt.has(app.logging) = " << std::boolalpha << tt.has("app.logging") << '\n';
		REQUIRE(tt.has("app.logging"));
		REQUIRE(tt.has("app.logging."));
		REQUIRE(tt.has("app.logging.file"));
	}
}

SCENARIO("trie/store: unit tests", "[trie][unit-test]") {
	using namespace trie::tests;
	auto tt = build_minimal_trie(); // these common codes will be re-exec'd before each GIVEN starting
	using trie_t = decltype(tt);

	// std::printf("tt.size = %zu\n", tt.size());
	std::cout << "tt.size = " << tt.size() << '\n';
	REQUIRE(tt.size() == 8);

	GIVEN("a test store") {
		// std::printf("tt.has(app.logging) = %d\n", tt.has("app.logging"));
		std::cout << "tt.has(app.logging) = " << std::boolalpha << tt.has("app.logging") << '\n';
		REQUIRE(tt.has("app.logging"));
		REQUIRE(tt.has("app.logging."));
		REQUIRE(tt.has("app.logging.file"));
	}
	GIVEN("search a key path and update it") {
		trie_t::weak_node_ptr wp = tt.get("app.logging");
		if (auto sp = wp.lock()) {
			REQUIRE(sp->path() == "app.logging.");
		}
		wp = tt.get("app.logging.file");
		if (auto sp = wp.lock()) {
			REQUIRE(sp->path() == "app.logging.file");
			REQUIRE(std::get<char const *>(sp->value()) == std::string("~/.trie.log"));

			sp->value((char const *) "wow");
			REQUIRE(std::get<char const *>(sp->value()) == std::string("wow"));

			tt.set("app.logging.file", (char const *) "hello");
			REQUIRE(std::get<char const *>(sp->value()) == std::string("hello"));
		}
	}
	GIVEN("get value from a key path and update it") {
		trie_t::weak_node_ptr wp = tt.get("app.logging.file");
		if (auto sp = wp.lock()) {
			if (sp) {
				REQUIRE(sp->path() == "app.logging.file");
				REQUIRE(std::get<char const *>(sp->value()) == std::string("~/.trie.log"));

				sp->value((char const *) "wow");
				REQUIRE(std::get<char const *>(sp->value()) == std::string("wow"));

				tt.set("app.logging.file", (char const *) "hello");
				REQUIRE(std::get<char const *>(sp->value()) == std::string("hello"));
			}
		}
		auto const &val = tt.get<char const *>("app.logging.file");
		REQUIRE(val == std::string("hello"));
	}
	GIVEN("set value with different types") {
		auto wp = tt.get("app.dump");
		if (auto sp = wp.lock()) {
			if (sp) {
				REQUIRE(sp->path() == "app.dump");
				REQUIRE(std::get<int>(sp->value()) == 3);

				sp->value((char const *) "wow");
				REQUIRE(std::get<char const *>(sp->value()) == std::string("wow"));

				tt.set("app.dump", true);
				REQUIRE(std::get<bool>(sp->value()) == true);
			}
		}

		auto const &val = tt.get<char const *>("app.logging.file");
		REQUIRE(val == std::string("~/.trie.log"));
	}
	GIVEN("about extension package, and different delimiter char") {
		using describable_store = trie::trie_t<
		        trie::value_t, '/',
		        trie::extensions::description_holder<>>;
		describable_store ds;

		ds.insert("app/home/ops", 1);
		ds.insert("app/home/admin", false);
		auto wp = ds.get("app/home/ops");
		if (auto sp = wp.lock()) {
			if (sp) {
				sp->desc("str");
				REQUIRE(sp->desc() == "str");

				REQUIRE(ds.has("app/h", true));
				REQUIRE(ds.has("app/home"));
				REQUIRE(ds.has("app/home/"));
			}
		}
		wp = ds.get("app/home");
		if (auto sp = wp.lock()) {
			if (sp) {
				REQUIRE(sp->type() == describable_store::node_type::NODE_BRANCH);
			}
		}
	}
	GIVEN("about extension package, more") {
		using describable_store = trie::trie_t<
		        trie::value_t, '/',
		        trie::extensions::description_holder<>,
		        trie::extensions::comment_holder<>,
		        trie::extensions::tag_holder<>>;
		describable_store ds;

		ds.insert("app/home/ops", 1);
		ds.insert("app/home/admin", false);
		auto wp = ds.get("app/home/ops");
		if (auto sp = wp.lock()) {
			if (sp) {
				sp->desc("str");
				REQUIRE(sp->desc() == "str");

				REQUIRE(ds.has("app/h", true));
				REQUIRE(ds.has("app/home"));
				REQUIRE(ds.has("app/home/"));
			}
		}
		wp = ds.get("app/home");
		if (auto sp = wp.lock()) {
			if (sp) {
				REQUIRE(sp->type() == describable_store::node_type::NODE_BRANCH);
			}
		}
	}
}

// int main() {
//
// 	using namespace trie::tests;
// 	test1();
//
// 	return 0;
// }
