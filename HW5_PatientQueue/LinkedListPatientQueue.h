// LinkedListPatientQueue.h

#pragma once

#include <iostream>
#include <string>
#include "patientnode.h"
#include "patientqueue.h"
#include "error.h"
#include "strlib.h"

using namespace std;

class LinkedListPatientQueue : public PatientQueue {
public:
    /*
     * Method: VectorPatientQueue
     * Usage: VectorPatientQueue PQ;
     * ----------------------
     * Initialize a new empty patient queue
     */
    LinkedListPatientQueue();

    /*
     * Method: ~VectorPatientQueue
     * Usage: automatically execute when object out of scope
     * ----------------------
     * Advoid memory leak when the class goes out of scope
     */
    ~LinkedListPatientQueue();

    /*
     * Method: frontName
     * Usage: cout << PQ.frontName();
     * ----------------------
     * Return the name of the most urgent patient
     */
    string frontName();

    /*
     * Method: clear
     * Usage: PQ.clear();
     * ----------------------
     * Free all memory occupied by the patient queue
     */
    void clear();

    /*
     * Method: frontPriority
     * Usage: cout << PQ.frontPriority();
     * ----------------------
     * Return the integer priority of the most urgent patient
     */
    int frontPriority();

    /*
     * Method: isEmpty
     * Usage: if (PQ.isEmpty())...
     * ----------------------
     * Return true if the patient queue does not contain any paitients
     * , and false if it does contain at least one patient.
     */
    bool isEmpty();

    /*
     * Method: newPatient
     * Usage: PQ.newPatient();
     * ----------------------
     * Add a new patient into the patient queue.
     */
    void newPatient(string name, int priority);

    /*
     * Method: processPatient
     * Usage: cout << PQ.processPatient();
     * ----------------------
     * Remove a most urgent patient from the patient queue.
     */
    string processPatient();

    /*
     * Method: upgradePatient
     * Usage: PQ.upgradePatient();
     * ----------------------
     * Updates a existing patient to a higher priority
     */
    void upgradePatient(string name, int newPriority);

    /*
     * Method: toString
     * Usage: cout << PQ.toString();
     * ----------------------
     * Returns the PatientQueue as a string in front-to-back order
     */
    string toString();

private:
    PatientNode* head; /* Pointer to the patient at the queue head */
};
