#include <iostream>

struct BNode {
    bool isLeaf;
    int numOfKeys=0;
    int* keys;
    int order;
    BNode** sons;
    BNode() {}
    BNode(int order);
    void split(BNode* node);
};

BNode::BNode(int ord) {
    keys = new int[2 * ord - 1];
    for (int i = 0; i < 2 * ord - 1; i++) {
        keys[i] = NULL;
    }
    sons = new BNode*[2 * ord];
    for (int i = 0; i < 2 * ord; i++) {
        sons[i] = nullptr;
    }
    numOfKeys = 0;
    isLeaf = false;
    this->order = ord;
}

struct BTree {
    int order;
    
    BNode* root = nullptr;
    void setOrder(int i);
    void add(int i);
    void find(int i);
    void print();
    void goDeeper(BNode* node, int num);
    void printDeeper(BNode* node);
    void insert(BNode* node, int num);
    BNode* splitNode(BNode* p, int i, BNode* c);
};

void BTree::goDeeper(BNode* node, int num) {
    int index = 0;
    for (int i = 0; i < node->numOfKeys; i++) {
        if (num > node->keys[node->numOfKeys - 1]) {
            index = node->numOfKeys;
            break;
        }
        if (node->keys[i] == num) {
            std::cout << num << " +\n";
            return;
        }
        else if (node->keys[i] > num) {
            index = i;
            break;
        }
    }
    if (node->isLeaf) {
        std::cout << num << " -\n";
        return;
    }
    else {
        node = node->sons[index];
        goDeeper(node, num);
    }
}

void BTree::find(int num) {
    if (root == nullptr) {
        std::cout << num << " -\n";
        return;
    }
    goDeeper(root, num);
}

void BTree::print() {
    if (root != nullptr) printDeeper(root);
    std::cout << std::endl;
}

void BTree::printDeeper(BNode* node) {
    for (int i = 0; i <= node->numOfKeys; i++) {
        if (!node->isLeaf && node->sons[i] != nullptr) { // jeœli nie jest liœciem, czyli ma synów
            printDeeper(node->sons[i]);
        }
        if (i < node->numOfKeys) std::cout << node->keys[i] << " ";     
    }
}

void BTree::setOrder(int i) {
    order = i;
}

void BTree::add(int num) {
    if (root != nullptr) insert(root, num);
    else {
        root = new BNode(order);
        root->isLeaf = true;
        root->keys[0] = num;
        root->numOfKeys++;
    }
}

void BNode::split(BNode* child) {
    BNode* newNode = new BNode(order);
    newNode->isLeaf = child->isLeaf;
    newNode->numOfKeys = order - 1;
    child->numOfKeys = order - 1;

    for (int i = 0; i < order - 1; i++) {
        newNode->keys[i] = child->keys[order + i];
    }
    if (!child->isLeaf) {
        for (int i = 0; i < order; i++) {
            newNode->sons[i] = child->sons[order + i];
        }
    }
    int i;
    for (i = this->numOfKeys - 1; child->keys[order - 1] < this->keys[i] && i >= 0; i--) {
        this->keys[i + 1] = this->keys[i];
        this->sons[i + 2] = this->sons[i+1];
    }
    this->sons[i + 2] = newNode;
    this->keys[i + 1] = child->keys[order - 1];
    this->numOfKeys++;
}


void BTree::insert(BNode* node, int num) {
    if (node->numOfKeys == 2 * order - 1 && node == root) { // dla roota
        BNode* newRoot = new BNode(order);
        newRoot->sons[0] = root;
        newRoot->split(root);
        root = newRoot;
        insert(root, num);
    }
    else if (!node->isLeaf) {
        int i;
        for (i = node->numOfKeys - 1; i >= 0 && num < node->keys[i]; i--) {

        }
        if (node->sons[i+1]->numOfKeys == 2 * order - 1) {
            node->split(node->sons[i+1]);
            if (num > node->keys[i+1]) i++;
        }
        insert(node->sons[i+1], num);
    }
    else {
        int* newKeys = new int[2 * order - 1];
        int pos = 0;
        for (int i = 0; i < node->numOfKeys; i++) {
            if (node->keys[i] > num) {
                pos++;
                newKeys[i+1] = node->keys[i];
            }
            else newKeys[i] = node->keys[i];
        }
        newKeys[node->numOfKeys - pos] = num;
        node->numOfKeys++;
        node->keys = newKeys;
    }
}

int main()
{
    BTree tree;
    int num;
    char c;
    std::cin >> c;
    while (c != 'X') {
        switch (c) {
        case 'I': 
            std::cin >> num; 
            tree.setOrder(num);
            break;
        case 'A': 
            std::cin >> num;
            tree.add(num);
            break;
        case '?':
            std::cin >> num;
            tree.find(num);
            break;
        case 'P':
            tree.print();
            break;
        std::cin >> c;
    }
}

