#ifndef PREFIXTREE_HPP
#define PREFIXTREE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Node {
private:
    int frequency = 0;
    char val = '\0';
    char prefixVal = '\0';
    Node* left = nullptr;
    Node* right = nullptr;

public:
    Node();
    Node(char val);
    Node(char val, char prefix, int frequency);
    bool isLeaf() const;
    bool setVal(char c);
    char getVal() const;
    bool setFrequency(int k);
    int getFrequency() const;
    bool setPrefix(char val);
    char getPrefix() const;
    Node* getLeftNode() const;
    Node* getRightNode() const;
    int setLeftNode(Node* n);
    int setRightNode(Node* n);
    bool operator<(const Node& other) const;
};

class PrefixTree {
private:
    Node* root = nullptr;
    std::vector<std::unique_ptr<Node>> ownedNodes;

public:
    PrefixTree();
    PrefixTree(std::vector<Node> n);
    PrefixTree(const PrefixTree&) = delete;
    PrefixTree& operator=(const PrefixTree&) = delete;
    bool insert(std::vector<Node> n);
    std::string getPrefixCode(char c);
    char getChar(std::string prefix);
};

#endif // PREFIXTREE_HPP