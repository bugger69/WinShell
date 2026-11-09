#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Node {
private:
    char val;
    char prefixVal;
    int frequency;
    Node* left;
    Node* right;
public:
    Node();
    Node(char val);
    Node(char val, char prefix);
    bool isLeaf();
    char getVal();
    int getFrequency();
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