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
    int total = 0; // liczba element�w listy
    int count = 0; // liczba element�w kolejki

    std::string getActual() {
        if (total > 0) {
            return std::to_string(actual->value);
        }
        return "NULL";
    }

    void goToNext() { // przesuwa wska�nik actual (i previous) na kolejny element
        if (total > 0) {
            Node* next;
            next = reinterpret_cast<Node*>(uintptr_t(actual->xorAddress) ^ uintptr_t(previous)); //znalezienie nast�pnego w�z�a
            previous = actual;
            actual = next;  
        }
    }

    void goToPrevious() { // przesuwa wska�nik actual (i previous) na poprzedni element
        if (total > 0) {
            Node* before;
            before = reinterpret_cast<Node*>(uintptr_t(previous->xorAddress) ^ uintptr_t(actual)); // znalezienie poprzedzaj�cego w�z�a
            actual = previous;
            previous = before;
        }
    }

    void addElement(int n, const std::string place) { // dodaje element w wybranym miejscu
        Node* node = new Node;
        node->value = n;
        if (total == 0) { // je�li to pierwszy w�ze� -> ustaw jako pierwszy, ostatni, aktualny i poprzedni
            head = node;
            tail = node;
            actual = node;
            previous = node;
        }
        if (actual == head && (place == "beg" || place == "end") ) { // je�li actual wskazuje na pierwszy el. - ustaw nowy w�ze� jako poprzedni
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

    void check(Node* node) { // funkcja sprawdza czy usuwany pierwszy lub ostatni element listy nie jest jednocze�nie wska�nikiem na aktualny lub poprzedni element
        if (node == actual) { //je�li jest - przesuwa wska�nik na poprzedzaj�cy element
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
        if (nodeToCheck == front) { // tylko je�li "delete" -> wypchnij element
            isInQueue = true;
            pop();
        }
        else if (nodeToCheck == back) { // tylko je�li "delete" -> przesu� wska�niki
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
                else if (actual == back) { // element poza kolejk�
                    isInQueue = false;
                    break;
                }
            }
        }
        if (action == "add" && isInQueue) { // je�li dodany element bezpo�rednio poprzedza front, przesu� wska�nik beforeFront
            if (reinterpret_cast<Node*>(nodeToCheck->xorAddress ^ uintptr_t(prev)) == front) beforeFront = nodeToCheck;
        }
        else if (action == "add") { // je�li dodany element bezpo�rednio poprzedza back, przesu� wska�nik beforeBack
            if (reinterpret_cast<Node*>(nodeToCheck->xorAddress ^ uintptr_t(prev)) == back) beforeBack = nodeToCheck;
        }
        if (isInQueue) { // je�li jest w kolejce i element jest usuwany -> zmniejsz liczbe el. kolejki. W przeciwnym razie zwi�ksz
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
                if (tail == actual) tail = previous; // przepisanie wska�nika, je�li actual by� jednocze�nie pierwszym lub ostatnim w�z�em
                if (head == actual) head = next; 
                delete actual;
                actual = previous;
                previous = replacement;
                if (total == 2) previous = actual; // je�li przedostatni el. jest usuwany - ostatni musi by� jednocze�nie actual i previous
            }
        }
        if (total == 1) { // je�li usuwany element jest ostatnim ustaw wszystkie wska�niki na NULL
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
            if (count == total) { // je�li wszystkie elementy listy s� elementami kolejki
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
            if (!count) { // je�li to by� ostatni element kolejki
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
            Node* tmpAct = actual; // przepisanie tymczasowe wska�nik�w, aby zacz�� wypisywanie od pocz�tku listy
            Node* tmpPrev = previous;
            actual = tail;
            previous = reinterpret_cast<Node*>(uintptr_t(tail->xorAddress) ^ uintptr_t(head));
            for (int i = 0; i < total; i++) { // wypisywanie nast�pnych w�z��w (pocz�wszy od ostatniego, aby pierwszym wypisanym by� pierwszy w�ze�)
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
            Node* tmpAct = actual; // przepisanie tymczasowe wska�nik�w, aby zacz�� wypisywanie od ko�ca listy
            Node* tmpPrev = previous;
            actual = head;
            previous = tail;
            for (int i = 0; i < total; i++) { // wypisywanie poprzednich w�z��w (pocz�wszy od pierwszego, aby pierwszym wypisanym by� ostatni w�ze�)
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