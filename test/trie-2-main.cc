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
// Created by Hedzr Yeh on 2024/9/25.
//

#include "trie-cxx/trie-core.hh"

// #include "trie-cxx/trie-chrono.hh"

#include <random>

namespace trie::tests {
	inline trie::trie_t<trie::value_t> build_minimal_trie() {
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

	void test1() {
		auto tt = build_minimal_trie();

		tt.dump(std::cout);
	}

	void test2() {
		trie::chrono::timer tr([](auto duration) -> bool {
			auto const dur = duration * 1000 * 1000;
			std::cout << "test2<1>: It took " << dur << "ns." << '\n';
			return false;
		});

		auto tt = build_minimal_trie();

		tt.remove("app.logging.words");
		tt.dump(std::cout);

		tt.insert("app.logging.words", std::vector<std::string>{"a", "1", "false"});

		auto val = std::vector<std::string>{"b", "2", "true"};
		tt.insert("app.logging.words", std::move(val));
		// tt.insert("app.logging.words", std::vector<std::string>{"b", "2", "true"});

		tt.dump(std::cout);
	}

	void test3() {
		trie::chrono::timer tr([](auto duration) -> bool {
			auto const dur = duration * 1000 * 1000;
			std::cout << "test3<1>: It took " << dur << "ns." << '\n';
			return false;
		});

		auto tt = build_minimal_trie();

		tt.remove("app.logging.");
		tt.dump(std::cout);
	}

	void test5_bench_inserts(trie::trie_t<trie::value_t> &tt, std::vector<std::string> &keys, int MAX_INSERTS,
	                         int MAX_INSERTS_FLAG) {
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

		/*trie::chrono::high_res_duration hrd([&tt, &keys](auto duration) -> bool {
				auto const tt_size = tt.size();
				auto const keys_size = keys.size();
				auto const op = duration / tt_size;
				std::cout << "tt contains " << tt_size << " leaves." << '\n';
				std::cout << "   keys.tt_size() = " << keys_size << '\n';
				std::cout << "test5_bench_insert<1>: It took " << duration << ", each insert takes " << op << "." << '\n';
				return false;
			});*/

		trie::chrono::timer tr([&tt, &keys](auto duration) -> bool {
			auto const tt_size = tt.size();
			auto const keys_size = keys.size();
			auto const dur = duration * 1000 * 1000;
			auto const op = dur / tt_size;
			std::cout << "tt contains " << tt_size << " leaves." << '\n';
			std::cout << "   keys.tt_size() = " << keys_size << '\n';
			std::cout << "test5_bench_inserts<1.ns>: It took " << dur << "ns, each insert takes " << op << "ns." << '\n';
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

	void test5_bench_finds(trie::trie_t<trie::value_t> const &tt, std::vector<std::string> const &keys, int MAX_FINDS) {
		int failed_fast_find{0};
		/*trie::chrono::high_res_duration hrd([MAX_FINDS, &keys, failed_fast_find](auto duration) -> bool {
			auto const keys_size = keys.size();
			auto const op = duration / MAX_FINDS / keys_size;
			std::cout << "test5_bench_fast_find<2>: It took " << duration << ", each fast_find() takes " << op << ". (failed: " << failed_fast_find << ")" << '\n';
			return false;
		});*/

		auto const keys_size = keys.size();
		auto const factor = MAX_FINDS * keys_size;

		trie::chrono::timer tr([failed_fast_find, factor](auto duration) -> bool {
			auto const dur = duration * 1000 * 1000;
			auto const op = dur / factor;
			std::cout << "test5_bench_finds<2.ns>: It took " << dur << "ns, each fast_find() takes " << op <<
					"ns. (failed: " << failed_fast_find << ")" << '\n';
			return false;
		});

		for (int i = 0; i < MAX_FINDS; i++) {
			for (auto const &key: keys) {
				auto ret = tt.fast_find(key.c_str());
				if (!ret.matched)
					failed_fast_find++;
			}
		}
	}

	void test5_bench_inserts_and_finds() {
		trie::trie_t<trie::value_t> tt;

		// const int MAX_FINDS = 3000;
		constexpr int MAX_FINDS = 200000;
		constexpr bool find_test{false};

		// const int MAX_INSERTS = 10000000;
		constexpr int MAX_INSERTS = 80;
		constexpr int MAX_INSERTS_FLAG = MAX_INSERTS - 40;

		std::vector<std::string> keys;
		keys.reserve(100);

		test5_bench_inserts(tt, keys, MAX_INSERTS, MAX_INSERTS_FLAG);

		if (find_test) {
			/*trie::chrono::high_res_duration hrd([MAX_FINDS, &keys](auto duration) -> bool {
				std::cout << "test5_bench_find<2>: It took " << duration << ", each find/locate takes " << (duration / MAX_FINDS / keys.size() / 2) << "." << '\n';
				return false;
			});*/
			for (int i = 0; i < MAX_FINDS; i++) {
				for (auto const &key: keys) {
					auto ret = tt.locate(key.c_str());
					(void) ret;
					auto ret1 = tt.find(key.c_str());
					(void) ret1;
				}
			}
		}

		test5_bench_finds(tt, keys, MAX_FINDS);
	}
} // namespace trie::tests

int main() {
	using namespace trie::tests;
	test1();
	test2();
	test3();
	test5_bench_inserts_and_finds();
	return 0;
}
