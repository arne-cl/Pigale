#!/bin/bash
#
# Convenience script to build and run the Pigale test suite
#

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Pigale Test Suite Builder ===${NC}"

# Check if tgraph library exists
if [ ! -f ../lib/libtgraph.a ] && [ ! -f ../lib/libtgraph_debug.a ]; then
    echo -e "${YELLOW}Warning: tgraph library not found. Building it first...${NC}"
    cd ../tgraph
    qmake tgraph.pro
    make
    cd ../tests
    echo -e "${GREEN}tgraph library built successfully${NC}"
fi

# Create build directory
echo -e "${GREEN}Creating build directory...${NC}"
mkdir -p build
cd build

# Configure with CMake
echo -e "${GREEN}Configuring with CMake...${NC}"
cmake ..

# Build
echo -e "${GREEN}Building tests...${NC}"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# Run tests
echo ""
echo -e "${GREEN}=== Running Unit Tests ===${NC}"
./unit_tests || echo -e "${RED}Unit tests failed!${NC}"

echo ""
echo -e "${GREEN}=== Running Integration Tests ===${NC}"
./integration_tests || echo -e "${RED}Integration tests failed!${NC}"

echo ""
echo -e "${GREEN}=== Test Suite Complete ===${NC}"
