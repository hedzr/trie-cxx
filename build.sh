#!/bin/bash

#
# @copy Copyright © 2016 - 2024 Hedzr Yeh.
#
# trie - C++17/C++20 Text Difference Utilities Library
#
# This file is part of trie.
#
# trie is free software: you can redistribute it and/or modify
# it under the terms of the Apache 2.0 License.
# Read /LICENSE for more information.
#

BUILD_DIR=./build
INSTALL_TMP_DIR=./bin/install
OSN=trie-cxx
VER=0.1.0

[ -f .version.cmake ] && {
	VER=$(echo $(grep -oE ' \d+\.\d+\.\d+' .version.cmake))
	# grep -oE ' [0-9]+.[0-9]+.[0-9]+' .version.cmake
	# echo "VERSION = $VER"
}

#

alias cmake="cmake -Werror=dev --warn-uninitialized"

#

#

#

build-clean() {
	[ -d $BUILD_DIR ] && rm -rf $BUILD_DIR/ "$@"
}

build-prepare() {
	cmake -S . -B $BUILD_DIR/ -G Ninja "$@"
}

build-build() {
	cmake --build $BUILD_DIR/ "$@"
}

build-install() {
	cmake --install $BUILD_DIR/ --prefix $INSTALL_TMP_DIR --strip "$@"
	# sudo cp -R $INSTALL_TMP_DIR/include/* /usr/local/include/
	# sudo cp -R $INSTALL_TMP_DIR/lib/cmake/fsm_cxx /usr/local/lib/cmake/
}

build-ctest() {
	cmake -E chdir $BUILD_DIR ctest "$@"
}

build-cpack() {
	cpack --config $BUILD_DIR/CPackConfig.cmake -B $BUILD_DIR/ "$@"
}

build-doc() {
	cmake -H. -B$BUILD_DIR/docs -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DBUILD_DOCUMENTATION=1
	cmake --build $BUILD_DIR/docs "$@"
}

build-pi() {
	# package info
	local FILE="$BUILD_DIR/packages/$OSN-$VER-1.src.rpm"
	[ -f $FILE ] && {
		ls -laG $FILE
		rpm -qip "$FILE"
		rpm -qpl "$FILE"
		xyz="ARCH"
		rpm -qp --qf %{${xyz}} "$FILE"
		echo ""
	}

	echo
	echo
	FILE="$BUILD_DIR/packages/$OSN-$VER-Source.deb"
	[ -f $FILE ] && {
		ls -laG $FILE
		dpkg -I "$FILE"
		#xyz="ARCH"
		#rpm -qp --qf %{${xyz}} "$FILE"
		#echo ""
	}
	# echo
}

#
#
#
# sync project files with iCloud backup point --------------------------
#
#

iCloud="$HOME/Library/Mobile Documents/com~apple~CloudDocs/Nextcloud.new"

build-rsync-cmake-to() {
	for tgt in "$iCloud/dev/cc.icloud"; do # "../log-cxx" "../trie-cxx" "../new-cxx" "../xlib-cxx" "../aml-cxx"; do
		echo
		echo
		echo ---------------------- syncing to "$tgt" ...
		do-rsync-cmake-to "$tgt" "$@"
	done
}

do-rsync-cmake-to() {
	local tgt="${1:-$iCloud/dev/cc.icloud}" && shift

	for f in build.sh cmake-tool.sh .editorconfig .gitignore .clang-* .markdown* .option* LICENSE* Makefile* meson*; do
		if [ -f "$f" ]; then
			rsync -avrztopg --partial --delete --progress \
				"$@" \
				"$f" "$tgt/"
		fi
	done

	rsync -avrztopg --partial --delete --progress \
		--exclude .DS_Store --exclude '*.log' --exclude '*.tmp' --exclude 'cmake.alias.*' \
		"$@" \
		./cmake "$tgt/"
}

build-rsync-cmake-from() {
	local src="$iCloud/dev/cc.icloud"
	echo
	echo
	echo ---------------------- syncing from "$src" ...
	do-rsync-cmake-from "$src" "$@"
}

build-rsync-cmake-from-icloud-to-others() {
	local src="$iCloud/dev/cc.icloud"
	for p in ctl-cxx diff-cxx log-cxx new-cxx xlib-cxx; do
		echo
		echo
		echo ---------------------- syncing from "$src" - ... >"$p"
		pushd "../$p" >/dev/null
		echo "PWD: $(pwd)"
		echo "SRC: $src"
		do-rsync-cmake-from "$src" "$@"
		popd >/dev/null
	done
}

do-rsync-cmake-from() {
	local src="${1:-$iCloud/dev/cc.icloud}" && shift

	for f in build.sh cmake-tool.sh .editorconfig .gitignore .clang-* .markdown* .option* LICENSE* Makefile* meson*; do
		if [ -f "$src/$f" ]; then
			rsync -avrztopg --partial --delete --progress \
				"$@" \
				"$src/$f" ./
		fi
	done

	rsync -avrztopg --partial --delete --progress \
		--exclude .DS_Store --exclude '*.log' --exclude '*.tmp' --exclude 'cmake.alias.*' \
		"$@" \
		"$src/cmake" ./ # --dry-run
}

#

#

#

##
##

cmd_exists() { command -v $1 >/dev/null; } # it detects any builtin or external commands, aliases, and any functions
fn_exists() { LC_ALL=C type $1 2>/dev/null | grep -qE '(shell function)|(a function)'; }
fn_builtin_exists() { LC_ALL=C type $1 2>/dev/null | grep -q 'shell builtin'; }
fn_aliased_exists() { LC_ALL=C type $1 2>/dev/null | grep -qE '(alias for)|(aliased to)'; }

cmd="$1" && (($#)) && shift
fn_exists "$cmd" && {
	eval $cmd "$@"
	unset cmd
} || {
	xcmd="cmake-$cmd" && fn_exists "$xcmd" && eval $xcmd "$@" || {
		xcmd="build-$cmd" && fn_exists "$xcmd" && eval $xcmd "$@" || {
			xcmd="build-c$cmd" && fn_exists "$xcmd" && eval $xcmd "$@"
		}
	}
	unset cmd xcmd
}
