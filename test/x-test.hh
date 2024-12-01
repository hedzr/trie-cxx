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
// Created by Hedzr Yeh on 2023/9/20.
//

#ifndef TRIE_CXX_X_TEST_HH
#define TRIE_CXX_X_TEST_HH

#if defined(TRIE_UNIT_TEST) && TRIE_UNIT_TEST == 1

#include <algorithm>
#include <ctime>
#include <functional>
#include <memory>

#include "trie-cxx/trie-chrono.hh"
#include "x-dbg.hh"


namespace trie::test {

	/**
     * @brief wrapper will wrap a test function to add pre-/post-process on it.
     * @tparam _Callable 
     * @tparam _Args 
     * @see TRIE_TEST_FOR
     * @note To use wrapper class, see also TRIE_TEST_FOR(func) macro.
     * In your test app, it'll be quoted as:
     * @code{c++}
     * void test_func_1(){ ... }
     * int main(){
     *   TRIE_TEST_FOR(test_func_1);
     * }
     * @endcode
     */
	template<typename _Callable, typename... _Args>
	class wrapper {
	public:
		explicit wrapper(const char *_fname_, _Callable &&f, _Args &&...args) {
			// auto filename = debug::type_name<decltype(f)>();
			auto bound = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
			chrono::high_res_duration hrd;
			before(_fname_);
			try {
				bound();
			} catch (...) {
				after(_fname_);
				throw;
			}
			after(_fname_);
		}
		~wrapper() = default;

	private:
		void before(const char *fname) {
			printf("\n--- BEGIN OF %-40s ----------------------\n", fname);
		}
		void after(const char *fname) {
			printf("--- END OF %-42s ----------------------\n\n", fname);
		}
	}; // class wrapper

	template<typename _Callable, typename... _Args>
	inline auto bind(const char *funcname, _Callable &&f, _Args &&...args) {
		wrapper w{funcname, f, args...};
		return w;
	}

	/**
     * @brief TRIE_TEST_FOR will wrap a test function to add pre-/post-process on it.
     * @details In your test app, it'll be quoted as:
     * @code{c++}
     * void test_func_1(){ ... }
     * int main(){
     *   TRIE_TEST_FOR(test_func_1);
     * }
     * @endcode
     */
#define TRIE_TEST_FOR(f) trie::test::bind(#f, f)

	namespace detail {
		inline void third_party(int n, std::function<void(int)> f) {
			f(n);
		}

		/**
     * @brief 
     * 
     * foo f;
     * f.invoke(1, 2);
     * 
     */
		struct foo {
			template<typename... Args>
			void invoke(int n, Args &&...args) {
				auto bound = std::bind(&foo::invoke_impl<Args &...>, this,
				                       std::placeholders::_1, std::forward<Args>(args)...);

				third_party(n, bound);
			}

			template<typename... Args>
			void invoke_impl(int, Args &&...) {
			}
		};
	} // namespace detail

} // namespace trie::test
#endif

#endif //TRIE_CXX_X_TEST_HH
