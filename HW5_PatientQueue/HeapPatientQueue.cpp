// heappatientqueue.cpp

#include "HeapPatientQueue.h"

HeapPatientQueue::HeapPatientQueue() {
    clear();
}


HeapPatientQueue::~HeapPatientQueue() {
    delete [] PQueue;
    PQueue = nullptr; // advoid dangling pionter
}


void HeapPatientQueue::clear() {
    // free memory if necessary
    if (PQueue != nullptr) {
        delete [] PQueue;
    }

    // intialize a new patient queue
    capacity = IINITIAL_CAPACITY;
    PQueue = new PatientNode[capacity];
    size = 0;
}


string HeapPatientQueue::frontName() {
    return PQueue[1].name;
}


int HeapPatientQueue::frontPriority() {
    return PQueue[1].priority;
}


bool HeapPatientQueue::isEmpty() {
    return size == 0;
}

// log n
void HeapPatientQueue::newPatient(string name, int priority){
    // if non-empty queue but size is full
    if (size + 1 >= capacity) { // Note: max size + 1 == capacity
        expandCapacity();
    }

    // place new patient at the first empty index
    size++;
    PQueue[size].name = name;
    PQueue[size].priority = priority;

    // percolate up
    percolateUp(size);
}


// log n
string HeapPatientQueue::processPatient() {
    if (isEmpty()) {
        error("Processing patient in an EMPTY queue.");
    }

    string removedName = frontName();  // name for later output

    // Replace the front with last patient.
    swap(PQueue[1], PQueue[size]);
    size--;  // remove the last patient

    // Percolate down the front patient.
    percolateDown(1);
    
    return removedName;
}


// n
void HeapPatientQueue::upgradePatient(string name, int newPriority) {
    if (isEmpty()) {
        error("Upgrading patient in an EMPTY queue.");
    }

    // Search through heap to find the specified patient
    int indexToFind;  // the index of the specified patient
    for (indexToFind = 1; indexToFind <= size; indexToFind++) {
        if (PQueue[indexToFind].name == name) break;
    }

    // Now currIndex is the index of the specifed patient:
    if (indexToFind > size) {
        // if no such patient name, throw exception
        error("No such patient name in the queue.");
    } else if (newPriority >= PQueue[indexToFind].priority) {
        // if existing priority is already more urgent, throw exception
        error("The existing priority is already more urgent.");
    }

    // Upgrade the new priority
    PQueue[indexToFind].priority = newPriority;

    // Percolate up
    percolateUp(indexToFind);
}


string HeapPatientQueue::toString() {
    string output = "";
    for (int i = 1; i <= size; i++) {
        if (i != 1) output += ", ";
        output += integerToString(PQueue[i].priority) + ":" + PQueue[i].name;
    }
    return "{" + output + "}";
}


// Percolate Up algorithm
void HeapPatientQueue::percolateUp(int child) {
    // percolate up by swapping with its parent
    int parent = child / 2;
    while (child != 1 && PQueue[parent].priority > PQueue[child].priority) {
        swap(PQueue[parent], PQueue[child]); // swap child and parent
        child = parent;                      // update child index
        parent = child / 2;                  // update parent index
    }
}


// Percolate Down algorithm
void HeapPatientQueue::percolateDown(int parent) {
    int left = parent * 2;
    int right = parent * 2 + 1;
    while ( (left <= size || right <= size) &&                  // has at least one child
           (PQueue[parent].priority > PQueue[left].priority ||  // not correct order
            PQueue[parent].priority > PQueue[right].priority) ) {

        // CASE 1: has 1 child(left child).
        if (right > size) {
            if (PQueue[parent].priority >= PQueue[left].priority) {
                // if left child is urgent than or equal to parent
                swap(PQueue[parent], PQueue[left]);  // swap the left child
                parent = left;                       // update parent index
            }
            break; // Done for percolating! Now in correct order along with only one child
        }

        // CASE 2: has 2 children.
        if (PQueue[left].priority < PQueue[right].priority) {
            // left child is more urgent.
            swap(PQueue[parent], PQueue[left]);  // swap the left child
            parent = left;                       // update parent index
        } else if (PQueue[left].priority > PQueue[right].priority){
            // right child is more urgent.
            swap(PQueue[parent], PQueue[right]); // swap the right child
            parent = right;                      // update parent index
        } else {
            // left child and right child have a tie in priority.
            if (PQueue[left].name < PQueue[right].name) { // breaking ties by string comparison
                    swap(PQueue[parent], PQueue[left]);   // swap the left child
                parent = left;                       // update parent index
            } else {
                swap(PQueue[parent], PQueue[right]); // swap the right child
                parent = right;                      // update parent index
            }
        }

        left = parent * 2;      // update left child index
        right = parent * 2 + 1; // update right child index
    }
}


// Expand the capacity by doubling it.
void HeapPatientQueue::expandCapacity() {
    capacity *= 2; // update capacity first (private variable)
    PatientNode* newPQ = new PatientNode[capacity];
    for (int i = 0; i <= size; i++) {
        newPQ[i] = PQueue[i];
    }
    delete [] PQueue;
    PQueue = newPQ;
}
