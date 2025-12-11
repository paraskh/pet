#!/usr/bin/env bash
set -euo pipefail

# Build script for pet-toolchain Docker image
# Usage: ./scripts/build-toolchain.sh [tag]
#   tag: Optional tag name (default: local)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

TAG="${1:-local}"
IMAGE_NAME="pet-toolchain:${TAG}"
DOCKERFILE="${PROJECT_ROOT}/docker/Dockerfile.toolchain"

echo "Building Docker toolchain image..."
echo "  Image: ${IMAGE_NAME}"
echo "  Dockerfile: ${DOCKERFILE}"
echo "  Platform: linux/amd64"
echo ""

cd "${PROJECT_ROOT}"

docker build \
  --platform linux/amd64 \
  -f "${DOCKERFILE}" \
  -t "${IMAGE_NAME}" \
  .

echo ""
echo "✓ Successfully built ${IMAGE_NAME}"
echo ""
echo "To use this image locally, update scripts/dev-shell.sh or run:"
echo "  docker run --rm -it -v \"\$(pwd)\":/workspace -w /workspace ${IMAGE_NAME} bash"

