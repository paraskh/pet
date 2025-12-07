#!/usr/bin/env bash
docker run --rm -it \
  -v "$(pwd)":/workspace \
  -w /workspace \
  ghcr.io/paraskh/pet-toolchain:latest \
  bash
