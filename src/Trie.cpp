#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>
#include <utility>
#include "Trie.hpp"

/* TrieNode functions */
TrieNode :: TrieNode(char c, bool val, int c1, int c2) {
    this->symbol = c;
    this->isWordEnd = val;
    this->prefixCount = c1;
    this->wordCount = c2;
}

TrieNode :: ~TrieNode() {
    for(auto it : this->children) delete it.second;
}

char TrieNode :: getSymbol() {
    return this->symbol;
}

bool TrieNode :: incrementPrefixCount() {
    this->prefixCount += 1;
    return true;
}

bool TrieNode :: decrementPrefixCount() {
    this->prefixCount -= 1;
    return true;
}

int TrieNode :: getPrefixCount() {
    return this->prefixCount;
}

bool TrieNode :: incrementWordCount() {
    this->wordCount += 1;
    return true;
}

bool TrieNode :: decrementWordCount() {
    this->wordCount -= 1;
    return true;
}

int TrieNode :: getWordCount() {
    return this->wordCount;
}

bool TrieNode :: isEndOfWord() {
    return this->isWordEnd;
}

int TrieNode :: toggleEndOfWord() {
    this->isWordEnd = !this->isWordEnd;
    return TRIE_STATUS_SUCCESS;
}

int TrieNode :: insertChild(TrieNode* child) {
   try {
        char sym = child->getSymbol();
        this->children[sym] = child;
        return TRIE_STATUS_SUCCESS;
   } catch (std::runtime_error &e) {
        std::cout << "Unable to insert: " << "Error: " << e.what() << std::endl;
        return TRIE_STATUS_FAILURE;
   }
}

std::vector<char> TrieNode :: getChildren() {
    std::vector<char> childArr;
    for(auto it : this->children) childArr.push_back(it.first);
    return childArr;
}

TrieNode* TrieNode :: getChildNode(char c) {
    if(this->children.find(c) != this->children.end()) {
        return this->children[c];
    }
    return nullptr;
}

int TrieNode :: insertExec(std::string exec) {
    this->executables.push_back(exec);
    return TRIE_STATUS_SUCCESS;
}

std::vector<std::string> TrieNode :: getExecs() {
    return this->executables;
}



/* Trie Functions */
Trie :: Trie() {
    TrieNode* node = new TrieNode('*', false);
    this->head = node;
}

Trie :: ~Trie() {
    delete head;
}

bool Trie :: search(std::string word) {
    int i = 0;
    TrieNode* curr = this->head;
    while(i < word.size()) {
        curr = curr->getChildNode(word[i]);
        if(curr == nullptr) return false;
        i++;
    }
    return !word.empty() && curr->isEndOfWord();
}

std::vector<std::string> Trie :: getExec(std::string word) {
    std::vector<std::string> empty;
    int i = 0;
    TrieNode* curr = this->head;
    while(i < word.size()) {
        curr = curr->getChildNode(word[i]);
        if(curr == nullptr) return empty;
        i++;
    }
    return !word.empty() && curr->isEndOfWord() ? curr->getExecs() : empty;
}

bool Trie :: insert(std::string word, std::string exec) {
    if (word.empty()) return false;

    int i = 0;
    TrieNode* curr = this->head;
    while (i < word.size()) {
        TrieNode* next;
        if(curr->getChildNode(word[i]) == nullptr) {
            next = new TrieNode(word[i], false, 1, 0);
            curr->insertChild(next);
        } else {
            next = curr->getChildNode(word[i]);
            curr->incrementPrefixCount();
        }
        curr = next;
        i++;
    }

    if (!curr->isEndOfWord()) curr->toggleEndOfWord();
    curr->incrementWordCount();
    curr->insertExec(exec);
    return true;
}

std::string Trie :: extendPrefix(std::string prefix) {
    std::string extPrefix = prefix;
    TrieNode* curr = this->head;
    int i = 0;
    while(i < prefix.size()) {
        TrieNode* next = curr->getChildNode(prefix[i]);
        if (next == nullptr) return extPrefix;
        curr = next;
        i++;
    }
    int origPrefix = curr->getPrefixCount();
    while(curr->getChildren().size() == 1) {
        std::vector<char> child = curr->getChildren();
        TrieNode* next = curr->getChildNode(child[0]);
        extPrefix += next->getSymbol();
    }
    return extPrefix;
}

std::vector<std::string> Trie :: allCmdsFromPrefix(std::string prefix) {
    std::vector<std::string> allCmd;
    std::stack<std::pair<TrieNode*, std::string>> s;
    TrieNode* curr = this->head;
    int i = 0;

    while(i < prefix.size()) {
        TrieNode* next = curr->getChildNode(prefix[i]);
        if (next == nullptr) return allCmd;
        curr = next;
        i++;
    }
    
    s.push({curr, prefix});
    while(!s.empty()) {
        curr = s.top().first;
        std::string command = s.top().second;

        s.pop();
        if (curr->isEndOfWord()) {
            allCmd.push_back(command);
        }

        for (char childSymbol : curr->getChildren()) {
            TrieNode* next = curr->getChildNode(childSymbol);
            s.push({next, command + childSymbol});
        }
    }

    return allCmd;
}

bool Trie :: erase(std::string word) {
    if (word.empty()) return false;

    std::vector<TrieNode*> path;
    TrieNode* curr = this->head;
    int i = 0;
    while (i < word.size()) {
        curr = curr->getChildNode(word[i]);
        if (curr == nullptr) return false;
        path.push_back(curr);
        i++;
    }

    if (!curr->isEndOfWord()) return false;

    curr->toggleEndOfWord();
    if (curr->getWordCount() > 0) curr->decrementWordCount();
    for (TrieNode* node : path) {
        if (node->getPrefixCount() > 0) node->decrementPrefixCount();
    }
    return true;
}