#include "aho_corasick.h"

#include <map>
#include <vector>

using std::map;
using std::vector;

int inserted_words = 0;

int insert(Trie* t, const char* s) {
    while (*s) {
        char ch = *s;
        auto it = t->children.find(ch);
        Trie* next = nullptr;
        if (it == t->children.end()) {
            next = new Trie;
            next->parent = t;
            next->from = ch;
            t->children[ch] = next;
        } else {
            next = it->second;
        }
        t = next;
        s++;
    }
    return t->terminal = ++inserted_words;
}

Trie* link(Trie* t);
Trie* go(Trie* t, char ch);

Trie* go(Trie* t, char ch) {
    auto it = t->children.find(ch);
    if (it == t->children.end()) {
        if (!t->parent) {
            return t;
        }
        Trie* ans = go(link(t), ch);
        t->children[ch] = ans;
        return ans;
    }
    return it->second;
}

Trie* link(Trie* t) {
    if (!t->parent) {
        return t;
    }
    if (!t->parent->parent) {
        return t->parent;
    }
    if (!t->suflink) {
        t->suflink = go(link(t->parent), t->from);
    }
    return t->suflink;
}

Trie* superLink(Trie* t) {
    if (!t->parent) {
        return nullptr;
    }
    if (!t->superlink) {
        if (link(t)->terminal) {
            t->superlink = link(t);
        } else {
            t->superlink = superLink(link(t));
        }
    }
    return t->superlink;
}

vector<int> terminals(Trie* t) {
    vector<int> ans;
    while (t) {
        if (t->terminal) {
            ans.push_back(t->terminal);
        }
        t = superLink(t);
    }
    return ans;
}
