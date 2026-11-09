#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "prefixTree.hpp"

/* Node functions */
Node :: Node() {}

Node :: Node(char val) {
    this->val = val;
}

Node :: Node(char val, char prefix, int frequency) {
    this->val = val;
    this->prefixVal = prefix;
    this->frequency = frequency;
}

bool Node :: isLeaf() {
    return this->val != '\0';
}

bool Node :: setVal(char c) {
    this->val = c;
    return true;
}

char Node :: getVal() {
    return this->val;
}

bool Node :: setFrequency(int k) {
    this->frequency = k;
    return true;
}

int Node :: getFrequency() {
    return this->frequency;
}

bool Node :: setPrefix(char val) {
    if(val != '0' && val != '1') return false;
    this->prefixVal = val;
    return true;
}

char Node :: getPrefix() {
    return this->prefixVal;
}

Node* Node :: getLeftNode() {
    return this->left;
}

Node* Node :: getRightNode() {
    return this->right;
}

int Node :: setLeftNode(Node* n) {
    if(n == nullptr) return -1;
    this->left = n;
    return 0;
}

int Node :: setRightNode(Node* n) {
    if(n == nullptr) return -1;
    this->right = n;
    return 0;
}

/* Prefix Tree Implementation */





