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
// Created by Hedzr Yeh on 2024/9/16.
//

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TREE_WIDTH 256

#define WORDLENMAX 128

struct trie_node_st {
	int count;
	int pass; //add a count for the part-include for example 'this is' then the 'is' is hited two times
	struct trie_node_st *next[TREE_WIDTH];
};

static struct trie_node_st root = {0, 0, {nullptr}};

static const char *spaces = " \t\n/.\"\'()";

void myfree(struct trie_node_st *rt) {
	for (int i = 0; i < TREE_WIDTH; i++) {
		if (rt->next[i] != nullptr) {
			myfree(rt->next[i]);
			rt->next[i] = nullptr;
		}
	}
	free(rt);
	return;
}

static int
insert(const char *word) {
	int i;
	struct trie_node_st *curr, *newnode;

	if (word[0] == '\0') {
		return 0;
	}
	curr = &root;
	for (i = 0;; ++i) {
		if (word[i] == '\0') {
			break;
		}
		curr->pass++; //count
		if (curr->next[(int) word[i]] == nullptr) {
			newnode = (struct trie_node_st *) malloc(sizeof(struct trie_node_st));
			memset(newnode, 0, sizeof(struct trie_node_st));
			curr->next[(int) word[i]] = newnode;
		}
		curr = curr->next[(int) word[i]];
	}
	curr->count++;

	return 0;
}

static void
printword(const char *str, int n) {
	printf("%s\t%d\n", str, n);
}

static int
do_travel(struct trie_node_st *rootp) {
	static char worddump[WORDLENMAX + 1];
	static int pos = 0;
	int i;

	if (rootp == nullptr) {
		return 0;
	}
	if (rootp->count) {
		worddump[pos] = '\0';
		printword(worddump, rootp->count + rootp->pass);
	}
	for (i = 0; i < TREE_WIDTH; ++i) {
		worddump[pos++] = i;
		do_travel(rootp->next[i]);
		pos--;
	}
	return 0;
}

// Run this:
//
//    ./bin/test-trie-1 <./testdata/trie-dict.txt
//
// Or run it in a terminal directly, and input the dict
// line by line, and end the input by typing CTRL-D
// finally, which means give an EOF char to `getline()`.
//
// Sometimes you might have to type CTRL-D twice to
// end the input processing while you're inputing a word
// without line-end. At this time, first CTRL-D does
// action like line-end char to end one line, and the
// second CTRL-D does end the whole input processing.
//
// If you're running under Windows, CTRL-Z is EOF char.
int main(void) {
	char const *ev = getenv("CI_RUNNING");
	if (ev && *ev == '1')
		return 0;

	char const *ev1 = getenv("CTEST_INTERACTIVE_DEBUG_MODE");
	if (ev1 && (*ev1 == '0' || *ev1 == '1'))
		return 0;

	// list all environment variables
	extern char **environ;
	std::ofstream out("ctest-env-vars.log");
	for (char **current = environ; *current; current++) {
		printf("ENV: %s\n", *current);
		out << (*current) << '\n';
	}
	out.close();

	char *linebuf = nullptr, *line, *word;
	size_t bufsize = 0;
	int ret;

	while (1) {
		ret = getline(&linebuf, &bufsize, stdin);
		if (ret == -1) {
			break;
		}
		line = linebuf;
		while (1) {
			word = strsep(&line, spaces);
			if (word == nullptr) {
				break;
			}
			if (word[0] == '\0') {
				continue;
			}
			insert(word);
		}
	}

	do_travel(&root);

	free(linebuf);

	for (int i = 0; i < TREE_WIDTH; i++) {
		if (root.next[i] != nullptr) {
			myfree(root.next[i]);
		}
	}

	// exit(0);
	return 0;
}
