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

#ifndef TRIE_CXX_TRIE_STORE_HH
#define TRIE_CXX_TRIE_STORE_HH

#include "trie-base.hh"
#include "trie-chrono.hh"
#include "trie-node.hh"


namespace trie {
	template<typename ValueT,
		char delimiter = '.',
		typename DescT = extensions::void_desc, // use description_holder if u'd like
		typename CommentT = extensions::void_comment, // use comment_holder if u'd like
		typename TagT = extensions::void_tag, // use tag_holder if u'd like
		typename ExtPkgT = extensions::detail::ext_package<DescT, CommentT, TagT> >
	class store_t {
	public:
		using node_t = store_node<ValueT, delimiter, DescT, CommentT, TagT, ExtPkgT>;
		using value_t = typename node_t::value_t;
		using desc_t = typename node_t::desc_t;
		using comment_t = typename node_t::comment_t;
		using tag_t = typename node_t::tag_t;
		using walk_cb = typename node_t::walk_cb;
		using node_type = typename node_t::node_type;
		using node_ptr = typename node_t::node_ptr;
		using const_node_ptr = typename node_t::const_node_ptr;
		// using weak_node_ptr = typename node_t::weak_node_ptr;
		// using return_t = typename node_t::return_t;

	private:
		node_ptr _root{};
		node_ptr _empty{};

	public:
		store_t();
		~store_t() = default;

	public:
		// auto insert(std::string const &path, value_t const &value) -> return_s; // insert or update
		auto insert(std::string const &path, value_t &&value) -> return_s;
		// auto insert(char const *path, value_t const &value) -> return_s; // insert or update
		// auto insert(char const *path, value_t &&value) -> return_s;
		template<typename... Args, std::enable_if_t<std::is_constructible_v<value_t, Args...>, bool>  = true>
		auto insert(char const *path, Args &&... args) -> return_s {
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
	}; //class store_t
}

#endif //TRIE_CXX_TRIE_STORE_HH
