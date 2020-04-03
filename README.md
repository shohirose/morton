# Morton
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

This is a modified version of [`libmorton`](https://github.com/Forceflow/libmorton) originally created by @Forceflow. Encoding and decoding morton codes using pre-shifted look-up tables in `libmorton` are implemented here for self-learning.　Unit tests are created by using [Google Test](https://github.com/google/googletest).

## Usage

Include `morton/morthon.hpp`.

```cpp
// 32/64 bits encoding/decoding in two dimensions.
uint_fast32_t morton::encode(const uint_fast16_t x, const uint_fast16_t y);
void morton::decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y);
uint_fast64_t morton::encode(const uint_fast32_t x, const uint_fast32_t y);
void morton::decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y);

// 32/64 bits encoding/decoding in three dimensions.
uint_fast32_t morton::encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
void morton::decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y, const uint_fast16_t z);
uint_fast64_t morton::encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z);
void morton::decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y, const uint_fast32_t z);
```

## Testing

`test` directory contains unit tests for the above functions by using Google Test.

## Build

You can build and test by using [CMake](https://cmake.org/). Type the following commands under the project root directory.

```terminal
mkdir build
cd build
cmake ..   # Configure with make
make       # Build
ctest      # Run unit tests
```

You can use [Ninja](https://ninja-build.org/) instead of make by typing the following command.

```terminal
cmake .. -G Ninja   # Configure with Ninja
ninja               # Build with Ninja
```

## Citation

Please follow the instruction written in [`libmorton`](https://github.com/Forceflow/libmorton).