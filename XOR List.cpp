#include <iostream>
#include <string>

struct Node {
    int value;
    uintptr_t xorAddress;
};

struct List {
    Node* head;
    Node* tail;
    Node* actual;
    Node* previous;
    Node* front;
    Node* back;
    Node* beforeBack;
    Node* beforeFront;
    int total = 0; // liczba elementÛw listy
    int count = 0; // liczba elementÛw kolejki

    std::string getActual() {
        if (total > 0) {
            return std::to_string(actual->value);
        }
        return "NULL";
    }

    void goToNext() { // przesuwa wskaünik actual (i previous) na kolejny element
        if (total > 0) {
            Node* next;
            next = reinterpret_cast<Node*>(uintptr_t(actual->xorAddress) ^ uintptr_t(previous)); //znalezienie nastÍpnego wÍz≥a
            previous = actual;
            actual = next;  
        }
    }

    void goToPrevious() { // przesuwa wskaünik actual (i previous) na poprzedni element
        if (total > 0) {
            Node* before;
            before = reinterpret_cast<Node*>(uintptr_t(previous->xorAddress) ^ uintptr_t(actual)); // znalezienie poprzedzajπcego wÍz≥a
            actual = previous;
            previous = before;
        }
    }

    void addElement(int n, const std::string place) { // dodaje element w wybranym miejscu
        Node* node = new Node;
        node->value = n;
        if (total == 0) { // jeúli to pierwszy wÍze≥ -> ustaw jako pierwszy, ostatni, aktualny i poprzedni
            head = node;
            tail = node;
            actual = node;
            previous = node;
        }
        if (actual == head && (place == "beg" || place == "end") ) { // jeúli actual wskazuje na pierwszy el. - ustaw nowy wÍze≥ jako poprzedni
            previous = node;
        }
        if (place == "beg") { // ADD_BEG
            node->xorAddress = uintptr_t(tail) ^ uintptr_t(head);
            head->xorAddress = uintptr_t(node) ^ (head->xorAddress ^ uintptr_t(tail));
            tail->xorAddress = (tail->xorAddress ^ uintptr_t(head)) ^ uintptr_t(node);
            head = node;
            if (count > 1) checkIfInQueue(node, tail, "add");
        }
        else if (place == "end") { // ADD_END
            node->xorAddress = uintptr_t(tail) ^ uintptr_t(head);
            head->xorAddress = uintptr_t(node) ^ (head->xorAddress ^ uintptr_t(tail));
            tail->xorAddress = uintptr_t(node) ^ (tail->xorAddress ^ uintptr_t(head));
            tail = node;
            if (count > 1) checkIfInQueue(node, reinterpret_cast<Node*>(node->xorAddress ^ uintptr_t(head)), "add");
        }
        else if (place == "act") { // ADD_ACT
                node->xorAddress = uintptr_t(previous) ^ uintptr_t(actual);
                previous->xorAddress = (previous->xorAddress ^ uintptr_t(actual)) ^ uintptr_t(node);
                actual->xorAddress = (actual->xorAddress ^ uintptr_t(previous)) ^ uintptr_t(node);
                previous = node;
                if (actual == head) head = node;
        }
        if (node->xorAddress == (uintptr_t(front) ^ uintptr_t(back))) beforeBack = node;
        total++;
    }

    void check(Node* node) { // funkcja sprawdza czy usuwany pierwszy lub ostatni element listy nie jest jednoczeúnie wskaünikiem na aktualny lub poprzedni element
        if (node == actual) { //jeúli jest - przesuwa wskaünik na poprzedzajπcy element
            Node* tmpPrev = previous;
            previous = reinterpret_cast<Node*>(previous->xorAddress ^ uintptr_t(actual));
            actual = tmpPrev;
        }
        else if (node == previous) {
            previous = reinterpret_cast<Node*>(previous->xorAddress ^ uintptr_t(actual));
        }
    }
    void checkIfInQueue(Node* nodeToCheck, Node* prev, const std::string action) {        
        bool isInQueue = false;
        if (nodeToCheck == front) { // tylko jeúli "delete" -> wypchnij element
            isInQueue = true;
            pop();
        }
        else if (nodeToCheck == back) { // tylko jeúli "delete" -> przesuÒ wskaüniki
            isInQueue = true;
            Node* newBack = reinterpret_cast<Node*>(back->xorAddress ^ uintptr_t(beforeBack));
            back = newBack;
        }
        else if (nodeToCheck == beforeBack) {
            beforeBack = reinterpret_cast<Node*>(beforeBack->xorAddress ^ uintptr_t(back));
        }
        else if (nodeToCheck == beforeFront) {
            beforeFront = reinterpret_cast<Node*>(beforeFront->xorAddress ^ uintptr_t(front));
            isInQueue = true;
        }
        
        if (!isInQueue) {
            actual = nodeToCheck;
            previous = prev;
            while (true) {
                goToNext();
                if (actual == front) { // element w kolejce
                    isInQueue = true;
                    break;
                }
                else if (actual == back) { // element poza kolejkπ
                    isInQueue = false;
                    break;
                }
            }
        }
        if (action == "add" && isInQueue) { // jeúli dodany element bezpoúrednio poprzedza front, przesuÒ wskaünik beforeFront
            if (reinterpret_cast<Node*>(nodeToCheck->xorAddress ^ uintptr_t(prev)) == front) beforeFront = nodeToCheck;
        }
        else if (action == "add") { // jeúli dodany element bezpoúrednio poprzedza back, przesuÒ wskaünik beforeBack
            if (reinterpret_cast<Node*>(nodeToCheck->xorAddress ^ uintptr_t(prev)) == back) beforeBack = nodeToCheck;
        }
        if (isInQueue) { // jeúli jest w kolejce i element jest usuwany -> zmniejsz liczbe el. kolejki. W przeciwnym razie zwiÍksz
            if (action == "delete") count--;
            else if (action == "add") count++;
        }
    }

    void removeElement(const std::string place) { // usuwa element w wybranym miejscu
        Node* replacement;
        if (total > 1) {
            if (place == "beg") { //DEL_BEG
                replacement = reinterpret_cast<Node*>(head->xorAddress ^ uintptr_t(tail)); // replacement to obecnie drugi element listy
                if (count) checkIfInQueue(head, tail, "delete");
                check(head);                
                replacement->xorAddress = uintptr_t(tail) ^ (replacement->xorAddress ^ uintptr_t(head));
                tail->xorAddress = (tail->xorAddress ^ uintptr_t(head)) ^ uintptr_t(replacement);
                delete head;
                head = replacement;
            }
            else if (place == "end") { //DEL_END
                replacement = reinterpret_cast<Node*>(tail->xorAddress ^ uintptr_t(head));
                if (count) checkIfInQueue(tail, replacement, "delete");
                check(tail);
                replacement->xorAddress = uintptr_t(head) ^ (replacement->xorAddress ^ uintptr_t(tail));
                head->xorAddress = (head->xorAddress ^ uintptr_t(tail)) ^ uintptr_t(replacement);
                delete tail;
                tail = replacement;
            }
            else if (place == "act") { //DEL_ACT
                replacement = reinterpret_cast<Node*>(previous->xorAddress ^ uintptr_t(actual));
                previous->xorAddress = (actual->xorAddress ^ uintptr_t(previous)) ^ uintptr_t(replacement);
                Node* next = reinterpret_cast<Node*>(actual->xorAddress ^ uintptr_t(previous));
                next->xorAddress = (next->xorAddress ^ uintptr_t(actual)) ^ uintptr_t(previous);
                if (tail == actual) tail = previous; // przepisanie wskaünika, jeúli actual by≥ jednoczeúnie pierwszym lub ostatnim wÍz≥em
                if (head == actual) head = next; 
                delete actual;
                actual = previous;
                previous = replacement;
                if (total == 2) previous = actual; // jeúli przedostatni el. jest usuwany - ostatni musi byÊ jednoczeúnie actual i previous
            }
        }
        if (total == 1) { // jeúli usuwany element jest ostatnim ustaw wszystkie wskaüniki na NULL
            delete actual;
            head = nullptr;
            tail = nullptr;
            actual = nullptr;
            previous = nullptr;
        }
        if (total >= 1) total--;
    }
    void push(int value) {
        if (!count) { // pierwszy element kolejki
            if (!total) { // pusta lista
                addElement(value, "act"); 
            }         
            front = actual;
            back = actual;
            beforeBack = previous;
            beforeFront = previous;
            actual->value = value;          
        }
        else {
            if (count == total) { // jeúli wszystkie elementy listy sπ elementami kolejki
                previous = beforeBack;
                actual = back;
                addElement(value, "act");
                back = previous;
                beforeBack = front;
                if (count == 1) {
                    beforeFront = back;
                }
            }
            else if (count < total){
                if (count == 1) {
                    while (actual != front) {
                        goToNext();
                    }
                    beforeFront = previous;
                    beforeBack = previous;
                }
                beforeBack->value = value;
                Node* newBefore = reinterpret_cast<Node*>(beforeBack->xorAddress ^ uintptr_t(back)); // znalezienie poprzednika nowego back
                back = beforeBack;
                beforeBack = newBefore;
            }            
        }
        count++;
    }
    void pop() {
        if (count) {
            Node* newBefore = reinterpret_cast<Node*>(beforeFront->xorAddress ^ uintptr_t(front));
            std::cout << front->value << std::endl;
            front = beforeFront;
            beforeFront = newBefore;
            count--;
            if (!count) { // jeúli to by≥ ostatni element kolejki
                front = nullptr;
                back = nullptr;
                beforeBack = nullptr;
                beforeFront = nullptr;
            }
        }
        else std::cout << "NULL" << std::endl;
    }
    void getListSize() const{
        std::cout << total << std::endl;
    }
    void getQueueSize() const{
        std::cout << count << std::endl;
    }
    void garbageSoft() {
        if (count) {
            actual = reinterpret_cast<Node*>(front->xorAddress ^ uintptr_t(beforeFront));
            previous = front;
            while (actual != back) {
                actual->value = 0;
                goToNext();               
            }
        }
        else {
            if (total) {
                for (int i = 0; i <= total; i++) {
                    actual->value = 0;
                    goToNext();
                }
            }
            else std::cout << "NULL\n";
        }
    }
    void garbageHard() {
        if (count) {
            actual = reinterpret_cast<Node*>(front->xorAddress ^ uintptr_t(beforeFront));
            previous = front;
            while (actual != back) {
                removeElement("act");
                goToNext();
            }
            beforeBack = front;
        }
        else {
            if (total) {
                for (int i = 0; i < total; i++) {
                    goToNext();
                    removeElement("act");
                }
            }
            else std::cout << "NULL\n";
        }
    }

    void printQueue() {
        if (count) {
            Node* tmpAct = actual;
            Node* tmpPrev = previous;
            actual = front;
            previous = beforeFront; 
            for (int i = 0; i < count; i++) {
                std::cout << getActual() << " ";
                goToPrevious();
            }
            actual = tmpAct;
            previous = tmpPrev;
        }
        else {
            std::cout << "NULL";
        }
        std::cout << std::endl;
    }

    void printForward() { 
        if (total > 0) {
            Node* tmpAct = actual; // przepisanie tymczasowe wskaünikÛw, aby zaczπÊ wypisywanie od poczπtku listy
            Node* tmpPrev = previous;
            actual = tail;
            previous = reinterpret_cast<Node*>(uintptr_t(tail->xorAddress) ^ uintptr_t(head));
            for (int i = 0; i < total; i++) { // wypisywanie nastÍpnych wÍz≥Ûw (poczπwszy od ostatniego, aby pierwszym wypisanym by≥ pierwszy wÍze≥)
                goToNext();
                std::cout << getActual() << " ";
            }
            std::cout << std::endl;
            actual = tmpAct;
            previous = tmpPrev;
        }
        else std::cout << "NULL\n";
    }

    void printBackward() {
        if (total > 0) {
            Node* tmpAct = actual; // przepisanie tymczasowe wskaünikÛw, aby zaczπÊ wypisywanie od koÒca listy
            Node* tmpPrev = previous;
            actual = head;
            previous = tail;
            for (int i = 0; i < total; i++) { // wypisywanie poprzednich wÍz≥Ûw (poczπwszy od pierwszego, aby pierwszym wypisanym by≥ ostatni wÍze≥)
                goToPrevious();
                std::cout << getActual() << " ";
            }
            std::cout << std::endl;
            actual = tmpAct;
            previous = tmpPrev;
        }
        else std::cout << "NULL\n";
    }
};

int main()
{
    List* list = new List;
    std::string command;
    int number;

    while (std::cin >> command) {

        if (command == "ADD_BEG") {
            std::cin >> number;
            list->addElement(number, "beg");
        }
        else if (command == "ADD_END") {
            std::cin >> number;
            list->addElement(number, "end");
        }
        else if (command == "SIZE") {
            list->getListSize();
        }
        else if (command == "COUNT") {
            list->getQueueSize();
        }
        else if (command == "PUSH") {
            //std::cin >> number;
            scanf("%d", &number);
            list->push(number);
        }
        else if (command == "POP") {
            list->pop();
        }
        else if (command == "DEL_BEG") {
            list->removeElement("beg");
        }
        else if (command == "DEL_END") {
            list->removeElement("end");
        }
        else if (command == "PRINT_QUEUE") {
            list->printQueue();
        }
        else if (command == "GARBAGE_SOFT") {
            list->garbageSoft();
        }
        else if (command == "GARBAGE_HARD") {
            list->garbageHard();
        }
        else if (command == "PRINT_FORWARD") {
            list->printForward();
        }
        else if (command == "PRINT_BACKWARD") {
            list->printBackward();
        }
    }

    delete list;
    return 0;
}