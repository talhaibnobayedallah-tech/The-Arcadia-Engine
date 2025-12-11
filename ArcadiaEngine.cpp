// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
    static const int TABLE_SIZE = 1007;

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
        int primary = 997;
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
        int ndx = index1;
        while (table[ndx].used && ndx < TABLE_SIZE) {  // If a collision occurred.
            ndx = (index1 + i * index2) % TABLE_SIZE;
            i++;
        }

        if (ndx >= TABLE_SIZE) {
            cout << "ID: " << playerID << " can't be inserted" << endl;
        } else {
            table[ndx].ID = playerID;
            table[ndx].name = name;
            table[ndx].used = true;
        }
    }

    string search(int playerID) override {
        int index1 = h1(playerID);
        int index2 = h2(playerID);

        int i = 1;
        int ndx = index1;
        while (table[ndx].used && ndx < TABLE_SIZE) {  // If it found element at this index
            if (playerID == table[ndx].ID) {
                return table[ndx].name;
            }

            ndx = (index1 + i * index2) % TABLE_SIZE;
            i++;
        }

        // Return "" if player not found
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        return {};
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
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
        if (root == NIL || root->id == id) // base case
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
        Node* z = new Node(itemID, price);
        z->left = z->right = z->parent = NIL;

        Node* parent = nullptr;
        Node* current = root;
        while (current != NIL) {
            parent = current;
            if (z->price < current->price || (z->price == current->price && z->id < current->id)) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        z->parent = parent;
        if (parent != nullptr) {
            root = z;
        } else if (z->price < parent->price || (z->price == parent->price && z->id < parent->id)) {
            parent->left = z;
        } else {
            parent->right = z;
        }

        fixInsert(z);
    }

    void deleteItem(int itemID) override {
        Node* z = searchItem(root, itemID);

        if (z == nullptr) {
            cout << "ID not found in the tree !" << endl;
            return;
        }

        Node* x;
        Node* y = z;
        char yOriginalColor = z->color;
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
    // edges are bidirectional
    return false;
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
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
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
