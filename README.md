# LRU and ARC Caches Comparison Benchmark in C++

This repository contains a benchmarking tool for comparing the performance of Least Recently Used (LRU) and Adaptive
Replacement Cache (ARC) algorithms implemented in C++.

## Overview

LRU and ARC are two popular cache replacement algorithms used in computer systems to manage cache contents efficiently.
This benchmarking tool allows users to compare the performance of these algorithms under various scenarios, such as
different cache sizes, workload patterns, and access frequencies.

## Features

- **LRU Cache Implementation**: Efficient LRU cache implementation in C++.
- **ARC Cache Implementation**: Implementation of the ARC cache algorithm in C++.
- **Benchmarking Tool**: A comprehensive benchmarking tool to compare the performance of LRU and ARC caches.
- **Configurable Parameters**: Users can customize cache size, workload, and other parameters for benchmarking.
- **Performance Metrics**: Detailed performance metrics such as hit ratio, miss ratio, and cache eviction rates are
  provided for analysis.

## Usage

1. **Clone the Repository**:

```bash
git clone git
```

2. **Build the Benchmarking Tool**:

```bash
g++ -std=c++20 main.cpp -o lru_arc_benchmark
```

3. **Run Benchmarks**:

```bash
./lru_arc_benchmark
```

4. **Customization**:

- Modify parameters such as cache size, workload pattern, and access frequencies in the source code before building for
  custom benchmarks.

## Results

| Algorithm | Total Examples | Hit    | Miss   | Hit Rate |
|-----------|----------------|--------|--------|----------|
| LRU       | 100,000        | 9,922  | 90,078 | 0.09922  |
| ARC       | 100,000        | 13,633 | 86,367 | 0.13633  |

## Acknowledgments

- This benchmarking tool was inspired by the need for efficient cache management in various computer systems.
- We thank the open-source community for their contributions to cache algorithms and benchmarking tools, which made this
  project possible.

