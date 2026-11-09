#include <string>
#include <utility>
#include <vector>
#include <algorithm>
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

bool Node :: isLeaf() const {
    return this->val != '\0';
}

bool Node :: setVal(char c) {
    this->val = c;
    return true;
}

char Node :: getVal() const {
    return this->val;
}

bool Node :: setFrequency(int k) {
    this->frequency = k;
    return true;
}

int Node :: getFrequency() const {
    return this->frequency;
}

bool Node :: setPrefix(char val) {
    if(val != '0' && val != '1') return false;
    this->prefixVal = val;
    return true;
}

char Node :: getPrefix() const {
    return this->prefixVal;
}

Node* Node :: getLeftNode() const {
    return this->left;
}

Node* Node :: getRightNode() const {
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

bool Node :: operator<(const Node& other) const {
    return this->frequency < other.frequency;
}

/* Prefix Tree Implementation */
PrefixTree:: PrefixTree() = default;

PrefixTree:: PrefixTree(std::vector<Node> n) {
    insert(n);
}

bool PrefixTree :: insert(std::vector<Node> n) {
    if(n.empty()) return false;

    ownedNodes.clear();
    root = nullptr;
    ownedNodes.reserve(n.size() * 2 - 1);

    std::vector<Node*> nodes;
    nodes.reserve(n.size());
    for(const Node& leaf : n) {
        ownedNodes.push_back(std::unique_ptr<Node>(
            new Node(leaf.getVal(), '\0', leaf.getFrequency())));
        nodes.push_back(ownedNodes.back().get());
    }

    while(nodes.size() > 1) {
        std::stable_sort(nodes.begin(), nodes.end(),
            [](const Node* left, const Node* right) {
                return *left < *right;
            });

        Node* left = nodes[0];
        Node* right = nodes[1];
        left->setPrefix('0');
        right->setPrefix('1');

        std::unique_ptr<Node> parent(new Node());
        parent->setLeftNode(left);
        parent->setRightNode(right);
        parent->setFrequency(left->getFrequency() + right->getFrequency());

        ownedNodes.push_back(std::move(parent));
        nodes.erase(nodes.begin(), nodes.begin() + 2);
        nodes.push_back(ownedNodes.back().get());
    }

    root = nodes[0];
    return true;
}

std::string PrefixTree :: getPrefixCode(char c) {
    if(root == nullptr) return "";

    std::vector<std::pair<Node*, std::string>> pending;
    pending.push_back(std::make_pair(root, std::string()));
    while(!pending.empty()) {
        Node* current = pending.back().first;
        std::string code = pending.back().second;
        pending.pop_back();

        if(current->isLeaf()) {
            if(current->getVal() == c) return code.empty() ? "0" : code;
            continue;
        }

        if(current->getRightNode() != nullptr) {
            pending.push_back(std::make_pair(
                current->getRightNode(), code + '1'));
        }
        if(current->getLeftNode() != nullptr) {
            pending.push_back(std::make_pair(
                current->getLeftNode(), code + '0'));
        }
    }
    return "";
}

char PrefixTree :: getChar(std::string prefix) {
    if(root == nullptr || prefix.empty()) return '\0';
    if(root->isLeaf()) {
        return prefix == "0" ? root->getVal() : '\0';
    }

    Node* current = root;
    for(char bit : prefix) {
        if(current->isLeaf()) return '\0';
        if(bit == '0') {
            current = current->getLeftNode();
        } else if(bit == '1') {
            current = current->getRightNode();
        } else {
            return '\0';
        }
        if(current == nullptr) return '\0';
    }
    return current->isLeaf() ? current->getVal() : '\0';
}

