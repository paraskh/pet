#!/usr/bin/env bash
set -euo pipefail

# Development shell script for pet-toolchain Docker image
# Tries to use local image first, falls back to remote image
# Optimized for Mac development with proper volume mounting

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Try local image first, fallback to remote
LOCAL_IMAGE="pet-toolchain:local"
REMOTE_IMAGE="ghcr.io/paraskh/pet-toolchain:latest"

if docker image inspect "${LOCAL_IMAGE}" &>/dev/null; then
  IMAGE="${LOCAL_IMAGE}"
  echo "Using local image: ${IMAGE}"
else
  IMAGE="${REMOTE_IMAGE}"
  echo "Using remote image: ${IMAGE}"
  echo "Tip: Build local image with: ./scripts/build-toolchain.sh"
fi

# Detect OS for volume mount optimization
if [[ "$(uname)" == "Darwin" ]]; then
  # macOS: use cached mount for better performance
  VOLUME_OPT=":cached"
  echo "Detected macOS - using cached volume mount"
else
  VOLUME_OPT=""
fi

cd "${PROJECT_ROOT}"

# Run container (as root for now - can be improved with non-root user in Dockerfile)
docker run --rm -it \
  --platform linux/amd64 \
  -v "${PROJECT_ROOT}:/workspace${VOLUME_OPT}" \
  -w /workspace \
  "${IMAGE}" \
  bash
