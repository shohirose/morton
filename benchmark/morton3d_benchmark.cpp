#include <benchmark/benchmark.h>

#include <limits>
#include <random>
#include <vector>

#include "morton/morton3d.hpp"

using namespace morton3d;

template <typename T, int N, typename Tag>
void RunBenchmark(benchmark::State& state) {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_int_distribution<T> dist(0, (static_cast<T>(1) << N) - 1);
  std::vector<coordinates<T>> coords(state.range(0));

  for (auto _ : state) {
    state.PauseTiming();
    for (auto&& c : coords) {
      c.x = dist(engine);
      c.y = dist(engine);
    }
    state.ResumeTiming();
    for (int i = 0; i < state.range(0); ++i) {
      const auto c = encode(coords[i], Tag{});
      benchmark::DoNotOptimize(c);
    }
  }
}

static void BM_32BitEncodingUsingPreshiftedLookupTable(
    benchmark::State& state) {
  RunBenchmark<uint16_t, 10, tag::preshifted_lookup_table>(state);
}
BENCHMARK(BM_32BitEncodingUsingPreshiftedLookupTable)->Range(8, 8 << 10);

static void BM_32BitEncodingUsingLookupTable(benchmark::State& state) {
  RunBenchmark<uint16_t, 10, tag::lookup_table>(state);
}
BENCHMARK(BM_32BitEncodingUsingLookupTable)->Range(8, 8 << 10);

static void BM_32BitEncodingUsingMagicBits(benchmark::State& state) {
  RunBenchmark<uint16_t, 10, tag::magic_bits>(state);
}
BENCHMARK(BM_32BitEncodingUsingMagicBits)->Range(8, 8 << 10);

#ifdef MORTON3D_USE_BMI
static void BM_32BitEncodingUsingBmi(benchmark::State& state) {
  RunBenchmark<uint16_t, 10, tag::bmi>(state);
}
BENCHMARK(BM_32BitEncodingUsingBmi)->Range(8, 8 << 10);
#endif  // MORTON3D_USE_BMI

static void BM_64BitEncodingUsingPreshiftedLookupTable(
    benchmark::State& state) {
  RunBenchmark<uint32_t, 21, tag::preshifted_lookup_table>(state);
}
BENCHMARK(BM_64BitEncodingUsingPreshiftedLookupTable)->Range(8, 8 << 10);

static void BM_64BitEncodingUsingLookupTable(benchmark::State& state) {
  RunBenchmark<uint32_t, 21, tag::lookup_table>(state);
}
BENCHMARK(BM_64BitEncodingUsingLookupTable)->Range(8, 8 << 10);

static void BM_64BitEncodingUsingMagicBits(benchmark::State& state) {
  RunBenchmark<uint32_t, 21, tag::magic_bits>(state);
}
BENCHMARK(BM_64BitEncodingUsingMagicBits)->Range(8, 8 << 10);

#ifdef MORTON3D_USE_BMI
static void BM_64BitEncodingUsingBmi(benchmark::State& state) {
  RunBenchmark<uint32_t, 21, tag::bmi>(state);
}
BENCHMARK(BM_64BitEncodingUsingBmi)->Range(8, 8 << 10);
#endif  // MORTON3D_USE_BMI

BENCHMARK_MAIN();