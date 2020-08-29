// VectorPatientQueue.cpp

#include "VectorPatientQueue.h"

// constructor
VectorPatientQueue::VectorPatientQueue() {}


// destructor: do nothing for Vector type queue
VectorPatientQueue::~VectorPatientQueue() {}


void VectorPatientQueue::clear() {
    patients.clear();
}

int VectorPatientQueue::mostUrgentIndex() {
    int minIndex = 0;
    int minPriority = patients[minIndex].priority; // minimum priority

    for (int i = 1; i < patients.size(); i++) {
        if (minPriority > patients[i].priority) {
            minPriority = patients[i].priority;
            minIndex = i;
        }
    }

    return minIndex;
}

string VectorPatientQueue::frontName() {
    // throw exception when accessing an empty list
    if (isEmpty()) {
        error("Attempting to find a front name from an empty queue.");
    }

    // find the first patient's name with the lowest priority.
    string minName = patients[mostUrgentIndex()].name;

    return minName;
}


int VectorPatientQueue::frontPriority() {
    // throw exception when accessing an empty list
    if (isEmpty()) {
        error("Attempting to find front priority from an empty queue.");
    }

    // find the first patient with the lowest priority.
    int minPriority = patients[mostUrgentIndex()].priority;

    return minPriority;
}


bool VectorPatientQueue::isEmpty() {
    return patients.size() == 0;
}


void VectorPatientQueue::newPatient(string name, int priority) {  
    // initialize a new patient type
    patientT newPatient;
    newPatient.name = name;
    newPatient.priority = priority;

    // add into the patient list
    patients.add(newPatient);
}


string VectorPatientQueue::processPatient() {
    // throw exception when accessing an empty list
    if (isEmpty()) {
        error("There is no top. I must abort. I never learned how to love...");
    }

    // remove the most urgent patient and return the patient name
    int minIndex = mostUrgentIndex();
    string minName = patients[minIndex].name;
    patients.remove(minIndex); // processing patient

    return minName;
}


void VectorPatientQueue::upgradePatient(string name, int newPriority) {
    int minPriority = MAXIMUM_PRIORITY; // set to maximum so that it can be updated in the loop's firt iteration.
    int index = -1;                     // set initial index as -1 to indicat that patient is not found

    // find the patient name with the highest priority in the patient queue
    for (int i = 0; i < patients.size(); i++) {
        if (patients[i].name == name && patients[i].priority < minPriority) {
            minPriority = patients[i].priority; // get minimum priority of the patient
            index = i;                          // get index of the patient
        }
    }

    // throw exception if patient does not exsit in queue,
    // otherwise, update priority only when it is more urgent:
    if (index == -1) {
        error("There is no such a patient in the queue.");
    } else {
        if (patients[index].priority > newPriority) { // update priority only when new priority is more urgent
            patients[index].priority = newPriority;   // update priority of that name!
        } else {
            error("The paitient already has a more urgent priority.");
        }
    }
}


string VectorPatientQueue::toString() {
    string output = "";

    for (int i = 0; i < patients.size(); i++) {
        if (i != 0) output += ", ";
        output += integerToString(patients[i].priority) + ":" + patients[i].name;
    }
    return "{" + output + "}";
}
