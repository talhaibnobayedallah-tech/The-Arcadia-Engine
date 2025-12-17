// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <functional>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
    static const int TABLE_SIZE = 101;

    struct Profile {
        int ID;
        string name;
        bool used;

        Profile() {
            ID = 0;
            name = "";
            used = false;
        }
    };
    vector<Profile> table;

    // Multiplication method --> h1(k) = floor(n(kA mod 1))
    int h1(int key) {
        double A = 0.618033;
        return int(TABLE_SIZE * (key * A - floor(key * A)));
    }

    int h2(int key) {
        int primary = 97;
        return primary - (key % primary);
    }

public:
    ConcretePlayerTable() {
        table = vector<Profile>(TABLE_SIZE);
    }

    void insert(int playerID, string name) override {
        int index1 = h1(playerID);
        int index2 = h2(playerID);

        int i = 1;
        int idx = index1;
        while (table[idx].used && i <= TABLE_SIZE) {  // If a collision occurred.
            idx = (index1 + i * index2) % TABLE_SIZE;
            i++;
        }

        if (i > TABLE_SIZE) {
            cout << "Table is Full" << endl;
        } else {
            table[idx].ID = playerID;
            table[idx].name = name;
            table[idx].used = true;
        }
    }

    string search(int playerID) override {
        int index1 = h1(playerID);
        int index2 = h2(playerID);

        int i = 1;
        int idx = index1;
        while (table[idx].used && i <= TABLE_SIZE) {  // If it found element at this index
            if (playerID == table[idx].ID) {
                return table[idx].name;
            }

            idx = (index1 + i * index2) % TABLE_SIZE;
            i++;
        }

        // Return "" if player not found
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    struct Node {
        int playerID;
        int score;
        vector<Node*> forward;

        Node (int id, int sc, int level) : playerID(id), score(sc), forward(level + 1, nullptr) {}
    };

    static const int MAX_LEVEL = 8;
    Node* header;
    int levels;

    // Flip Coin
    int randomLevel() {
        int level = 0;
        while ((double)rand() / RAND_MAX < 0.5 && level < MAX_LEVEL) {
            level++;
        }
        return level;
    }

public:
    ConcreteLeaderboard() {
        header = new Node(-1, numeric_limits<int>::max(), MAX_LEVEL);
        levels = 0;
    }

    void addScore(int playerID, int score) override {
        vector<Node*> update(MAX_LEVEL + 1);
        Node* current = header;

        for (int i = levels; i >= 0; i--) {
            while (current->forward[i] && (current->forward[i]->score > score || (current->forward[i]->score == score && current->forward[i]->playerID < playerID))) {
                current = current->forward[i];
            }

            // Store the last node before where we insert at level i
            update[i] = current;
        }

        current = current->forward[0];

        // Insert only if the player does not already exist
        if (current == nullptr || current->playerID != playerID) {
            int newLevel = randomLevel();

            // If new node is taller than current list height, initialize new levels to point from header
            if (newLevel > levels) {
                for (int i = newLevel; i > levels; i--) {
                    update[i] = header;
                }
                levels = newLevel;
            }

            Node* newNode = new Node(playerID, score, newLevel);

            // Fix links
            for (int i = 0; i <= newLevel; i++) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        }
    }

    void removePlayer(int playerID) override {
        vector<Node*> update(MAX_LEVEL + 1);
        Node* current = header;

        // Find predecessors of the target node at each level
        for (int i = levels; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->playerID != playerID) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        // Target node (if it exists)
        current = current->forward[0];

        if (current) {
            // Remove node from every level it appears in
            for (int i = 0; i <= levels; i++) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            delete current;

            // Reduce levels if highest levels become empty
            while (levels > 0 and header->forward[levels] == nullptr) {
                levels--;
            }
        }
    }

    vector<int> getTopN(int n) override {
        vector<int> topN;
        Node* current = header->forward[0];

        while (current && n--) {
            topN.push_back(current->playerID);
            current = current->forward[0];
        }

        return topN;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // Attributes
    struct Node {
        int id;
        int price;
        char color;
        Node* left;
        Node* right;
        Node* parent;

        Node(int itemID, int itemPrice) : id(itemID), price(itemPrice), color('R'), left(nullptr), right(nullptr), parent(nullptr) {}
    };
    Node* NIL;
    Node* root;

    // Helpers
    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != NIL) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == NIL) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != NIL) x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == NIL) root = x;
        else if (y == y->parent->right) y->parent->right = x;
        else y->parent->left = x;
        x->right = y;
        y->parent = x;
    }

    Node* minimum(Node* node) {
        while (node->left != NIL) {
            node = node->left;
        }
        return node;
    }

    void transplant(Node* u, Node* v) {  // delete --> (replace one subtree with another)
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    Node* searchItem(Node* root, int id) {
        if (root == NIL || root->id == id)
            return root;

        Node* l = searchItem(root->left, id);
        if (l != NIL) return l;
        return searchItem(root->right, id);
    }

    void fixInsert(Node* z) {
        while (z->parent && z->parent->color == 'R') {
            if (z->parent == z->parent->parent->left) {
                Node* uncle = z->parent->parent->right;
                if (uncle->color == 'R') {
                    // case 1
                    z->parent->color = 'B';
                    uncle->color = 'B';
                    z->parent->parent->color = 'R';
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        // case 2
                        z = z->parent;
                        rotateLeft(z);
                    }
                    // case 3
                    z->parent->color = 'B';
                    z->parent->parent->color = 'R';
                    rotateRight(z->parent->parent);
                }
            } else {
                Node* uncle = z->parent->parent->left;
                if (uncle->color == 'R') {
                    // case 1
                    z->parent->color = 'B';
                    uncle->color = 'B';
                    z->parent->parent->color = 'R';
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        // case 2
                        z = z->parent;
                        rotateRight(z);
                    }
                    // case 3
                    z->parent->color = 'B';
                    z->parent->parent->color = 'R';
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = 'B';
    }

    void fixDelete(Node* x) {
        while (x != root && x->color == 'B') {
            if (x == x->parent->left) {
                Node* sibling = x->parent->right;

                // CASE: The Sibling Is Red
                if (sibling->color == 'R') {
                    sibling->color = 'B';
                    x->parent->color = 'R';
                    rotateLeft(x->parent);
                    sibling = x->parent->right;
                }

                // CASE: The sibling and both its children are black
                if (sibling->left->color == 'B' && sibling->right->color == 'B') {
                    sibling->color = 'R';
                    x = x->parent;
                } else {  // CASE: the DB's sibling is black & and DB's sibling far child is black & near child is red
                    if (sibling->right->color == 'B') {
                        sibling->left->color = 'B';
                        sibling->color = 'R';
                        rotateRight(sibling);
                        sibling = x->parent->right;
                    }

                    sibling->color = x->parent->color;
                    x->parent->color = 'B';
                    sibling->right->color = 'B';
                    rotateLeft(x->parent);
                    x = root;
                }
            } else {
                Node* sibling = x->parent->left;

                // CASE: The Sibling Is Red
                if (sibling->color == 'R') {
                    sibling->color = 'B';
                    x->parent->color = 'R';
                    rotateRight(x->parent);
                    sibling = x->parent->left;
                }

                // CASE: The sibling and both its children are black
                if (sibling->right->color == 'B' && sibling->left->color == 'B') {
                    sibling->color = 'R';
                    x = x->parent;
                } else {  // CASE: the DB's sibling is black & and DB's sibling far child is black & near child is red
                    if (sibling->left->color == 'B') {
                        sibling->right->color = 'B';
                        sibling->color = 'R';
                        rotateLeft(sibling);
                        sibling = x->parent->left;
                    }

                    sibling->color = x->parent->color;
                    x->parent->color = 'B';
                    sibling->left->color = 'B';
                    rotateRight(x->parent);
                    x = root;
                }
            }
        }
        x->color = 'B';
    }

public:
    ConcreteAuctionTree() {
        NIL = new Node(-1, 0);
        NIL->color = 'B';
        NIL->left = NIL->right = NIL->parent = NIL;
        root = NIL;
    }

    void insertItem(int itemID, int price) override {
        Node* newNode = new Node(itemID, price);
        newNode->left = newNode->right = newNode->parent = NIL;

        Node* parent = NIL;
        Node* current = root;
        while (current != NIL) {
            parent = current;
            if (newNode->price < current->price || (newNode->price == current->price && newNode->id < current->id)) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        newNode->parent = parent;
        if (parent == NIL) {
            root = newNode;
        } else if (newNode->price < parent->price || (newNode->price == parent->price && newNode->id < parent->id)) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }

        fixInsert(newNode);
    }

    void deleteItem(int itemID) override {
        Node* z = searchItem(root, itemID);

        if (z == NIL) {
            cout << "ID not found in the tree !" << endl;
            return;
        }

        Node* x;
        Node* y = z;
        char yOriginalColor = y->color;
        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);  // Get the Successor
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        if (yOriginalColor == 'B')
            fixDelete(x);
        delete z;
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2
    return 0;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    vector <int> grid[n];
    vector <int> visited(n);
    for (const auto& e:edges)
    {
        //edge is pair (1,3)
        int first_element_in_edge_in_edges=e[0], second_element_in_edge_in_edges=e[1];
        grid[first_element_in_edge_in_edges].push_back(second_element_in_edge_in_edges);
        grid[second_element_in_edge_in_edges].push_back(first_element_in_edge_in_edges);
    }

    function<bool(int)> DFS = [&](int now_node) -> bool {
            if (now_node == dest) return true;
            visited[now_node]= true;
            for ( const auto& i : grid[now_node] )
            {
                if (!visited[i] && DFS(i) )
                {
                    return true;
                }
            }
            return false;

    };
   return DFS(source);
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
     vector<int> freq(26, 0);
    
    for (char c : tasks) {
        int i = int(c) - 65; 
        freq[i]++;
    }
    
    sort(freq.rbegin(), freq.rend());
    
    int max_freq = freq[0];
    int num_of_max_freq = 0;
    
    for (int f : freq) {
        if (f == max_freq) {
            num_of_max_freq++;
        } else {
            break;
        }
    }

    int minTime = (max_freq - 1) * (n + 1) + num_of_max_freq;
    int res = max(minTime, (int)tasks.size());
    
    return res;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() { 
        return new ConcretePlayerTable(); 
    }

    Leaderboard* createLeaderboard() { 
        return new ConcreteLeaderboard(); 
    }

    AuctionTree* createAuctionTree() { 
        return new ConcreteAuctionTree(); 
    }
}
