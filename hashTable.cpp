#include <iostream>
#include <string>
#include <math.h>

struct Node {
    std::string stringA, stringB;
    int occurance;
    Node* next = nullptr;

    Node() {
        occurance = NULL;
    }
    Node(const std::string a, const std::string b, int i) {
        stringA = a;
        stringB = b;
        occurance = i;
    }
    void add(int i) {
        occurance += i;
    }
    bool ifEqual(const std::string a, const std::string b) {
        if (a == stringA && b == stringB) {
            return true;
        }
        return false;
    }
};

struct HashTable {
    int tableSize = 101;
    Node values [101];

    HashTable() {}

    int hash(const std::string a, const std::string b);
    void addEl(const std::string a, const std::string b, int num);
    void print(const std::string a, const std::string b);
};

int HashTable::hash(const std::string a, const std::string b) {
    int base = 'z' - 'a' + 1;
    int digitPos = 0, k = 0;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] < 'z' && a[i] > 'a') {
            int digit = a[i] - 'a';
            k = k + digit * pow(base, digitPos);
            digitPos = (digitPos + 1) % a.size();
        }
    }
    return (k + b[0]) % tableSize;
}

void HashTable::addEl(const std::string a, const std::string b, int num) {
    Node* node = &values[hash(a, b)];
    if (node->occurance == NULL) { // nie ma elementów o tym indeksie
        values[hash(a, b)].stringA = a;
        values[hash(a, b)].stringB = b;
        values[hash(a, b)].occurance = num;
    }
    else {
        if (node->ifEqual(a, b)) {
            node->add(num);
        }
        else {
            while (node->next != nullptr) {
                node = node->next;
                if (node->ifEqual(a, b)) {
                    node->add(num);
                    return;
                }
            }
            Node* newNode = new Node(a, b, num);
            node->next = newNode;
        }
    }
}

void HashTable::print(const std::string a, const std::string b) {
    Node* node = &values[hash(a, b)];
    if (node->ifEqual(a, b)) {
        std::cout << node->occurance << std::endl;
    }
    else {
        while (node->next != nullptr) {
            node = node->next;
            if (node->ifEqual(a, b)) {
                std::cout << node->occurance << std::endl;
                return;
            }
        }
        std::cout << 0 << std::endl;
    }
}

int main()
{
    char c;
    int number;
    std::string stringA, stringB;
    HashTable hashTable;

    while (std::cin >> c) {       
        if (c == '+') {
            std::cin >> stringA >> stringB >> number;
            hashTable.addEl(stringA, stringB, number);
        }
        else if (c == '?') {
            std::cin >> stringA >> stringB;
            hashTable.print(stringA, stringB);
        }          
    }
}