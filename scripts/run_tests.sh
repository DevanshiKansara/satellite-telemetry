#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

echo "== Build =="
cmake -S . -B build -G Ninja
cmake --build build

echo "== C++ unit tests (GoogleTest) =="
ctest --test-dir build --output-on-failure

echo "== Python tests (pytest) =="
python3 -m pytest tests -v

echo "== ALL TESTS PASSED =="
