// heappatientqueue.h

#pragma once

#include <iostream>
#include <string>
#include "patientnode.h"
#include "patientqueue.h"
#include "error.h"
#include "strlib.h"

using namespace std;

const int IINITIAL_CAPACITY = 10;

class HeapPatientQueue : public PatientQueue  {
public:
    // constructor
    HeapPatientQueue();

    // destructor
    ~HeapPatientQueue();

    // clean up the current queue
    void clear();

    // get the front patient's name
    string frontName();

    // cheack if current queue is empty
    bool isEmpty();

    // get the most urgent patient's priority
    int frontPriority();

    // add a new paitient along with the priority
    void newPatient(string name, int priority);

    // upgrade an existing patient's priority
    void upgradePatient(string name, int newPriority);

    // remove the most urgent patient
    string processPatient();

    // display the current patient queue as a string
    string toString();

private:
    PatientNode* PQueue = nullptr;  // a patient queue pionter to the most urgent patient
    int capacity;                   // total capacity of the queue
    int size;                       // current size of the queue

    // Percolate Up algorithm
    void percolateUp(int child);

    // Percolate Down algorithm
    void percolateDown(int parent);

    // double the current queue capacity
    void expandCapacity();
};
