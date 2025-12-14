# The-Arcadia-Engine
Your team has been hired by Arcadia Games to build the backend engine for their new MMORPG (Massively Multiplayer Online Role-Playing Game). The game requires a highperformance system to handle player data, inventory management, world navigation, and server task scheduling.

# Tasks
- PART A: REGISTORY (Data Structure)
  1. Class PlayerTable (Double Hashing) --> **done**
  
  2. Class Leaderboard (Skip List)
     - Constructor --> **not done**
     - removePlayer --> **not done**
     - getTopN --> **not done**

  3. Class AuctionTree (Red-Black Tree) --> **done**

- PART B: INVENTORY SYSTEM (Dynamic Programming)
  1. optimizeLootSplit --> **not done**
  
  2. maximizeCarryValue --> **not done**

  3. countStringPossibilities --> **not done**

- PART C: WORLD NAVIGATOR (Graphs)
  1. pathExists --> **not done**
 
  2. minBribeCost --> **not done**
 
  3. sumMinDistancesBinary --> **not done**
 
- PART D: SERVER KERNEL (Greedy)
  1. minIntervals --> **not done**


# Notes

- time complexities:
  - Hash table O(1) avg, Skip list O(log n), RB tree O(log n)
  - Knapsack O(n×W), Parition O(n×sum), String O(n)
  - Path O(V+E), MST O(E log V), All-Pairs Shortest Path - APSP O(V³)
  - Scheduler O(n log n)
    
- Do not modify ArcadiaEngine.h under any circumstances.
- You may add private helper functions or struct in ArcadiaEngine.cpp.
- All team members must understand all parts of the project.
- in getTopN, Return tied players in any order, but ensure all top-scoring players appear before lowerscoring ones.

# Contributors
BadrEldeen - [Badreldeen2006](https://github.com/Badreldeen2006)

Samir Sherif - [SamirSheriff](https://github.com/SamirSheriff)

Ahmed Nasser - [AhmedNAF](https://github.com/AhmedNAF)

Mostafa Ahmed - [Kali8877](https://github.com/Kali8877)

Ziad Muhammad - [biggtr](https://github.com/biggtr)
