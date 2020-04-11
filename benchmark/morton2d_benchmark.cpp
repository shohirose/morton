#include <benchmark/benchmark.h>

#include <random>
#include <vector>

#include "morton/morton2d.hpp"

using namespace morton2d;

template <typename T, typename Tag>
void BM_Morton2dEncoding(benchmark::State& state) {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_int_distribution<T> dist;
  std::vector<coordinates<T>> coords(state.range(0));
  for (auto&& c : coords) {
    c.x = dist(engine);
    c.y = dist(engine);
  }

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      const auto c = encode(coords[i], Tag{});
      benchmark::DoNotOptimize(c);
    }
  }
}

BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint16_t, tag::preshifted_lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint16_t, tag::lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint16_t, tag::magic_bits)
    ->Range(8, 8 << 10);
#ifdef MORTON2D_USE_BMI
BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint16_t, tag::bmi)->Range(8, 8 << 10);
#endif  // MORTON2d_USE_BMI

BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint32_t, tag::preshifted_lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint32_t, tag::lookup_table)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint32_t, tag::magic_bits)
    ->Range(8, 8 << 10);
#ifdef MORTON2D_USE_BMI
BENCHMARK_TEMPLATE(BM_Morton2dEncoding, uint32_t, tag::bmi)->Range(8, 8 << 10);
#endif  // MORTON2d_USE_BMI

BENCHMARK_MAIN();