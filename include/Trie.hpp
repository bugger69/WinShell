#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define TRIE_STATUS_SUCCESS 1
#define TRIE_STATUS_FAILURE 0

class TrieNode {
private:
    char symbol;
    bool isWordEnd;
    int prefixCount;
    int wordCount;
    std::unordered_map<char, TrieNode*> children;
    std::vector<std::string> executables;

public:
    TrieNode(char c, bool val, int c1 = 0, int c2 = 0);
    ~TrieNode();
    char getSymbol();
    bool incrementPrefixCount();
    bool decrementPrefixCount();
    int getPrefixCount();
    bool incrementWordCount();
    bool decrementWordCount();
    int getWordCount();
    bool isEndOfWord();
    int toggleEndOfWord();
    int insertChild(TrieNode* child);
    std::vector<char> getChildren();
    TrieNode* getChildNode(char c);
    int insertExec(std::string exec);
    std::vector<std::string> getExecs();
};

class Trie {
private:
    TrieNode* head;

public:
    Trie();
    ~Trie();
    bool search(std::string word);
    std::vector<std::string> getExec(std::string word);
    bool insert(std::string word, std::string exec);
    std::string extendPrefix(std::string prefix);
    std::vector<std::string> allCmdsFromPrefix(std::string prefix);
    bool erase(std::string word);
};
