// VectorPatientQueue.h

#pragma once

#include <iostream>
#include <string>
#include "patientqueue.h"
#include "vector.h"

using namespace std;

const int MAXIMUM_PRIORITY = 999999999;

class VectorPatientQueue : public PatientQueue {
public:
    /*
     * Method: VectorPatientQueue
     * Usage: VectorPatientQueue PQ;
     * ----------------------
     * Initialize a new empty patient queue
     */
    VectorPatientQueue();

    /*
     * Method: ~VectorPatientQueue
     * Usage: automatically execute when object out of scope
     * ----------------------
     * Do nothing for vector type patient queue
     */
    ~VectorPatientQueue();

    /*
     * Method: clear
     * Usage: PQ.clear();
     * ----------------------
     * Remove all elements from the patient queue
     */
    void clear();

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
     * Puts person into the patient queue, append to a vector
     */
    void newPatient(string name, int priority);

    /*
     * Method: frontPriority
     * Usage: cout << PQ.frontPriority();
     * ----------------------
     * Return the integer priority of the most urgent patient
     */
    int frontPriority();

    /*
     * Method: frontName
     * Usage: cout << PQ.frontName();
     * ----------------------
     * Return the name of the most urgent patient
     */
    string frontName();

    /*
     * Method: processPatient
     * Usage: cout << PQ.processPatient();
     * ----------------------
     * Scan vector list, return and remove the highest-priority patient.
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
    struct patientT {          // patient type defines here.
        string name;
        int priority;
    };

    /*
     * Method: mostUrgentIndex
     * Usage: int index = mostUrgentIndex();
     * ----------------------
     * Returns the most urgent patient's index
     */
    int mostUrgentIndex();

    Vector<patientT> patients; // patient queue
};

