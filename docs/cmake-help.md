# CMake Help

## Build

```bash
# generate the building config
cmake -B build/
cmake -S . -B build/ -G Ninja  # # Or: configure with ninja

# build the whole project (with unittest too)
cmake --build build/

# CTest
cmake -E chdir build ctest

# install
cmake --install build/
# Or: cmake --build build/ --target install
#
# 'sudo' might be needed in certain (a) OS(es):
#   sudo cmake --build build/ --target install
# Or use a locall 'install' directory:
#   cmake --install build/ --prefix ./install --strip
#   sudo cp -R ./install/include/* /usr/local/include/
#   sudo cp -R ./install/lib/cmake/fsm_cxx /usr/local/lib/cmake/
```

## See also

In chinese: <https://hedzr.github.io/tags/#cmake-hello>
