#include <benchmark/benchmark.h>

#include <limits>
#include <random>
#include <vector>

#include "morton/morton3d.hpp"

using namespace morton3d;

template <typename T, int MaxBits, typename Tag>
void BM_Morton3dEncoding(benchmark::State& state) {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_int_distribution<T> dist(0, (T(1) << MaxBits) - 1);
  std::vector<coordinates<T>> coords(state.range(0));
  for (auto&& c : coords) {
    c.x = dist(engine);
    c.y = dist(engine);
    c.z = dist(engine);
  }

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      const auto c = encode(coords[i], Tag{});
      benchmark::DoNotOptimize(c);
    }
  }
}

BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint16_t, 10,
                   tag::preshifted_lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint16_t, 10, tag::lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint16_t, 10, tag::magic_bits)
    ->Range(8, 8 << 10);
#ifdef MORTON3D_USE_BMI
BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint16_t, 10, tag::bmi)
    ->Range(8, 8 << 10);
#endif

BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint32_t, 21,
                   tag::preshifted_lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint32_t, 21, tag::lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint32_t, 21, tag::magic_bits)
    ->Range(8, 8 << 10);
#ifdef MORTON3D_USE_BMI
BENCHMARK_TEMPLATE(BM_Morton3dEncoding, uint32_t, 21, tag::bmi)
    ->Range(8, 8 << 10);
#endif

BENCHMARK_MAIN();