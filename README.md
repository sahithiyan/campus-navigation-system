╔══════════════════════════════════════════════════════════════════╗
║   CAMPUS PATHFINDER                                              ║
║   Amrita Vishwa Vidyapeetham – Nagercoil Campus                  ║
║   C Backend + Interactive HTML/CSS/JS Frontend                   ║
╚══════════════════════════════════════════════════════════════════╝

FILES IN THIS FOLDER
────────────────────
  server.c    →  C backend HTTP server (Bellman-Ford, BFS, DFS)
  index.html  →  Interactive browser UI (auto-connects to server)
  setup.sh    →  One-click compile + run script (WSL Ubuntu)
  README.txt  →  This file

══════════════════════════════════════════════════════════════════
  HOW TO RUN (WSL Ubuntu in PowerShell)
══════════════════════════════════════════════════════════════════

STEP 1 — Open PowerShell and launch WSL:
  wsl

STEP 2 — Navigate to this folder:
  Example: cd /mnt/c/Users/YourName/Desktop/campus_pathfinder

STEP 3 — Give permission and run the setup script:
  chmod +x setup.sh
  ./setup.sh

  This will automatically:
    ✅ Check / install gcc
    ✅ Compile server.c
    ✅ Start the server on http://localhost:8080

STEP 4 — Open the frontend:
  Double-click index.html in Windows File Explorer
  It opens in your browser and auto-connects to the C server

  ✅ Green badge = "C Server Online"  (C backend running)
  ⚠️  Red badge  = "Server Offline"  (uses JS fallback, still works)

══════════════════════════════════════════════════════════════════
  MANUAL COMPILE (if you prefer not to use setup.sh)
══════════════════════════════════════════════════════════════════

  gcc -o server server.c
  ./server

══════════════════════════════════════════════════════════════════
  API ENDPOINTS (C Server at localhost:8080)
══════════════════════════════════════════════════════════════════

  GET /graph
      Returns all campus nodes and edges as JSON

  GET /route?algo=bellman&src=0&dst=9
      Bellman-Ford shortest path from node 0 to node 9

  GET /route?algo=bfs&src=0&dst=9
      BFS fewest-hops path

  GET /route?algo=dfs&src=0&dst=9
      DFS exploration path

══════════════════════════════════════════════════════════════════
  CAMPUS NODE IDs
══════════════════════════════════════════════════════════════════

   0  Main Gate              6  ECE Block
   1  Admin Block            7  Library
   2  AVVP Block             8  Canteen
   3  CSE Block              9  Girls Hostel
   4  Science & Hum. Block  10  Anugraha Bhavan (Boys Hostel)
   5  Civil Block           11  Vyasa Bhavan (Boys Hostel)

══════════════════════════════════════════════════════════════════
  ALGORITHMS
══════════════════════════════════════════════════════════════════

  Bellman-Ford  →  Weighted shortest path       O(V × E)
  BFS           →  Fewest hops (unweighted)     O(V + E)
  DFS           →  Depth-first exploration      O(V + E)

══════════════════════════════════════════════════════════════════
  PROJECT INFO
══════════════════════════════════════════════════════════════════

  Mini Project : Campus PathFinder
  College      : Amrita Vishwa Vidyapeetham, Nagercoil
  Subject      : Design and Analysis of Algorithms
  Tech Stack   : C (backend) + HTML/CSS/JavaScript (frontend)
  Platform     : WSL Ubuntu + Windows Browser
