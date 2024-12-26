# trie-cxx and store-cxx

[![CMake Build Matrix](https://github.com/hedzr/trie-cxx/actions/workflows/cmake.yml/badge.svg)](https://github.com/hedzr/trie-cxx/actions/workflows/cmake.yml)
[![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/trie-cxx.svg?label=release)](https://github.com/hedzr/trie-cxx/releases)

`trie-cxx` is a c++20 header-only library, which implements a extensible Trie-tree container.

`trie-cxx` allows accessing the target node data via a dotted key path. This is almost a in-memory key-value store.

## Features

The released codes allowes the extras data attached to the Trie-tree nodes/leaves, such as data (with any type), comment, descriptions, and tags, etc..

```cpp
auto build_trie() -> trie::trie_t<trie::value_t> {
	trie::trie_t<trie::value_t> tt;

	tt.set("app.debug", true);
	tt.set("app.verbose", true);
	tt.set("app.dump", 3);
	tt.set("app.logging.file", "~/.trie.log");
	tt.set("app.server.start", 5);
	tt.set("app.logging.rotate", (unsigned long) (6));
	// tt.set("app.logging.words", std::vector<std::string>{"a", 1, false});
	tt.set("app.logging.words", std::vector<std::string>{"a", "1", "false"});
	tt.set("app.server.sites", 1);
	float f = 2.718f;
	tt.set("app.ref-types.float", std::move(f));
	tt.set("app.ref-types.double", M_PI);
	tt.set("app.ref-types.string", std::string("hello, trie."));
	tt.set("app.ref-types.intvec", std::vector<int>{3, 5, 7, 9});

	return tt;
}
```

Here is an earlier pre-release for sharing the common codes and structures in my mind.
It's certain an unstable encapsulations, which might be updated in the future.
These codes are and will be released under Apache 2.0 for free. So referring to it or copying from it would be okay.

## For Developers

`trie-cxx` needs a c++-20 compliant compiler such as gcc-13.

From v0.3.1, `trie-cxx` was compiled passed under macOS (clang), Linux (gcc-13) and Windows (msvc).

```bash
```

### CMake Standard

cmdr-cxx is findable via CMake Modules.

You could install cmdr-cxx manually:

```bash
git clone https://github.com/hedzr/cmdr-cxx.git
cd cmdr-cxx
cmake -DCMAKE_VERBOSE_DEBUG=ON -DCMAKE_AUTOMATE_TESTS=OFF -S . -B build/ -G Ninja
# Or:
#    cmake -S . -B build/
cmake --build build/
cmake --install build/
# Or:
#   cmake --build build/ --target install
#
# Sometimes sudo it:
#   sudo cmake --build build/ --target install
# Or:
#   cmake --install build/ --prefix ./dist/install --strip
#   sudo cp -R ./dist/install/include/* /usr/local/include/
#   sudo cp -R ./dist/install/lib/cmake/* /usr/local/lib/cmake/
rm -rf ./build
cd ..
```

### More cmake commands

```bash
# clean (all targets files, but the immedieted files)
cmake --build build/ --target clean
# clean and build (just relinking all targets without recompiling)
cmake --build build/ --clean-first

# clean deeply
rm -rf build/

# clean deeply since cmake 3.24.0
# (your custom settings from command-line will lost.
#   For example, if you ever run `cmake -DCMAKE_VERBOSE_DEBUG=ON -S . -B build',
#   and now cmake --fresh -B build/ will ignore `CMAKE_VERBOSE_DEBUG = ON' 
#   and reconfigure to default state.
# )
cmake --fresh -B build/

# recompiling and relinking (simply passing `-B' to `make')
cmake --build build/ -- -B

# reconfigure
rm ./build/CMakeCache.txt && cmake -DENABLE_AUTOMATE_TESTS=OFF -S . -B build/

# print compiling command before exeuting them
cmake --build build/ -- VERBOSE=1
# Or:
VERBOSE=1 cmake --build build/
# Or:
cmake --build build --verbose

# Run CTest
cmake -E chdir build ctest
# Run a special ctest
cmake -E chdir build ctest -R name_of_your_test
```

detect running in CTest by env var `CTEST_INTERACTIVE_DEBUG_MODE`:

```c++
#include <stdlib.h>
char const *ev1 = getenv("CTEST_INTERACTIVE_DEBUG_MODE");
if (ev1 && (*ev1 == '0' || *ev1 == '1'))
    return 0;
```

### Preparing Catch2 v3.x

for testing, catch2 v3.x is necessary.

openSUSE,

```bash
sudo zypper install catch2-devel # don't install catch2-2-devel
```

Ubuntu 22.04/24.04,

```bash
git clone -b v3.0.1 https://github.com/catchorg/Catch2.git
cd Catch2
mkdir build
cmake -S . -B build
cmake --build build
cmake --install build --prefix ./dist/install --strip
sudo cp -R ./dist/install/include/* /usr/local/include/
sudo cp -R ./dist/install/lib/cmake/* /usr/local/lib/cmake/
```

macOS,

```bash
brew install catch2
```

Windows (x86_64)

```powershell
vcpkg install catch2:x64-windows
```

If catch2 not installed via package manager, cmake will try downloading it from github and use it as a child target.

## License

Apache 2.0
