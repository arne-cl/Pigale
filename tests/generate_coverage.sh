#!/bin/bash
# Coverage Report Generation Script
# Generates HTML coverage reports from test execution

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}===================================================${NC}"
echo -e "${BLUE}  Pigale Test Suite - Coverage Report Generator${NC}"
echo -e "${BLUE}===================================================${NC}"
echo ""

# Check if running from tests directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${YELLOW}Error: Must run from tests/ directory${NC}"
    exit 1
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Error: build/ directory not found. Run cmake first.${NC}"
    exit 1
fi

cd build

echo -e "${GREEN}[1/6] Cleaning old coverage data...${NC}"
find . -name "*.gcda" -delete
find . -name "coverage*.info" -delete
rm -rf coverage_html
echo "      Cleaned."

echo ""
echo -e "${GREEN}[2/6] Running unit tests...${NC}"
./unit_tests --gtest_brief=1
echo ""

echo -e "${GREEN}[3/6] Running integration tests...${NC}"
./integration_tests --gtest_brief=1
echo ""

echo -e "${GREEN}[4/6] Capturing coverage data...${NC}"
lcov --capture --directory . --output-file coverage.info \
    --ignore-errors mismatch,gcov,source \
    2>&1 | grep -E "Processing|Finished" || true
echo "      Coverage data captured."
echo ""

echo -e "${GREEN}[5/6] Filtering coverage data...${NC}"
lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/googlemock/*' '*/_deps/*' \
    --output-file coverage_filtered.info \
    --ignore-errors unused \
    2>&1 | grep -E "Deleted|lines|Writing"
echo ""

echo -e "${GREEN}[6/6] Generating HTML report...${NC}"
genhtml coverage_filtered.info --output-directory coverage_html \
    --title "Pigale Test Suite Coverage" \
    --legend \
    2>&1 | tail -5

echo ""
echo -e "${BLUE}===================================================${NC}"
echo -e "${GREEN}✓ Coverage report generated successfully!${NC}"
echo -e "${BLUE}===================================================${NC}"
echo ""
echo -e "  Report location: ${YELLOW}tests/build/coverage_html/index.html${NC}"
echo ""
echo -e "  Open with: ${YELLOW}firefox tests/build/coverage_html/index.html${NC}"
echo -e "         or: ${YELLOW}xdg-open tests/build/coverage_html/index.html${NC}"
echo ""

# Display summary
echo -e "${BLUE}Coverage Summary:${NC}"
lcov --summary coverage_filtered.info 2>&1 | grep -E "lines|functions"
echo ""
