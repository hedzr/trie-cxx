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

#ifndef TRIE_CXX_X_CLASS_HH
#define TRIE_CXX_X_CLASS_HH

#pragma once

#include <stdlib.h>
#include <string>

namespace trie::debug {

	class X {
		std::string _str;

		void _ct(const char *leading) {
			printf("  - %s: X[ptr=%p].str: %p, '%s'\n", leading, (void *) this, (void *) _str.c_str(), _str.c_str());
		}

	public:
		X() {
			_ct("ctor()");
		}
		~X() {
			_ct("dtor");
		}
		X(std::string &&s)
		    : _str(std::move(s)) {
			_ct("ctor(s)");
		}
		X(std::string const &s)
		    : _str(s) {
			_ct("ctor(s(const&))");
		}
		X &operator=(std::string &&s) {
			_str = std::move(s);
			_ct("operator=(&&s)");
			return (*this);
		}
		X &operator=(std::string const &s) {
			_str = s;
			_ct("operator=(const&s)");
			return (*this);
		}

		const char *c_str() const { return _str.c_str(); }
		operator const char *() const { return _str.c_str(); }
	};

} // namespace trie::debug

#endif //TRIE_CXX_X_CLASS_HH
