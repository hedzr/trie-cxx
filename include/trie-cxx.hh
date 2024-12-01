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

#if defined(_MSC_VER) ||                                                \
        (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
         (__GNUC__ >= 4)) // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "trie-cxx/trie-all.hh"

#if (__cplusplus < 202002L) // c++20a, c++17 and lower
//c++20a,c++17, and lower...
#else // c++20 or higher
//c++20 and higher
#endif
#if __cplusplus > 201703L
// C++20a, and C++20 and higher
#endif
