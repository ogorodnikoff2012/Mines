#ifndef __XENON_MINES_AHO_CORASICK_H__
#define __XENON_MINES_AHO_CORASICK_H__

#include <map>
#include <vector>

struct Trie {
    char from;
    int terminal;
    Trie* parent;
    Trie* suflink;
    Trie* superlink;
    std::map<char, Trie*> children;
    Trie()
        : from(0), terminal(0), parent(nullptr), suflink(nullptr),
          superlink(nullptr) {}
};

int insert(Trie* t, const char* s);
Trie* go(Trie* t, char ch);
std::vector<int> terminals(Trie* t);

#endif //__XENON_MINES_AHO_CORASICK_H__
