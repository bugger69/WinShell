#ifndef PREFIXTREE_HPP
#define PREFIXTREE_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Node {
private:
    char val = '\0';
    char prefixVal = '\0';
    int frequency;
    Node* left;
    Node* right;

public:
    Node();
    Node(char val);
    Node(char val, char prefix, int frequency);
    bool isLeaf();
    bool setVal(char c);
    char getVal();
    bool setFrequency(int k);
    int getFrequency();
    bool setPrefix(char val);
    char getPrefix();
    Node* getLeftNode();
    Node* getRightNode();
    int setLeftNode(Node* n);
    int setRightNode(Node* n);
};

class PrefixTree {
private:
    Node* root;

public:
    PrefixTree();
    PrefixTree(std::vector<Node*> n);
    bool insert(std::vector<Node*> n);
    char getPrefixCode(char c);
    char getChar(char prefix);
};

#endif // PREFIXTREE_HPP