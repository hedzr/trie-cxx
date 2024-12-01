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

#ifndef TRIE_CXX_TRIE_CORE_HH
#define TRIE_CXX_TRIE_CORE_HH

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
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


#include "trie-base.hh"
#include "trie-chrono.hh"
#include "trie-node.hh"

// node
namespace trie {

	template<typename ValueT,
	         char delimiter = '.',
	         typename DescT = extensions::void_desc,       // use description_holder if u'd like
	         typename CommentT = extensions::void_comment, // use comment_holder if u'd like
	         typename TagT = extensions::void_tag,         // use tag_holder if u'd like
	         typename ExtPkgT = extensions::detail::ext_package<DescT, CommentT, TagT>>
	class node final
	    : public std::enable_shared_from_this<node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>> {
	public:
		node() = default;
		~node() = default;

		enum NodeType {
			NODE_NONE,
			NODE_LEAF,
			NODE_BRANCH,
		};

		using ext_pkg_t = ExtPkgT;

		using node_t = node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>;
		using value_t = ValueT;
		using desc_t = typename DescT::desc_t;
		using comment_t = typename CommentT::comment_t;
		using tag_t = typename TagT::tag_t;
		using node_type = NodeType;
		using node_ptr = std::shared_ptr<node_t>;
		using const_node_ptr = std::shared_ptr<node_t const>;
		using weak_node_ptr = std::weak_ptr<node_t>;
		using const_weak_node_ptr = std::weak_ptr<node_t const>;
		using children_t = std::vector<node_ptr>;
		// using return_t = std::tuple<value_t, errno_t, bool>;
		// using const_return_t = std::tuple<value_t const, errno_t, bool>;
		// using find_return_t = std::tuple<std::size_t, weak_node_ptr, errno_t, bool>;             // partial_matched_size, node*, matched
		// using const_find_return_t = std::tuple<std::size_t, const_weak_node_ptr, errno_t, bool>; // partial_matched_size, node*, matched
		using locate_return_t = std::tuple<std::size_t, std::vector<weak_node_ptr> *, weak_node_ptr, errno_t, bool>;
		using const_locate_return_t = std::tuple<std::size_t, std::vector<const_weak_node_ptr> *, const_weak_node_ptr,
		                                         errno_t, bool>;

		friend node_ptr;

		struct return_s {
			bool ok{};
			errno_t en{};
			value_t old{};
		};

		struct const_find_return_s {
			std::size_t partial_matched_size{};
			const_weak_node_ptr ptr{};
			errno_t en{};
			bool matched{};
			const_find_return_s() = default;
			const_find_return_s(std::size_t pms, weak_node_ptr &ptr_, errno_t en_, bool m)
			    : partial_matched_size(pms)
			    , ptr(ptr_)
			    , en(en_)
			    , matched(m) {
			}
			virtual ~const_find_return_s() {
			}
		};

		struct find_return_s {
			std::size_t partial_matched_size{};
			weak_node_ptr ptr{};
			errno_t en{};
			bool matched{};
			find_return_s() = default;
			find_return_s(std::size_t pms, weak_node_ptr &ptr_, errno_t en_, bool m)
			    : partial_matched_size(pms)
			    , ptr(ptr_)
			    , en(en_)
			    , matched(m) {
			}
			find_return_s(find_return_s &&o) noexcept
			    : partial_matched_size(o.partial_matched_size)
			    , ptr(o.ptr)
			    , en(o.en)
			    , matched(o.matched) {
			}
			virtual ~find_return_s() {
			}
			auto to_const() -> const_find_return_s {
				const_find_return_s ret{partial_matched_size, ptr, en, matched};
				return ret;
			}
		};

		struct const_locate_return_s final : public const_find_return_s {
			std::vector<const_weak_node_ptr> *parents{};
			const_locate_return_s() = default;
			const_locate_return_s(std::size_t pms, weak_node_ptr &ptr_, errno_t en_, bool m,
			                      std::vector<const_weak_node_ptr> *pv)
			    : find_return_s(pms, ptr_, en_, m)
			    , parents(pv) {
			}
			~const_locate_return_s() override {
				if (parents) delete parents;
			}
		};

		struct locate_return_s final : public find_return_s {
			std::vector<weak_node_ptr> *parents{};
			locate_return_s() = default;
			locate_return_s(std::size_t pms, weak_node_ptr &ptr_, errno_t en_, bool m,
			                std::vector<weak_node_ptr> *pv)
			    : find_return_s(pms, ptr_, en_, m)
			    , parents(pv) {
			}
			locate_return_s(locate_return_s &&o) noexcept
			    : find_return_s(std::move(o)) {
				parents = o.parents;
				o.parents = nullptr;
			}
			~locate_return_s() override {
				if (parents) delete parents;
			}
			explicit operator const_find_return_s() { return to_const_obj(); }
			auto to_const_obj() -> const_locate_return_s {
				const_locate_return_s ret{};
				ret.partial_matched_size = this->partial_matched_size;
				ret.ptr = this->ptr;
				ret.en = this->en;
				ret.matched = this->matched;
				if (this->parents) {
					std::vector<const_weak_node_ptr> *p = new std::vector<const_weak_node_ptr>();
					p->reserve(this->parents->size());
					for (auto &el : *parents) {
						p->push_back(el);
					}
				}
				return ret;
			}
		};

		// for errno_t, see:
		//    https://en.cppreference.com/w/cpp/error/errno
		//    https://en.cppreference.com/w/cpp/error/errno_macros
		//    https://learn.microsoft.com/en-us/cpp/c-runtime-library/errno-constants?view=msvc-170
		//    https://stackoverflow.com/questions/7901117/how-do-i-use-errno-in-c

		node(node &&o) noexcept
		    : _type(o._type)
		    , _path(std::move(o._path))
		    , _fragment(std::move(o._fragment))
		    , _fragment_length(o._fragment_length)
		    , _value(std::move(o._value))
		    , _children(std::move(o._children)) {
		}
		explicit node(const node_type type, std::string const &full, std::string const &frag, value_t &&val)
		    : _type(type)
		    , _path(full)
		    , _fragment(frag)
		    , _fragment_length(frag.length())
		    , _value(std::move(val)) {
		}
		template<typename... Args>
		explicit node(node_type type, std::string const &full, std::string const &frag, Args &&...args)
		    : _type(type)
		    , _path(full)
		    , _fragment(frag)
		    , _fragment_length(frag.length())
		    , _value(std::forward<Args>(args)...) {
		}

	public:
		std::string &path() { return _path; } // full path from root to this node
		std::string const &path() const { return _path; }
		void path(std::string const &s) { _path = s; }
		// std::string &fragment() { return _fragment; } // path fragment in this node
		std::string const &fragment() const { return _fragment; }
		void fragment(std::string const &s) {
			_fragment = s;
			_fragment_length = s.length();
		}
		std::size_t fragment_length() const { return _fragment_length; }
		value_t &value() { return _value; } // the payload of a leaf node; empty if branch mode
		value_t const &value() const { return _value; }
		void value(value_t &&val) { std::swap(_value, val); }
		node_type type() const { return _type; } // node type: branch, leaf or none.
		void type(node_type t) { _type = t; }
		desc_t const &desc() const { return _pkg.desc(); } // leaf node's description
		node_t &desc(desc_t const &s) {
			_pkg.desc(s);
			return (*this);
		}
		comment_t const &comment() const { return _pkg.comment(); } // leaf node's comment
		node_t &comment(comment_t const &s) {
			_pkg.comment(s);
			return (*this);
		}
		tag_t const &tag() const { return _pkg.tag(); } // leaf node's tagged value
		node_t &tag(tag_t const &s) {
			_pkg.tag(s);
			return (*this);
		}

		// pure trie-tree interfaces

	public:
		// auto insert(std::string const &path, value_t const &value) -> return_s; // insert or update
		auto insert(std::string const &path, value_t &&value) -> return_s;
		// auto insert(char const *path, ValueT const &value) -> return_s; // insert or update
		auto insert(char const *path, value_t &&value) -> return_s;
		auto insert(char const *path, char const *value) -> return_s;
		template<typename... Args, std::enable_if_t<std::is_constructible_v<value_t, Args...>, bool> = true>
		auto insert(char const *path, Args &&...args) -> return_s {
			return_s ret{};
			if (!path) return ret;

			find_return_s fr{};
			if (auto path_len = std::strlen(path); fast_find_internal(fr, path, path_len)) {
				// matched
				if (fr.partial_matched_size == 0) {
					// matched a node completely, replace it with new value
					if (node_ptr sp = fr.ptr.lock()) {
						sp->_value.swap(ret.old);
						sp->_value.template emplace<Args...>(args...);
						ret.ok = true;
					}
					return ret; // return {{}, false};
				}

				// split the node:
				//
				// `herz -> hers` to:
				//   `her->s`
				//      `->z`
				if (node_ptr sp = fr.ptr.lock()) {
					auto const frag = sp->fragment();
					auto const minus = sp->fragment_length() - fr.partial_matched_size;
					if (minus == 0) {
						// insert as child simply
						std::string new_path{path};
						auto const title = new_path.substr(sp->path().length());
						node_ptr new_leaf = std::make_shared<node_t>(
						        NODE_LEAF, new_path,
						        title, args...);
						sp->add(new_leaf);
						ret.ok = true;
						return ret;
					}

					auto const full_path = sp->path();
					ret.old = sp->set_value(std::move(ret.old));
					sp->path(full_path.substr(0, full_path.length() - minus));
					sp->fragment(frag.substr(0, fr.partial_matched_size));
					sp->type(NODE_BRANCH);

					node_ptr child = std::make_shared<node_t>(
					        NODE_LEAF, full_path.substr(full_path.length() - minus - 1),
					        frag.substr(fr.partial_matched_size), std::move(ret.old));
					sp->add(child);

					std::string new_path{path};
					auto const title = new_path.substr(new_path.length() - minus - 1);
					node_ptr new_leaf = std::make_shared<node_t>(
					        NODE_LEAF, new_path,
					        title, args...);
					sp->add(new_leaf);

					ret.ok = true;
					return ret;
				}
			} else {
				if (fr.partial_matched_size > 0) {
					if (node_ptr sp = fr.ptr.lock()) {
						auto const frag = sp->fragment();
						auto const minus = sp->fragment_length() - fr.partial_matched_size;

						if (minus == 0) {
							// add child directly
							auto rest_title = path + sp->path().length();
							node_ptr new_leaf = std::make_shared<node_t>(
							        NODE_LEAF, path, rest_title, args...);
							sp->add(new_leaf);
							ret.ok = true;
							return ret;
						}

						// split the node pointed by sp
						auto const dad_title = frag.substr(0, fr.partial_matched_size);
						auto const full_path = sp->path();
						ret.old = sp->set_value(std::move(ret.old));
						sp->path(full_path.substr(0, full_path.length() - minus));
						sp->fragment(dad_title);
						sp->type(NODE_BRANCH);

						auto const child_title = frag.substr(fr.partial_matched_size);
						node_ptr child = std::make_shared<node_t>(
						        NODE_LEAF, full_path,
						        child_title, std::move(ret.old));
						sp->add(child);

						// auto rest = path + partial_matched_size;
						// ret = sp->insert(rest, std::move(value));

						auto const rest_title = path + sp->path().length();
						node_ptr new_leaf = std::make_shared<node_t>(
						        NODE_LEAF, path, rest_title, args...);
						sp->add(new_leaf);

						ret.ok = true;
					}
					return ret;
				}

				// insert full
				auto sp = this->shared_from_this();
				ret.old = sp->set_value(std::move(ret.old));
				std::string new_path{path};
				node_ptr new_leaf = std::make_shared<node_t>(NODE_LEAF, new_path, new_path, args...);
				sp->add(new_leaf);
				sp->type(NODE_BRANCH);
				ret.ok = true;
			}
			return ret;
		}

		auto remove(std::string const &path, bool include_children = true) -> return_s; // remove if exists
		auto remove(char const *path, bool include_children = true) -> return_s;

		auto find(std::string const &path) const -> const_find_return_s; // find a path
		auto find(char const *path) const -> const_find_return_s;
		auto locate(char const *path) const -> const_locate_return_s;
		auto locate(char const *path) -> locate_return_s;

		auto fast_find(char const *path) -> find_return_s;
		auto fast_find(char const *path) const -> const_find_return_s;

	private:
		auto locate_internal(char const *path, weak_node_ptr parent) -> locate_return_s;
		auto find_internal(char const *path) -> find_return_s;
		auto fast_find_internal(find_return_s &ctx, char const *path, std::size_t path_len) -> bool;

	public:
		auto children_count() const -> std::size_t { return _children.size(); }

		// auto root() const -> weak_node_ptr;

		auto dump(std::ostream &os, int indent_level = 0) const -> std::ostream &;

		auto to_string() const -> std::string;

		static int dump_left_width() { return _dump_left_width; }
		static void dump_left_width(int w) { _dump_left_width = w; }

		// store interfaces

	public:
		/**
		 * @brief set a store key with value.
		 * @details  A store,
		 * Store, which has path separated by delimiter
		 * like dot char ('.'), is an advance trie-tree for
		 * providing in-memory key-value paire with hierarchical
		 * data (tree data). A yaml config file is store's
		 * prefer persistent form.
		 * @param path
		 * @param value
		 * @return
		 */
		auto set(char const *path, value_t &&value) -> return_s;
		auto get(char const *path) const -> value_t const &;
		auto get(char const *path, value_t const &default_val) const -> value_t const &;
		auto get_node_with_info(char const *path) const -> find_return_s;
		auto has(char const *path, bool partial_match = false) const -> bool;

		// auto append(char const *path, value_t &&value) -> return_s;                  // concat value to an exists node or merge array values in it.
		// auto update(char const *path, value_t &&value) -> return_s;                  // update only
		// auto move(std::string const &path, std::string const &new_path) -> return_s; // move an exists node to new position
		// auto move(char const *path, char const *new_path) -> return_s;

		/**
		 * @brief store api: search a key path and return its information
		 * @details The difference of search() and find() is, search()
		 * try matching key path separated by delimiter, but find() treats
		 * delimiter as a normal char in a path.
		 * So, find("app.logging") will return matched == false and
		 * find("app.logging.") will get matched == true. But on the
		 * contrary, both search("app.logging") and search(app.logging.")
		 * will get matched == true.
		 *
		 * @param path a dotted key path like "app.logging"
		 * @return locate_return_s: [partial_matched_size, parents, weak_node_ptr, errno, matched]
		 */
		auto search(char const *path) -> locate_return_s;

		using walk_cb = std::function<void(node_type type, const_node_ptr,
		                                   int index, int level)>;
		auto walk(walk_cb cb) const -> void;

	protected:
		auto set_value(value_t &&val) -> value_t;
		auto add(node_ptr child) -> void;
		auto del(node_ptr child) -> void;
		auto dump_r(std::ostream &os, std::stringstream &ss, int level) const -> std::ostream &;
		auto walk_internal(walk_cb cb, int index, int level) const -> void;
		auto removed_fully(return_s &ret,
		                   char const *path,
		                   bool include_children,
		                   weak_node_ptr weak_nd_ptr,
		                   std::vector<weak_node_ptr> *parents,
		                   errno_t en) -> void;

	private:
		node_type _type{NODE_NONE}; // node type
		std::string _path{};        // full path to this node
		std::string _fragment{};    // path fragment
		std::size_t _fragment_length{0};
		value_t _value{};       // the payload
		children_t _children{}; // children nodes
		ext_pkg_t _pkg{};

		static int _dump_left_width;
	};

	// class node<...>
} // namespace trie

// trie_t
namespace trie {
	/**
	 * @brief A Trie-tree, a Radix-Trie tree.
	 * @details trie_t implements a compact trie tree, ie, a radix-trie tree. In
	 * each node, the key path fragment can be more than one character.
	 * trie_t supports our Store (in-memory key-value pairs store) directly.
	 * A Store is a tree with hierarchical data, which can be
	 * represented as a yaml-like data structure.
	 * A sample trie_t can be dumped to console:
	 * <root>
	 *   app.                           -> [B]
	 *     d                            -> [B]
	 *       ebug                       -> [L] (app.debug) 1
	 *       ump                        -> [L] (app.dump) 3
	 *     verbose                      -> [L] (app.verbose) 1
	 *     logging.                     -> [B]
	 *       file                       -> [L] (app.logging.file) "~/.trie.log"
	 *       rotate                     -> [L] (app.logging.rotate) 6
	 *       words                      -> [L] (app.logging.words) [a,1,false]
	 *     server.s                     -> [B]
	 *       tart                       -> [L] (app.server.start) 5
	 *       ites                       -> [L] (app.server.sites) 1
	 *
	 * @tparam ValueT
	 * @tparam delimiter
	 * @tparam DescT
	 * @tparam CommentT
	 * @tparam TagT
	 * @tparam ExtPkgT
	 */
	template<typename ValueT,
	         char delimiter = '.',
	         typename DescT = extensions::void_desc,       // use description_holder if u'd like
	         typename CommentT = extensions::void_comment, // use comment_holder if u'd like
	         typename TagT = extensions::void_tag,         // use tag_holder if u'd like
	         typename ExtPkgT = extensions::detail::ext_package<DescT, CommentT, TagT>>
	class trie_t {
	public:
		trie_t();
		~trie_t() = default;

		using node_t = node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>;
		using value_t = typename node_t::value_t;
		using desc_t = typename node_t::desc_t;
		using comment_t = typename node_t::comment_t;
		using tag_t = typename node_t::tag_t;
		using walk_cb = typename node_t::walk_cb;
		using node_type = typename node_t::node_type;
		using node_ptr = typename node_t::node_ptr;
		using const_node_ptr = typename node_t::const_node_ptr;
		using weak_node_ptr = typename node_t::weak_node_ptr;
		// using return_t = typename node_t::return_t;
		// using const_return_t = typename node_t::const_return_t;
		// using find_return_t = typename node_t::find_return_t;
		// using const_find_return_t = typename node_t::const_find_return_t;
		using locate_return_t = typename node_t::locate_return_t;
		using const_locate_return_t = typename node_t::const_locate_return_t;

		using return_s = typename node_t::return_s;
		using find_return_s = typename node_t::find_return_s;
		using const_find_return_s = typename node_t::const_find_return_s;
		using locate_return_s = typename node_t::locate_return_s;
		// using find_return_s = typename node_t::find_return_s;

		// auto insert(std::string const &path, value_t const &value) -> return_s; // insert or update
		auto insert(std::string const &path, value_t &&value) -> return_s;
		// auto insert(char const *path, value_t const &value) -> return_s; // insert or update
		// auto insert(char const *path, value_t &&value) -> return_s;
		template<typename... Args, std::enable_if_t<std::is_constructible_v<value_t, Args...>, bool> = true>
		auto insert(char const *path, Args &&...args) -> return_s {
			return _root->insert(path, std::forward<Args>(args)...);
		}
		auto insert(char const *path, char const *value) -> return_s { return _root->insert(path, value); }
		auto remove(std::string const &path, bool include_children = true) -> return_s; // remove if exists
		auto remove(char const *path, bool include_children = true) -> return_s;

		auto find(std::string const &path) const -> const_find_return_s; // find a path
		auto find(char const *path) const -> const_find_return_s;
		auto locate(char const *path) -> locate_return_s;

		auto fast_find(char const *path) const -> const_find_return_s { return _root->fast_find(path).to_const(); }
		auto fast_find(char const *path) -> find_return_s { return _root->fast_find(path); }

		// store apis

		auto append(char const *path, value_t &&value) -> return_s;
		// concat value to an exists node or merge array values in it.
		auto update(char const *path, value_t &&value) -> return_s; // update only

		// auto move(std::string const &path, std::string const &new_path) -> return_s; // move an exists node to new position
		auto move(char const *path, char const *new_path) -> return_s;

	public:
		/**
		 * @brief store api: has() returns whether the given key path exists or not.
		 * @details In a Store, a key path is a dotted string like "app.logging.file".
		 * @param path a key path is a dotted string like "app.logging.file".
		 * @return exists (true) or not (false)
		 */
		auto has(char const *path, bool partial_match = false) const -> bool;

		/**
		 * @brief store api: get the value of a key path
		 * @tparam T
		 * @tparam Types
		 * @param path a dotted key path separated by delimiter
		 * @return the concrete value
		 * @code
		 * auto const& val = tt.get<std::string>("app.logging.file");
		 * REQUIRE(val == std::string("~/.trie.log"));
		 * @endcode
		 */
		template<class T, class... Types>
		auto get(char const *path) const -> T const & {
			auto &var = const_cast<node_t *>(_root.get())->get(path);
			return std::get<T, Types...>(var);
		}
		template<class T, class... Types>
		auto get(char const *path, value_t const &default_val) const -> T const & {
			auto &var = const_cast<node_t *>(_root.get())->get(path, default_val);
			return std::get<T, Types...>(var);
		}

		/**
		 * @brief store api: get node ptr from a given key path.
		 * @details if cannot match, the return ptr will point to an empty node.
		 * @param path a dotted key path separated by delimiter
		 * @return weak_node_ptr, if matched, it points the searched node, or points an empty node
		 * @code
		 * trie_t::weak_node_ptr wp = tt.get("app.logging.file");
		 * if (auto sp = wp.lock()) {
		 *   if (sp) { // if matched ok, sp is a valid node
		 *     REQUIRE(std::get<std::string>(sp->value()) == std::string("~/.trie.log"));
		 *   }
		 * }
		 * @endcode
		 */
		auto get(char const *path) -> weak_node_ptr {
			if (auto ret = _root->search(path); ret.matched)
				return ret.ptr;
			return _empty;
		}

		/**
		 * @brief store api: set a store key with value.
		 * @details A store,
		 * Store, which has path separated by delimiter
		 * like dot char ('.'), is an advance trie-tree for
		 * providing in-memory key-value paire with hierarchical
		 * data (tree data). A yaml config file is store's
		 * prefer persistent form.
		 * @param path a dotted key path separated by delimiter
		 * @param value
		 * @return return_t: [old_val_if_has, errno_ignored, set_or_update_ok]
		 */
		auto set(char const *path, value_t &&value) -> return_s { return _root->set(path, std::move(value)); }

		/**
		 * @brief store api: search a key path and return its information
		 * @details The difference of search() and find() is, search()
		 * try matching key path separated by delimiter, but find() treats
		 * delimiter as a normal char in a path.
		 * So, find("app.logging") will return matched == false and
		 * find("app.logging.") will get matched == true. But on the
		 * contrary, both search("app.logging") and search(app.logging.")
		 * will get matched == true.
		 *
		 * @param path a dotted key path like "app.logging"
		 * @return locate_return_s: [partial_matched_size, parents, weak_node_ptr, errno, matched]
		 */
		auto search(char const *path) -> locate_return_s { return _root->search(path); }

		/**
		 * @brief store api: walk all keys in the store.
		 * @param cb
		 */
		auto walk(walk_cb cb) const -> void { _root->walk(cb); }

		auto dump(std::ostream &os) const -> std::ostream &;

	public:
		node_ptr root(node_ptr new_root) {
			node_ptr old;
			_root.swap(old);
			_root.swap(new_root);
			return old;
		}
		node_ptr &root() { return _root; }
		node_ptr const &root() const { return _root; }

		/**
		 * @brief calculating the count of leaves keys recursively.
		 * @return the total amount
		 */
		auto size() const -> std::size_t;

	private:
		auto ensure_root() -> node_ptr &;

	private:
		node_ptr _root{};
		node_ptr _empty{};
	}; // class trie_t<...>
} // namespace trie


// node<ValueT, TagT, char delimiter>
namespace trie {
	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	int node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::_dump_left_width{32};


	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        set(char const *path, value_t &&value) -> return_s {
		return insert(path, std::move(value));
	}

	/**
	 * @brief get_node_with_info find a full node like
	 * 'app.debug' or a partial node (branch or leaf)
	 * like 'app.logging.'.
	 * @details get_node_with_info checks tail delimiter
	 * (typically '.'), if so, it treats the case as fully
	 * matched, rather than partial matched (returned by
	 * find(path)). This behavior is to help inspecting
	 * store path (a dotted path) separated by delimiter
	 * char.
	 * A store, Store, which has path separated by delimiter
	 * like dot char ('.'), is an advance trie-tree for
	 * providing in-memory key-value paire with hierarchical
	 * data (tree data). A yaml config file is store's
	 * prefer persistent form.
	 * @tparam ValueT
	 * @tparam TagT
	 * @tparam delimiter
	 * @param path a key path
	 * @return a tuple with [pms, node_ptr, errno, matched].
	 */
	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        get_node_with_info(char const *path) const -> find_return_s {
		auto ret = fast_find(path);
		if (ret.matched) return ret;
		if (ret.partial_matched_size > 0) {
			if (auto sp = ret.ptr.lock()) {
				auto const &frag = sp->fragment();
				auto const size = sp->fragment_length();
				if (size == ret.partial_matched_size + 1 && frag[size - 1] == delimiter) {
					// for a node like "app.logging."
					ret.matched = true;
					return ret;
				}
			}
		}

		// not matched
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        get(char const *path) const -> value_t const & {
		if (auto ret = fast_find(path); ret.matched) {
			if (auto sp = ret.ptr.lock()) {
				return sp->value();
			}
		} else if (ret.partial_matched_size > 0) {
			if (auto sp = ret.ptr.lock()) {
				auto const &frag = sp->fragment();
				auto const size = sp->fragment_length();
				if (size == ret.partial_matched_size + 1 && frag[size - 1] == delimiter)
					// for a node like "app.logging."
					return sp->value();
			}
		}

		// assume this node is a branch, so its value shall be an empty value.
		return this->_value;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        get(char const *path, value_t const &default_val) const -> ValueT const & {
		if (auto ret = fast_find(path); ret.matched) {
			if (auto sp = ret.ptr.lock()) {
				return sp->value();
			}
		} else if (ret.partial_matched_size > 0) {
			if (auto sp = ret.ptr.lock()) {
				auto const &frag = sp->fragment();
				auto const size = sp->fragment_length();
				if (size == ret.partial_matched_size + 1 && frag[size - 1] == delimiter)
					// for a node like "app.logging."
					return sp->value();
			}
		}

		// assume this node is a branch, so its value shall be an empty value.
		return default_val;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        has(char const *path, bool partial_match) const -> bool {
		auto ret = fast_find(path);
		if (ret.matched) return true;
		if (ret.partial_matched_size > 0) {
			if (auto sp = ret.ptr.lock()) {
				auto const &frag = sp->fragment();
				auto const size = sp->fragment_length();
				if (size == ret.partial_matched_size + 1 && frag[size - 1] == delimiter)
					return true;
				return partial_match;
			}
		}
		return false;
	}

	/**
	 * @brief store api: walk all keys in the store.
	 * @tparam ValueT
	 * @tparam delimiter
	 * @tparam DescT
	 * @tparam CommentT
	 * @tparam TagT
	 * @tparam ExtPkgT
	 * @param cb
	 * @return
	 */
	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        walk(walk_cb cb) const -> void {
		this->walk_internal(cb, 0, 0);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        walk_internal(walk_cb cb, int index, int level) const -> void {
		if (_type != NODE_NONE) {
			auto ptr = this->shared_from_this();
			cb(_type, ptr, index, level);
		}

		auto idx{0};
		for (auto const &ch : _children) {
			ch->walk_internal(cb, idx++, level + 1);
		}
	}
} // namespace trie

// insert, remove, find, locate, dump, to_string, root
namespace trie {
	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        insert(std::string const &path, value_t &&value) -> return_s {
		return insert(path.c_str(), std::move(value));
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        insert(char const *path, char const *value) -> return_s {
		value_t v{value};
		return insert(path, std::move(v));
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        insert(char const *path, value_t &&value) -> return_s {
		return_s ret{};
		if (!path) return ret;

		find_return_s fr{};
		if (auto path_len = std::strlen(path); fast_find_internal(fr, path, path_len)) {
			// matched
			if (fr.partial_matched_size == 0) {
				// matched a node completely, replace it with new value
				if (node_ptr sp = fr.ptr.lock()) {
					ret.old = sp->set_value(std::move(value));
					ret.ok = true;
				}
				return ret; // return {{}, false};
			}

			// split the node:
			//
			// `herz -> hers` to:
			//   `her->s`
			//      `->z`
			if (node_ptr sp = fr.ptr.lock()) {
				auto const frag = sp->fragment();
				auto const minus = sp->fragment_length() - fr.partial_matched_size;
				if (minus == 0) {
					// insert as child simply
					std::string new_path{path};
					std::string title = new_path.substr(sp->path().length());
					node_ptr new_leaf = std::make_shared<node_t>(
					        NODE_LEAF, new_path,
					        title, std::move(value));
					sp->add(new_leaf);
					ret.ok = true;
					return ret;
				}

				auto full_path = sp->path();
				ret.old = sp->set_value(std::move(ret.old));
				sp->path(full_path.substr(0, full_path.length() - minus));
				sp->fragment(frag.substr(0, fr.partial_matched_size));
				sp->type(NODE_BRANCH);

				node_ptr child = std::make_shared<node_t>(
				        NODE_LEAF, full_path.substr(full_path.length() - minus - 1),
				        frag.substr(fr.partial_matched_size), std::move(ret.old));
				sp->add(child);

				std::string new_path{path};
				std::string title = new_path.substr(new_path.length() - minus - 1);
				node_ptr new_leaf = std::make_shared<node_t>(
				        NODE_LEAF, new_path,
				        title, std::move(value));
				sp->add(new_leaf);

				ret.ok = true;
				return ret;
			}
		} else {
			if (fr.partial_matched_size > 0) {
				if (node_ptr sp = fr.ptr.lock()) {
					auto const frag = sp->fragment();
					auto const minus = sp->fragment_length() - fr.partial_matched_size;

					if (minus == 0) {
						// add child directly
						auto rest_title = path + sp->path().length();
						node_ptr new_leaf = std::make_shared<node_t>(
						        NODE_LEAF, path, rest_title, std::move(value));
						sp->add(new_leaf);
						ret.ok = true;
						return ret;
					}

					// split the node pointed by sp
					auto dad_title = frag.substr(0, fr.partial_matched_size);
					auto full_path = sp->path();
					ret.old = sp->set_value(std::move(ret.old));
					sp->path(full_path.substr(0, full_path.length() - minus));
					sp->fragment(dad_title);
					sp->type(NODE_BRANCH);

					auto child_title = frag.substr(fr.partial_matched_size);
					node_ptr child = std::make_shared<node_t>(
					        NODE_LEAF, full_path,
					        child_title, std::move(ret.old));
					sp->add(child);

					// auto rest = path + partial_matched_size;
					// ret = sp->insert(rest, std::move(value));

					auto rest_title = path + sp->path().length();
					node_ptr new_leaf = std::make_shared<node_t>(
					        NODE_LEAF, path, rest_title, std::move(value));
					sp->add(new_leaf);

					ret.ok = true;
				}
				return ret;
			}

			// insert full
			auto sp = this->shared_from_this();
			ret.old = sp->set_value(std::move(ret.old));
			std::string new_path{path};
			node_ptr new_leaf = std::make_shared<node_t>(NODE_LEAF, new_path, new_path, std::move(value));
			sp->add(new_leaf);
			sp->type(NODE_BRANCH);
			ret.ok = true;
		}
		return ret;
	}

	// template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	// template<typename... Args, std::enable_if_t<std::is_constructible_v<value_t, Args...>, bool>>
	// inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	//         insert(char const *path, Args &&...args) -> return_s {
	//
	// }

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        set_value(value_t &&val) -> ValueT {
		auto ret = std::move(this->_value);
		std::swap(this->_value, val);
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        add(node_ptr it) -> void {
		// weak_node_ptr ptr = node;
		_children.push_back(std::move(it));
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        del(node_ptr it) -> void {
		typename children_t::iterator position = std::find_if(
		        _children.begin(), _children.end(),
		        [it](auto const &ptr) -> bool {
			        return it.get() == ptr.get();
		        });
		if (position != _children.end()) // == myVector.end() means the element was not found
			_children.erase(position);
	}

	inline std::size_t common_prefix(const char *s1, std::size_t const s1_len, const char *s2, std::size_t const s2_len) {
		std::size_t pos{0};
		for (const auto min_size = s1_len < s2_len ? s1_len : s2_len;
		     pos < min_size && *s1 == *s2;
		     ++s1, ++s2)
			pos++;
		// for (; *s1 != '\0' && *s2 != '\0' && *s1 == *s2; ++s1, ++s2)
		// 	pos++;
		// (void) s2_len;
		return pos;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        find(std::string const &path) const -> const_find_return_s {
		return find(path.c_str());
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        find(const char *path) const -> const_find_return_s {
		auto ret = locate(path);
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        fast_find(const char *path) -> find_return_s {
		find_return_s ret;
		if (!path) return ret;
		auto const path_len = std::strlen(path);
		fast_find_internal(ret, path, path_len);
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        fast_find(const char *path) const -> const_find_return_s {
		find_return_s ret;
		if (!path) return ret.to_const();
		auto const path_len = std::strlen(path);
		const_cast<node_t *>(this)->fast_find_internal(ret, path, path_len);
		return ret.to_const();
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        locate(const char *path) const -> const_locate_return_s {
		locate_return_s ret = const_cast<node_t *>(this)->locate_internal(path, weak_node_ptr{});
		return ret.to_const_obj();
		//
		// std::vector<const_weak_node_ptr> *v2 = new std::vector<const_weak_node_ptr>();
		// std::for_each(vec->begin(), vec->end(), [v2](auto it) { v2->push_back(it); });
		//
		// return {pms, v2, ptr, en, matched};
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        locate(const char *path) -> locate_return_s {
		static weak_node_ptr tmp_ptr{};
		locate_return_s ret = this->locate_internal(path, tmp_ptr);
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        find_internal(const char *path) -> find_return_s {
		auto ret = locate_internal(path, weak_node_ptr{});
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        fast_find_internal(find_return_s &ctx, const char *path, std::size_t path_len) -> bool {
		if (_fragment_length == 0) {
			if (_children.size() > 0) {
				// for root node only
				// auto wp_this = this->weak_from_this();
				for (auto const &ch : _children) {
					auto ret1 = ch->fast_find_internal(ctx, path, path_len);
					if (ret1 || ctx.partial_matched_size > 0) {
						return ret1;
					}
				}
			}
			return ctx.matched;
		}

		auto cp = common_prefix(_fragment.c_str(), _fragment_length, path, path_len);
		if (cp == 0) {
			return ctx.matched;
		}

		// auto const path_len = std::strlen(path);
		if (_fragment_length == cp) {
			if (_fragment_length == path_len) {
				ctx.ptr = this->weak_from_this();
				ctx.matched = true;
				return true;
			}

			if (_fragment_length < path_len) {
				auto const *rest = path + _fragment_length;
				auto const rest_len = path_len - _fragment_length;
				for (auto const &ch : _children) {
					auto ret1 = ch->fast_find_internal(ctx, rest, rest_len);
					if (ret1 || ctx.partial_matched_size > 0) {
						return ret1; // partial or fully
					}
				}

				ctx.partial_matched_size = cp;
				ctx.ptr = this->weak_from_this();
				return false;
			}

			// partial matched, and this node fully matched. for example:
			// finding 'app.xmak' in node 'app.x' will return [5, thisnode, true].
			// finding 'app.x' in node 'app.xmak' will return [5. thisnode, false].
			ctx.partial_matched_size = path_len;
			ctx.ptr = this->weak_from_this();
			ctx.matched = true;
			return true;
		}

		if (cp < _fragment_length) {
			if (path_len < _fragment_length) {
				ctx.partial_matched_size = cp;
				ctx.ptr = this->weak_from_this();
				// partial matched, and this node not matched. for example:
				// finding 'app.x' in node 'app.xmak' will return [5. thisnode, false].
				// finding 'app.xmak' in node 'app.x' will return [5, thisnode, true].
				return false;
			}

			// frag_len < path_len
			auto const *rest = path + cp;
			auto const rest_len = path_len - cp;
			for (auto const &ch : _children) {
				auto ret1 = ch->fast_find_internal(ctx, rest, rest_len);
				if (ret1 || ctx.partial_matched_size > 0) {
					return ret1; // partial or fully
				}
			}

			ctx.partial_matched_size = cp;
			ctx.ptr = this->weak_from_this();
			return false;
		}

		return ctx.matched;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        locate_internal(const char *path, weak_node_ptr parent) -> locate_return_s {
		if (!path) {
			return {};
		}

		auto const frag_len = _fragment_length;
		if (frag_len == 0) {
			// for root node only
			auto wp_this = this->weak_from_this();
			for (auto const &ch : _children) {
				auto ret1 = ch->locate_internal(path, wp_this);
				if (ret1.matched || ret1.partial_matched_size > 0) {
					if (ret1.parents == nullptr)
						ret1.parents = new std::vector<weak_node_ptr>{wp_this};
					return ret1;
				}
			}
			return {};
		}

		auto path_len = std::strlen(path);
		auto cp = common_prefix(_fragment.c_str(), frag_len, path, path_len);
		if (cp == 0) {
			return {};
		}

		if (cp == frag_len) {
			weak_node_ptr wp_this = this->weak_from_this();
			if (path_len == frag_len) {
				auto *parents = new std::vector<weak_node_ptr>{parent};
				return {0, wp_this, static_cast<errno_t>(0), true, parents};
			}

			if (path_len > frag_len) {
				auto const *rest = path + frag_len;
				for (auto const &ch : _children) {
					auto ret1 = ch->locate_internal(rest, wp_this);
					if (ret1.matched || ret1.partial_matched_size > 0) {
						if (ret1.parents == nullptr)
							ret1.parents = new std::vector<weak_node_ptr>{parent, wp_this};
						return ret1; // partial or fully
					}
				}

				auto *parents = new std::vector<weak_node_ptr>{parent};
				return {cp, wp_this, 0, false, parents};
			}

			// partial matched, and this node fully matched. for example:
			// finding 'app.xmak' in node 'app.x' will return [5, thisnode, true].
			// finding 'app.x' in node 'app.xmak' will return [5. thisnode, false].
			auto *parents = new std::vector<weak_node_ptr>{parent};
			return {path_len, wp_this, 0, true, parents};
		}

		if (cp < frag_len) {
			weak_node_ptr wp_this = this->weak_from_this();
			if (path_len < frag_len) {
				// partial matched, and this node not matched. for example:
				// finding 'app.x' in node 'app.xmak' will return [5. thisnode, false].
				// finding 'app.xmak' in node 'app.x' will return [5, thisnode, true].
				auto *parents = new std::vector<weak_node_ptr>{parent};
				return {cp, wp_this, 0, false, parents};
			}

			// frag_len < path_len
			auto const *rest = path + cp;
			for (auto const &ch : _children) {
				auto ret1 = ch->locate_internal(rest, wp_this);
				if (ret1.matched || ret1.partial_matched_size > 0) {
					if (ret1.parents == nullptr)
						ret1.parents = new std::vector<weak_node_ptr>{parent, wp_this};
					return ret1; // partial or fully
				}
			}

			auto *parents = new std::vector<weak_node_ptr>{parent};
			return {cp, wp_this, 0, false, parents};
		}

		return {};
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        search(const char *path) -> locate_return_s {
		locate_return_s ret = this->locate_internal(path, weak_node_ptr{});
		if (ret.matched == false) {
			if (auto pos = ret.partial_matched_size > 0) {
				if (auto sp = ret.ptr.lock()) {
					auto const &frag = sp->fragment();
					auto const size = sp->fragment_length();
					if (static_cast<std::size_t>(pos) + 1 == size && frag[size - 1] == delimiter) {
						ret.matched = true;
					}
				}
			}
		}
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        remove(std::string const &path, bool include_children) -> return_s {
		return remove(path.c_str(), include_children);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        removed_fully(return_s &ret, char const *path, const bool include_children,
	                      weak_node_ptr nd_ptr, std::vector<weak_node_ptr> *parents,
	                      errno_t en) -> void {
		(void) path;
		if (node_ptr sp = nd_ptr.lock()) {
			ret.old = sp->set_value(std::move(ret.old));
			if (include_children) {
				if (parents != nullptr) {
					if (node_ptr dad = parents->back().lock()) {
						dad->del(sp);
						ret.en = en;
						ret.ok = true;
						return;
					}
					// lock a weak ptr failure
					ret.en = EACCES;
				} else {
					// nothing to do if no parents
					ret.en = EOWNERDEAD;
				}
				return;
			}

			// cannot do anything:
			// for a branch node, there is no any meaning that erasing it but keeping its children.
			// so there's no action can be done if include_children == false.
			ret.en = EISDIR;
		} else {
			ret.en = EACCES;
		}
		return; // lock a weak ptr failure.
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        remove(char const *path, const bool include_children) -> return_s {
		return_s ret{};
		auto fr = locate_internal(path, weak_node_ptr{});
		ret.en = fr.en;
		if (fr.matched && fr.partial_matched_size == 0) {
			// fully matched
			removed_fully(ret, path, include_children, fr.ptr, fr.parents, fr.en);
			return ret;
		}

		if (fr.partial_matched_size > 0) {
			// partial matched, ...
			if (auto sp = fr.ptr.lock()) {
				auto &frag = sp->fragment();
				auto size = frag.length();
				if (size == fr.partial_matched_size + 1 && frag[size - 1] == delimiter) {
					removed_fully(ret, path, include_children, fr.ptr, fr.parents, fr.en);
					return ret;
				}
			}

			ret.en = ENAMETOOLONG;
		}

		// for errno_t, see:
		//    https://en.cppreference.com/w/cpp/error/errno
		//    https://en.cppreference.com/w/cpp/error/errno_macros
		//    https://learn.microsoft.com/en-us/cpp/c-runtime-library/errno-constants?view=msvc-170
		//    https://stackoverflow.com/questions/7901117/how-do-i-use-errno-in-c

		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        dump(std::ostream &os, const int indent_level) const -> std::ostream & {
		std::stringstream ss;
		if (indent_level > 0)
			ss << std::setw(indent_level * 2) << ' ';
		ss << "<root>\n";
		dump_r(os, ss, indent_level); // don't increase level because the first node is root.
		ss << '\n';
		return os << ss.str();
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        dump_r(std::ostream &os, std::stringstream &ss, const int level) const -> std::ostream & {
		if (_fragment_length > 0) {
			if (level > 0) {
				ss << std::setw(level * 2) << ' ';
				ss << std::left << std::setw(node_t::_dump_left_width - level * 2) << _fragment;
			} else {
				ss << std::left << std::setw(node_t::_dump_left_width) << _fragment;
			}
			ss << " -> ";
			ss << '[';
			if (_type == NODE_BRANCH) ss << 'B' << ']';
			else if (_type == NODE_LEAF) {
				ss << 'L' << ']' << ' ';
				ss << '(' << _path << ')' << ' ';
				ss << _value;
			} else
				ss << ' ' << ']';
			ss << '\n';
		}

		for (auto const &ch : _children) {
			ch->dump_r(os, ss, level + 1);
		}
		return os;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        to_string() const -> std::string {
		std::stringstream ss;
		ss << "";
		return ss.str();
	}

	// template<typename ValueT, char delimiter, typename DescT,typename CommentT,typename TagT>
	// inline auto node<ValueT,  delimiter,  TagT, has_desc, has_comment, has_tag>::
	//         root() const -> weak_node_ptr {
	// 	weak_node_ptr ret = this->shared_from_this();
	// 	return ret;
	// }
} // namespace trie


// trie_t<ValueT, TagT, char delimiter>
namespace trie {
	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::trie_t() {
		ensure_root();
	}

	// template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	// inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	//         insert(const char *path, value_t &&value) -> return_s {
	// 	return _root->insert(path, std::move(value));
	// }

	// template<typename ValueT, char delimiter, typename DescT,typename CommentT,typename TagT>
	// inline auto trie_t<ValueT,  delimiter,  TagT, has_desc, has_comment, has_tag>::insert(std::string const &path, ValueT const &value) -> return_t {
	// 	return _root->insert(path, value);
	// }

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        insert(std::string const &path, value_t &&value) -> return_s {
		return _root->insert(path, value);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        find(std::string const &path) const -> const_find_return_s {
		return _root->find(path.c_str());
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        find(const char *path) const -> const_find_return_s {
		return _root->find(path);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        locate(const char *path) -> locate_return_s {
		auto r = _root->locate(path);
		return r;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        remove(std::string const &path, bool include_children) -> return_s {
		return _root->remove(path.c_str(), include_children);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        remove(char const *path, bool include_children) -> return_s {
		return _root->remove(path, include_children);
	}


	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        has(const char *path, bool partial_match) const -> bool {
		return _root->has(path, partial_match);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        append(char const *path, value_t &&value) -> return_s {
		if (auto ret = _root->search(path); ret.matched) {
			if (auto sp = ret.ptr.lock()) {
				auto old = sp->set_value(value); // TODO append value
				return {old, 0, true};
			}
		}
		return {};
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        update(char const *path, value_t &&value) -> return_s {
		if (auto ret = _root->search(path); ret.matched) {
			if (auto sp = ret.ptr.lock()) {
				return_s r{};
				r.ok = true;
				r.old = sp->set_value(value);
				return r;
			}
		}
		return {};
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        move(char const *path, char const *new_path) -> return_s {
		auto ret = search(path);
		// if (ret.partial_matched_size > 0) {
		// 	if (auto sp = ret.ptr.lock()) {
		// 		auto &frag = sp->fragment();
		// 		auto size = frag.length();
		// 		if (size == ret.partial_matched_size + 1 && frag[size - 1] == delimiter)
		// 			ret.matched = true;
		// 	}
		// }
		if (ret.matched) {
			// TODO move the exists node...
			// auto [pms1, parents1, wp1, en1, matched1] = locate(new_path);
			(void) new_path;
		}
		return ret;
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        dump(std::ostream &os) const -> std::ostream & {
		return _root->dump(os);
	}

	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::
	        ensure_root() -> node_ptr & {
		if (!_root) {
			_root = std::make_unique<node_t>();
		}
		return _root;
	}

	/**
	 * @brief return how many leaves in this tree.
	 * @tparam ValueT
	 * @tparam TagT
	 * @tparam delimiter
	 * @return
	 */
	template<typename ValueT, char delimiter, typename DescT, typename CommentT, typename TagT, typename ExtPkgT>
	inline auto trie_t<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>::size() const -> std::size_t {
		int count{0};
		if (_root) {
			_root->walk([&count](node_type type, const_node_ptr n, int, int) {
				if (type == node_t::NODE_LEAF) count++;
				(void) n;
			});
			// return _root->children_count();
		}
		return count;
	}
} // namespace trie


#endif // TRIE_CXX_TRIE_CORE_HH
