#!/bin/bash
# ================================================================
#  CAMPUS PATHFINDER - Auto Setup & Run Script
#  Amrita Vishwa Vidyapeetham - Nagercoil Campus
#
#  HOW TO USE (in WSL Ubuntu PowerShell):
#    chmod +x setup.sh
#    ./setup.sh
# ================================================================

YELLOW='\033[1;33m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo ""
echo -e "${YELLOW}================================================${NC}"
echo -e "${YELLOW}  CAMPUS PATHFINDER - Setup Script${NC}"
echo -e "${YELLOW}  Amrita Vishwa Vidyapeetham, Nagercoil${NC}"
echo -e "${YELLOW}================================================${NC}"
echo ""

# Step 1: Check & install gcc
echo -e "${CYAN}[1/3] Checking gcc...${NC}"
if ! command -v gcc &> /dev/null; then
    echo "gcc not found. Installing..."
    sudo apt update -qq && sudo apt install gcc -y -qq
    echo -e "${GREEN}gcc installed.${NC}"
else
    echo -e "${GREEN}gcc found: $(gcc --version | head -1)${NC}"
fi

# Step 2: Compile
echo ""
echo -e "${CYAN}[2/3] Compiling server.c...${NC}"
gcc -Wall -o server server.c
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed. Check server.c${NC}"
    exit 1
fi
echo -e "${GREEN}Compiled successfully -> ./server${NC}"

# Step 3: Run
echo ""
echo -e "${CYAN}[3/3] Starting C backend server...${NC}"
echo ""
echo -e "${YELLOW}================================================${NC}"
echo -e "${GREEN}  Server running at http://localhost:8080${NC}"
echo -e "${YELLOW}  Open index.html in your Windows browser${NC}"
echo -e "${YELLOW}  Press Ctrl+C to stop the server${NC}"
echo -e "${YELLOW}================================================${NC}"
echo ""
./server
