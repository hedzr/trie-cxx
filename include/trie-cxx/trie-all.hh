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

#ifndef TRIE_CXX_TRIE_HH
#define TRIE_CXX_TRIE_HH

#if defined(_MSC_VER) ||                                                \
        (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
         (__GNUC__ >= 4)) // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

/*
 * @file trie-all.hh
 * @brief trie-tree, radix-tree, and store
 */

#include "trie-base.hh"
#include "trie-chrono.hh"
#include "trie-core.hh"

#endif // TRIE_CXX_TRIE_HH
