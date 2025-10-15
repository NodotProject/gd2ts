#!/bin/bash
# Test runner script for gd2ts GDExtension

set -e  # Exit on error

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

GODOT_VERSION="4.5.0"

echo -e "${BLUE}=== gd2ts Test Runner ===${NC}"
echo

# Check if Godot is installed
if ! command -v godot &> /dev/null; then
    echo -e "${RED}ERROR: Godot not found in PATH${NC}"
    echo -e "${YELLOW}Install Godot ${GODOT_VERSION} or add it to your PATH${NC}"
    echo -e "${YELLOW}Download from: https://godotengine.org/download${NC}"
    exit 1
fi

# Check Godot version
INSTALLED_VERSION=$(godot --version 2>&1 | head -n 1 | cut -d'.' -f1-3)
echo -e "${GREEN}Found Godot: ${INSTALLED_VERSION}${NC}"
echo

# Check if extension is built
if [ ! -d "addons/gd2ts/bin" ] || [ -z "$(ls -A addons/gd2ts/bin/*.so addons/gd2ts/bin/*.dll addons/gd2ts/bin/*.dylib 2>/dev/null)" ]; then
    echo -e "${YELLOW}WARNING: Extension binaries not found in addons/gd2ts/bin/${NC}"
    echo -e "${YELLOW}Run ./build.sh first to build the extension${NC}"
    echo
fi

# Parse command line arguments
TEST_PATTERN="test_"
VERBOSE=false
SPECIFIC_TEST=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -t|--test)
            SPECIFIC_TEST="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo
            echo "Options:"
            echo "  -v, --verbose       Enable verbose output"
            echo "  -t, --test NAME     Run specific test (e.g., test_async_await)"
            echo "  -h, --help          Show this help message"
            echo
            echo "Examples:"
            echo "  $0                          # Run all tests"
            echo "  $0 -v                       # Run all tests with verbose output"
            echo "  $0 -t test_async_await      # Run only async/await tests"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# Build GUT command
GUT_ARGS="-gexit"

if [ "$VERBOSE" = true ]; then
    GUT_ARGS="$GUT_ARGS -glog=2"  # Verbose logging
fi

if [ -n "$SPECIFIC_TEST" ]; then
    echo -e "${BLUE}Running specific test: ${SPECIFIC_TEST}${NC}"
    GUT_ARGS="$GUT_ARGS -gtest=${SPECIFIC_TEST}"
else
    echo -e "${BLUE}Running all tests...${NC}"
fi

echo

# Import project first (required for GDExtensions)
echo -e "${BLUE}Importing project...${NC}"
timeout 60 godot --headless --import --quit 2>&1 || true
echo

# Run tests using GUT
echo -e "${BLUE}Executing tests...${NC}"
echo

# Run godot with GUT command line tool
if godot --headless --path . -s addons/gut/gut_cmdln.gd $GUT_ARGS; then
    echo
    echo -e "${GREEN}=== All tests passed! ===${NC}"
    exit 0
else
    TEST_EXIT_CODE=$?
    echo
    echo -e "${RED}=== Tests failed with exit code $TEST_EXIT_CODE ===${NC}"
    exit $TEST_EXIT_CODE
fi
