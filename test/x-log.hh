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

#ifndef TRIE_CXX_X_LOG_HH
#define TRIE_CXX_X_LOG_HH

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <tuple>

// #include "trie-cxx/trie-common.hh"
// #include "trie-cxx/trie-def.hh"
#include <trie-cxx/trie-base.hh>

#include "x-terminal.hh"
// #include "x-util.hh"

// ------------------- singleton
namespace trie::util {

	template<typename T>
	class singleton {
	public:
		static T &instance();

		singleton(const singleton &) = delete;
		singleton &operator=(const singleton) = delete;

	protected:
		struct token {};
		singleton() = default;
	};

	template<typename T>
	inline T &singleton<T>::instance() {
		static std::unique_ptr<T> instance{new T{token{}}};
		return *instance;
	}

	// template<typename T>
	// using hus = trie::util::singleton<T>;

	template<typename C, typename... Args>
	class singleton_with_optional_construction_args {
	private:
		singleton_with_optional_construction_args() = default;
		static C *_instance;

	public:
		~singleton_with_optional_construction_args() {
			delete _instance;
			_instance = nullptr;
		}
		static C &instance(Args... args) {
			if (_instance == nullptr)
				_instance = new C(args...);
			return *_instance;
		}
	};

	template<typename C, typename... Args>
	C *singleton_with_optional_construction_args<C, Args...>::_instance = nullptr;

#if defined(_DEBUG) && defined(NEVER_USED)
	inline void test_singleton_with_optional_construction_args() {
		int &i = singleton_with_optional_construction_args<int, int>::instance(1);
		UTEST_CHECK(i == 1);

		tester1 &t1 = singleton_with_optional_construction_args<tester1, int>::instance(1);
		UTEST_CHECK(t1.result() == 1);

		tester2 &t2 = singleton_with_optional_construction_args<tester2, int, int>::instance(1, 2);
		UTEST_CHECK(t2.result() == 3);
	}
#endif // defined(NEVER_USED)

} // namespace trie::util

#define TRIE_SINGLETON(t) trie::util::singleton<t>

namespace trie::log {

	namespace detail {
		class Log final : public util::singleton<Log> {
		public:
			explicit Log(typename util::singleton<Log>::token) {}
			~Log() = default;

			// [[maybe_unused]] trie::terminal::colors::colorize _c;

			template<class... Args>
			void log([[maybe_unused]] const char *fmt, [[maybe_unused]] Args const &...args) {
				// std::fprintf(std::stderr)
			}
			void vdebug(const char *file, int line, const char *func,
			            char const *fmt, va_list args) {
				// auto t = cross::time(nullptr);
				char time_buf[100];
#if _MSC_VER
				struct tm tm_{};
				auto _tm = &tm_;
				time_t vt = time(nullptr);
				gmtime_s(_tm, &vt);
#else
				auto *_tm = cross::gmtime(nullptr);
#endif
				std::strftime(time_buf, sizeof time_buf, "%D %T", _tm);

				va_list args2;
				va_copy(args2, args);
				std::vector<char> buf((std::size_t) std::vsnprintf(nullptr, 0, fmt, args) + 1);
				std::vsnprintf(buf.data(), buf.size(), fmt, args2);
				va_end(args2);

				const char *const fg_reset_all = "\033[0m";
				const char *const clr_magenta_bg_light = "\033[2;35m";
				const char *const clr_cyan_bg_light = "\033[2;36m";
				const char *const fg_light_gray = "\033[37m";
				std::printf("%s"
				            "%s %s:"
				            "%s"
				            " %s  %s%s:%d "
				            "%s"
				            "(%s)"
				            "%s"
				            "\n",
				            clr_magenta_bg_light,
				            time_buf,
				            // _c.dim().s("[debug]").as_string().c_str(),
				            "[debug]",
				            fg_reset_all,
				            buf.data(),
				            clr_cyan_bg_light,
				            file, line,
				            fg_light_gray, func, fg_reset_all);
			}
		};
	} // namespace detail

#if 0
    class log {
    public:
        template<class... Args>
        static void print(const char *fmt, Args const &...args) {
            xlog().template log(fmt, args...);
        }

        // static void vdebug(char const *fmt, va_list args) {
        //     xlog().vdebug(fmt, args);
        // }
        static void debug(char const *fmt, ...) {
            // auto t = cross::time(nullptr);
            char time_buf[100];
            // std::strftime(time_buf, sizeof time_buf, "%D %T", cross::gmtime());
#if _MSC_VER
            struct tm tm_ {};
            auto _tm = &tm_;
            time_t vt = time(nullptr);
            gmtime_s(_tm, &vt);
#else
            auto _tm = cross::gmtime();
#endif
            std::strftime(time_buf, sizeof time_buf, "%D %T", _tm);

            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);
            std::vector<char> buf((std::size_t) std::vsnprintf(nullptr, 0, fmt, args1) + 1);
            va_end(args1);
            std::vsnprintf(buf.data(), buf.size(), fmt, args2);
            va_end(args2);

            std::printf("%s [debug]: %s\n", time_buf, buf.data());
        }

    private:
        static detail::Log &xlog() { return detail::Log::instance(); }
    }; // class log
#endif

	class holder {
		const char *_file;
		int _line;
		const char *_func;

	public:
		holder(const char *file, int line, const char *func)
		    : _file(file)
		    , _line(line)
		    , _func(func) {}

		void operator()(char const *fmt, ...) {
			va_list va;
			va_start(va, fmt);
			xlog().vdebug(_file, _line, _func, fmt, va);
			va_end(va);
		}

	private:
		static detail::Log &xlog() { return detail::Log::instance(); }
	};
	// Logger log;
} // namespace trie::log

#if defined(_MSC_VER)
#define log_print(...) trie::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define log_print(...) trie::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif

#if defined(_DEBUG)
#if defined(_MSC_VER)
#define log_debug(...) trie::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define log_debug(...) trie::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#else
#if defined(__GNUG__) || defined(_MSC_VER)
#define log_debug(...) \
	(void) 0
#else
#define log_debug(...)                                                                      \
	_Pragma("GCC diagnostic push")                                                            \
	        _Pragma("GCC diagnostic ignored \"-Wunused-value\"") do { (void) (__VA_ARGS__); } \
	while (0)                                                                                 \
	_Pragma("GCC diagnostic pop")
#endif
#endif

#if defined(TRIE_ENABLE_VERBOSE_LOG)
// inline void debug(char const *fmt, ...) {
//     va_list va;
//     va_start(va, fmt);
//     trie::log::log::vdebug(fmt, va);
//     va_end(va);
// }
#if defined(_MSC_VER)
#define log_verbose_debug(...) trie::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define log_verbose_debug(...) trie::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#else
// #define log_verbose_debug(...)
//     _Pragma("GCC diagnostic push")
//             _Pragma("GCC diagnostic ignored \"-Wunused-value\"") do { (void) (__VA_ARGS__); }
//     while (0)
//     _Pragma("GCC diagnostic pop")

//#define log_verbose_debug(...) (void)(__VA_ARGS__)

template<typename... Args>
void log_verbose_debug([[maybe_unused]] Args &&...args) { (void) (sizeof...(args)); }
#endif
#define log_trace log_verbose_debug

#endif //TRIE_CXX_X_LOG_HH
