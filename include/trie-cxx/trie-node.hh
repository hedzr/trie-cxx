// @copy Copyright © 2016 - 2024 Hedzr Yeh.
//
// trie - C++17/C++20 Text Difference Utilities Library
//
// This file is part of trie.
//
// trie is free software: you can redistribute it and/or modify
// it under the terms of the Apache 2.0 License.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2024/12/1.
//

#ifndef TRIE_CXX_TRIE_NODE_HH
#define TRIE_CXX_TRIE_NODE_HH

#include "trie-base.hh"
#include "trie-chrono.hh"

// errno_t
namespace trie {
#if defined(__GNUC__)
	using errno_t = int;
#endif
} // namespace trie

// value_t
namespace trie {
	class value_t : public base_t {
	public:
		using base_t::base_t;
		~value_t() = default;
	};

	inline std::ostream &operator<<(std::ostream &os, value_t const &o) {
		return variant_to_string(os, o);
	}
} // namespace trie

// has_member_impl
namespace trie::traits {
	template<typename T, typename F>
	constexpr auto has_member_impl(F &&f) -> decltype(f(std::declval<T>()), true) { return true; }

	template<typename>
	constexpr bool has_member_impl(...) { return false; }

	/**
	 * @brief
	 * @details For example:
	 * @code{c++}
	 * template&lt;class T&gt;
	 * std::string optionalToString(T* obj) {
	 *     if constexpr(has_member(T, toString()))
	 *         return obj->toString();
	 *     else
	 *         return "toString not defined";
	 * }
	 *
	 * @endcode
	 */
#define has_member(T, EXPR) store::traits::has_member_impl<T>([](auto &&obj) -> decltype(obj.EXPR) {})
} // namespace trie::traits

// dummy, void_desc, void_comment, void_tag, ...
namespace trie::extensions {
	struct dummy {
		static char c;
	};

	inline char dummy::c = '\0';

	struct [[maybe_unused]] void_desc {
		using desc_t = char;
		char const &desc() const { return dummy::c; }
		void desc(char const &) {
		}
	};

	struct [[maybe_unused]] void_comment {
		using comment_t = int;
		char const &comment() const { return dummy::c; }
		void comment(char const &) {
		}
	};

	struct [[maybe_unused]] void_tag {
		using tag_t = int;
		char const &tag() const { return dummy::c; }
		void tag(char const &) {
		}
	};

	template<typename T = std::string>
	class description_holder {
	public:
		using desc_t = T;
		desc_t const &desc() const { return _description; } // leaf node's description
		void desc(desc_t const &s) { _description = s; }

	private:
		desc_t _description{}; // description
	};

	template<typename T = std::string>
	class comment_holder {
	public:
		using comment_t = T;
		comment_t const &comment() const { return _comment; } // leaf node's comment
		void comment(comment_t const &s) { _comment = s; }

	private:
		comment_t _comment{}; // description
	};

	template<typename T = std::any>
	class tag_holder {
	public:
		using tag_t = T;
		tag_t const &tag() const { return _tag; } // leaf node's tagged value
		void tag(tag_t const &s) { _tag = s; }

	private:
		tag_t _tag{}; // description
	};
} // namespace trie::extensions

// ext_package
namespace trie::extensions::detail {
	template<typename DescT = extensions::void_desc,       // use description_holder if u'd like
	         typename CommentT = extensions::void_comment, // use comment_holder if u'd like
	         typename TagT = extensions::void_tag>         // use tag_holder if u'd like
	struct [[maybe_unused]] ext_package
	    : public DescT
	    , public CommentT
	    , public TagT {
	};
} // namespace trie::extensions::detail

// store_node<>
namespace trie {
	template<typename ValueT,
	         char delimiter = '.',
	         typename DescT = extensions::void_desc,       // use description_holder if u'd like
	         typename CommentT = extensions::void_comment, // use comment_holder if u'd like
	         typename TagT = extensions::void_tag,         // use tag_holder if u'd like
	         typename ExtPkgT = extensions::detail::ext_package<DescT, CommentT, TagT>>
	class store_node {
	public:
		enum NodeType {
			NODE_NONE,
			NODE_LEAF,
			NODE_BRANCH,
		};

		using ext_pkg_t = ExtPkgT;

		using node_t = store_node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>;
		using value_t = ValueT;
		using desc_t = typename DescT::desc_t;
		using comment_t = typename CommentT::comment_t;
		using tag_t = typename TagT::tag_t;
		using node_type = NodeType;
		using node_ptr = node_t *;             // std::shared_ptr<node_t>;
		using const_node_ptr = node_t const *; // std::shared_ptr<node_t const>;
		// using weak_node_ptr = std::weak_ptr<node_t>;
		// using const_weak_node_ptr = std::weak_ptr<node_t const>;
		using children_t = std::vector<node_ptr>;

	private:
		node_type _type{NODE_NONE}; // node type
		std::string _path{};        // full path to this node
		std::string _fragment{};    // path fragment
		std::size_t _fragment_length{0};
		value_t _value{};       // the payload
		children_t _children{}; // children nodes
		ext_pkg_t _pkg{};

	public:
		store_node() = default;
		~store_node() = default;

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

#if 0
	public:
		// auto insert(std::string const &path, value_t const &value) -> return_s; // insert or update
		auto insert(std::string const &path, value_t &&value) -> return_s;
		// auto insert(char const *path, ValueT const &value) -> return_s; // insert or update
		auto insert(char const *path, value_t &&value) -> return_s;
		auto insert(char const *path, char const *value) -> return_s;
		template<typename... Args, std::enable_if_t<std::is_constructible_v<value_t, Args...>, bool> = true>
		auto insert(char const *path, Args &&...args) -> bool {
			bool ret{};
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
#endif

	}; // class store_node
} // namespace trie

#endif //TRIE_CXX_TRIE_NODE_HH
