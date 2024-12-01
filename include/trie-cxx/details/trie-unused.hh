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
// Created by Hedzr Yeh on 2024/9/27.
//

#ifndef TRIE_DETAILS_UNUSED_HH
#define TRIE_DETAILS_UNUSED_HH

#ifndef _UNUSED_DEFINED
#define _UNUSED_DEFINED
#ifdef __clang__

//#ifndef UNUSED
//#define UNUSED(...) [__VA_ARGS__](){}
//#endif

/**
 * @brief UNUSED macro
 * @tparam Args
 * @param args
 * @code{c++}
 *   UNUSED(argc);
 *   UNUSED(argc, argv);
 *   // Cannot be used for variadic parameters:
 *   //   template&lt;class... Args> void unused_func(Args &&...args) { UNUSED(args); }
 *   // But you can expand the parameter pack like this:
 *   //   template&lt;typename... Args>
 *   //   inline void unused_func(Args &&...args) {
 *   //       UNUSED(sizeof...(args));
 *   //   }
 * @endcode
 */
template<typename... Args>
inline void UNUSED([[maybe_unused]] Args &&...args) {
	(void) (sizeof...(args));
}

#elif __GNUC__ || _MSC_VER

// c way unused
#ifndef UNUSED
#define UNUSED0()
#define UNUSED1(a)                                       (void) (a)
#define UNUSED2(a, b)                                    (void) (a), UNUSED1(b)
#define UNUSED3(a, b, c)                                 (void) (a), UNUSED2(b, c)
#define UNUSED4(a, b, c, d)                              (void) (a), UNUSED3(b, c, d)
#define UNUSED5(a, b, c, d, e)                           (void) (a), UNUSED4(b, c, d, e)

#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define VA_NUM_ARGS(...)                                 VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define ALL_UNUSED_IMPL_(nargs)                          UNUSED##nargs
#define ALL_UNUSED_IMPL(nargs)                           ALL_UNUSED_IMPL_(nargs)
#define UNUSED(...)                         \
	ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__)) \
	(__VA_ARGS__)
#endif

#endif
#endif //_UNUSED_DEFINED

#endif //TRIE_DETAILS_UNUSED_HH
