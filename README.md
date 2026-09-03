# Os tools

C++23 library providing winapi & linux api wrappers

## Usage

Before start you need to initialize git submodule `import-std` in order to enable experimental cmake support of `import std;` which is used in code
```console
git submodule update --init
```

Installation:
```console
cmake --preset=<dev|release>-<your-system>
ninja -C out/build/<preset-name>
cmake --install out/build/<preset-name> [--prefix <path-to-install>]
```
By default tests will be compiled, so you need boost::unit_test_framework to be installed and added to path resolution. If you do not want to use path resolution, add -DBOOST_ROOT=<path-to-boost-config> to `cmake --preset=<>` call (e.g `cmake --preset=dev-win64 -DBOOST_ROOT=D:\frameworks\boost\boost_1_84_0\stage\lib\cmake\Boost-1.84.0`)

In your projects you need to include this cmake code *before* project() call:
```cmake
find_package(os-tools REQUIRED)
enable_experimental_std()
```
And this code for every target:
```cmake
target_link_libraries(<target> PRIVATE os-tools::os-tools)
enable_cxx_modules(<target> STANDARD 23)
```

## Example

```cpp
```
