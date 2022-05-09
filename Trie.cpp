#include <iostream>
#include <string>

#define ALPHABET 26

struct Node {
    Node* nextLet[ALPHABET];
    std::string polish;
    Node() {
        for (int i = 0; i < ALPHABET; i++) {
            nextLet[i] = nullptr;
        }
    }
};

struct Trie {
    Node* root;
    void add(std::string a, std::string b);
    std::string read(std::string a);
    void writeAll(std::string a);
    Trie() { root = new Node; }
};

void Trie::add(std::string word, std::string translation) {
    Node* act = root;
    for (int i = 0; i < word.size(); i++) {
        int index = word[i] - 'a';
        if (act->nextLet[index] == nullptr) act->nextLet[index] = new Node;    
        act = act->nextLet[index];
    }
    act->polish = translation;
}

std::string Trie::read(std::string word) {
    Node* act = root;
    for (int i = 0; i < word.size(); i++) {
        int index = word[i] - 'a';
        if (act->nextLet[index] == nullptr) return "-";
        act = act->nextLet[index];
    }
    if (!act->polish.size()) return "-";
    return act->polish;
}

void checkDeeper(Node* act) {
    if (act == nullptr) return;
    if (act->polish.size()) std::cout << act->polish << std::endl;
    for (int i = 0; i < ALPHABET; i++) {
        if (act->nextLet[i] != nullptr) {
            checkDeeper(act->nextLet[i]);
        }
    }
}

void Trie::writeAll(std::string prefix) {
    Node* act = root;
    for (int i = 0; i < prefix.size(); i++) {
        int index = prefix[i] - 'a';
        if (act->nextLet[index] == nullptr) { 
            std::cout << "-\n"; return;
        }
        act = act->nextLet[index];
    }
    checkDeeper(act);
}

int main()
{
    Trie tree;
    char c;
    std::string word, translation;
    while (std::cin >> c) {
        switch (c) {
        case '+':
            std::cin >> word >> translation;
            tree.add(word, translation);
            break;
        case '?':
            std::cin >> word;
            std::cout << tree.read(word) << std::endl;
            break;
        case '*':
            std::cin >> word;
            tree.writeAll(word);
            break;
        }
    }
}

