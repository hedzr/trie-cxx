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

#ifndef TRIE_CXX_TRIE_BASE_HH
#define TRIE_CXX_TRIE_BASE_HH

#include <algorithm>
#include <functional>
#include <memory>

#include <any>
#include <optional>
#include <tuple>
#include <valarray>
#include <variant>

#include <chrono>

#include <deque>
#include <iterator>
#include <list>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cerrno>
#include <cfloat>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "trie-chrono.hh"

namespace trie {
	using base_base_t = std::variant<
		int, unsigned int, float, double, bool, std::string,
		std::vector<int>, std::vector<unsigned int>, std::vector<float>,
		std::vector<double>, std::vector<bool>, std::vector<std::string>,
		std::valarray<int>, std::valarray<unsigned int>, std::vector<float>,
		std::valarray<double> >;


	using base_t = std::variant<
		std::monostate,
		bool, char, // unsigned char, wchar_t,
		int, unsigned int, int8_t, int16_t, // int64_t == long long, // int32_t,
		uint8_t, uint16_t, // uint32_t, // uint64_t = unsigned long long,
		long, long long, unsigned long, unsigned long long,
		float, double, long double,
		std::chrono::duration<long double, std::ratio<60> >,
		std::chrono::nanoseconds,
		std::chrono::microseconds,
		std::chrono::milliseconds,
		std::chrono::seconds,
		std::chrono::minutes,
		std::chrono::hours,
#if __cplusplus > 201703L
	        std::chrono::days,
	        std::chrono::weeks,
	        std::chrono::months,
	        std::chrono::years,

	        // std::chrono::time_zone,
	        std::chrono::system_clock::time_point,
#endif
		std::byte,

		std::vector<int>,
		std::vector<unsigned int>,
		std::vector<float>,
		std::vector<double>,
		std::vector<bool>,
		std::vector<std::string>,

		// std::tuple<T, ...>,

		char const *,
		// wchar_t const *,
		std::string>;


	template<class>
	[[maybe_unused]] inline constexpr bool always_false_v = false;

	template<typename>
	struct is_std_vector : std::false_type {
	};

	template<typename T, typename A>
	struct is_std_vector<std::vector<T, A> > : std::true_type {
	};

	template<typename>
	struct is_std_list : std::false_type {
	};

	template<typename T, typename A>
	struct is_std_list<std::list<T, A> > : std::true_type {
	};


	template<typename T, typename FIX = char,
		std::enable_if_t<is_std_vector<T>::value, bool>  = true>
	// requires(is_std_vector<T>)
	inline std::ostream &list_to_string(std::ostream &os, T const &v, FIX prefix, FIX suffix) {
		os << prefix;
		int ix{0};
		for (auto const &it: v) {
			if (ix++ > 0) os << ',';
			os << it;
		}
		os << suffix;
		return os;
	}


	[[maybe_unused]] inline auto variant_to_string(std::ostream &os, base_t const &v) -> std::ostream & {
		// std::stringstream os;

		std::visit([&os](auto &&arg) {
			           using T = std::decay_t<decltype(arg)>;
			           if constexpr (std::is_same_v<T, bool>)
				           os << std::boolalpha << arg;
			           else if constexpr (std::is_same_v<T, char>)
				           os << '\'' << arg << '\'';
				           // else if constexpr (std::is_same_v<T, unsigned char>)
				           // 	os << arg;
				           // else if constexpr (std::is_same_v<T, wchar_t>)
				           // 	os << arg;
			           else if constexpr (std::is_same_v<T, int>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, int8_t>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, int16_t>)
				           os << arg;
				           // else if constexpr (std::is_same_v<T, int32_t>)
				           // 	os << arg;
			           else if constexpr (std::is_same_v<T, int64_t>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, unsigned int>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, uint8_t>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, std::byte>)
				           os << (uint8_t) arg;
			           else if constexpr (std::is_same_v<T, uint16_t>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, uint32_t>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, uint64_t>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, long>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, long long>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, unsigned long>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, unsigned long long>)
				           os << arg;
			           else if constexpr (std::is_same_v<T, float>)
				           os << std::setprecision(std::numeric_limits<float>::digits10) << arg;
			           else if constexpr (std::is_same_v<T, double>)
				           // os << std::setprecision(35) << arg;
				           os << std::setprecision(std::numeric_limits<double>::digits10) << arg;
			           else if constexpr (std::is_same_v<T, long double>)
				           // os << std::setprecision(std::numeric_limits<long double>::max_digits10) << arg;
				           os << std::setprecision(LDBL_DIG * 2) << arg;
			           else if constexpr (std::is_same_v<T, std::string>)
				           os << std::quoted(arg);
			           else if constexpr (std::is_same_v<T, char *const>)
				           os << std::quoted(arg);
			           else if constexpr (std::is_same_v<T, char const *>)
				           os << std::quoted(arg);
				           // else if constexpr (std::is_same_v<T, wchar_t const *>)
				           // 	os << std::quoted(arg);
			           else if constexpr (std::is_same_v<T, std::chrono::duration<long double, std::ratio<60> > >)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::duration<long long, std::ratio<60> > >)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::nanoseconds>)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::microseconds>)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::milliseconds>)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::seconds>)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::minutes>)
				           chrono::format_duration(os, arg);
			           else if constexpr (std::is_same_v<T, std::chrono::hours>)
				           chrono::format_duration(os, arg);
#if __cplusplus > 201703L
			else if constexpr (std::is_same_v<T, std::chrono::days>)
				chrono::format_duration(os, arg);
			else if constexpr (std::is_same_v<T, std::chrono::weeks>)
				chrono::format_duration(os, arg);
			else if constexpr (std::is_same_v<T, std::chrono::months>)
				chrono::format_duration(os, arg);
			else if constexpr (std::is_same_v<T, std::chrono::years>)
				chrono::format_duration(os, arg);
			else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>)
				os << chrono::format_time_point(arg);
#endif
			           else if constexpr (std::is_same_v<T, std::vector<bool> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<int> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<unsigned int> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<int64_t> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<uint64_t> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<float> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<double> >)
				           list_to_string(os, arg, '[', ']');
			           else if constexpr (std::is_same_v<T, std::vector<std::string> >)
				           list_to_string(os, arg, '[', ']');

				           // std::vector<int>, std::vector<uint>, std::vector<float>,
				           //         std::vector<double>, std::vector<bool>, std::vector<std::string>,

			           else if constexpr (is_std_vector<T>::value) {
				           os << '[';
				           int ix{0};
				           for (auto const &it: arg) {
					           if (ix++ > 0) os << ',';
					           variant_to_string(os, it); // os << it;
				           }
				           os << ']';
			           } else if constexpr (is_std_list<T>::value) {
				           os << '[' << '/';
				           int ix{0};
				           for (auto const &it: arg) {
					           if (ix++ > 0) os << ',';
					           variant_to_string(os, it); // os << it;
				           }
				           os << '/' << ']';
			           } else if constexpr (std::is_same_v<T, std::monostate>)
				           os << "<null>";
			           else
				           static_assert(always_false_v<T>, "non-exhaustive visitor!");
		           },
		           v);
		return os;
	}
} // namespace trie


////////////////////////////////////////////////////////////////////////


// OS

#ifndef _OS_MACROS
#define _OS_MACROS

// https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor/46177613
// https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive/8249232
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
//#define OS_WINDOWS
#define OS_WIN
#ifdef _WIN64
// define something for Windows (64-bit only)
#define OS_WIN64
#else
// define something for Windows (32-bit only)
#define OS_WIN32
#endif

#elif __APPLE__
#define OS_MAC
#define OS_MACOS
#define OS_APPLE
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
// #error "Unknown Apple platform"
#define TARGET_UNKNOWN
#endif

#elif __ANDROID__
#define OS_ANDROID

#elif __linux__
// linux
#define OS_LINUX

#elif __FreeBSD__
#define OS_FREEBSD

#elif __unix__ // all unices not caught above
// Unix
#define OS_UNIX

#elif defined(_POSIX_VERSION)
// POSIX
#define OS_OTHERS_POSIX

#else
//#error "Unknown compiler"
#define OS_UNKNOWN
#endif

#endif //_OS_MACROS

#ifndef _OS_MACROS_MORE
#define _OS_MACROS_MORE

#ifdef OS_UNIX
#undef OS_UNIX
#define OS_UNIX 1
#else
#define OS_UNIX 0
#endif

#ifdef OS_FREEBSD
#undef OS_FREEBSD
#define OS_FREEBSD 1
#else
#define OS_FREEBSD 0
#endif

#ifdef OS_LINUX
#undef OS_LINUX
#define OS_LINUX 1
#else
#define OS_LINUX 0
#endif

#ifdef OS_APPLE
#undef OS_APPLE
#define OS_APPLE 1
#else
#define OS_APPLE 0
#endif

#ifdef OS_WIN
#undef OS_WIN
#define OS_WIN 1
#else
#define OS_WIN 0
#endif

#if defined(_POSIX_VERSION)
#define OS_POSIX 1
#else
#define OS_POSIX 0
#endif

// ARCH

#if defined(__arm__)
#define ARCH_ARM 1
#else
#define ARCH_ARM 0
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_X64 1
#else
#define ARCH_X64 0
#endif

#if defined(__aarch64__)
#define ARCH_AARCH64 1
#else
#define ARCH_AARCH64 0
#endif

#if defined(__powerpc64__)
#define ARCH_PPC64 1
#else
#define ARCH_PPC64 0
#endif

#endif // _OS_MACROS_MORE

////////////////////////////////////////////////////////////////////////


#include <stdlib.h>

#if OS_WIN
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#undef min
#undef max
#include <time.h>
namespace trie { namespace cross {
	inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
		UNUSED(__overwrite);
		std::ostringstream os;
		os << __name << '=' << __value;
		(void) _putenv(os.str().c_str());
	}

	inline time_t time(time_t *_t = nullptr) {
		return ::time(_t);
	}
	// BEWRAE: this is a thread-unsafe routine, it's just for the simple scene.
	inline struct tm *gmtime(time_t const *_t = nullptr) {
		static struct tm _tm{};
		if (!_t) {
			time_t vt = time();
			gmtime_s(&_tm, &vt);
		} else
			gmtime_s(&_tm, _t);
		return &_tm;
	}

	template<class T>
	inline T max(T a, T b) { return a < b ? b : a; }
	template<class T>
	inline T min(T a, T b) { return a < b ? a : b; }
}} // namespace trie::cross
#else
#include <algorithm>
#include <ctime>
#include <time.h>

namespace trie {
	namespace cross {
		inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
			::setenv(__name, __value, __overwrite);
		}

		inline time_t time(time_t *_t = nullptr) {
			return std::time(_t);
		}
		inline struct tm *gmtime(time_t const *_t = nullptr) {
			if (!_t) {
				time_t const vt = time();
				return std::gmtime(&vt);
			}
			return std::gmtime(_t);
		}

		template<class T>
		inline T max(T a, T b) { return std::max(a, b); }
		template<class T>
		inline T min(T a, T b) { return std::min(a, b); }
	}
} // namespace trie::cross
#endif

#endif // TRIE_CXX_TRIE_BASE_HH
