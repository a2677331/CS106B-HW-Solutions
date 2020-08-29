// LinkedListPatientQueue.cpp

#include "LinkedListPatientQueue.h"

LinkedListPatientQueue::LinkedListPatientQueue() {
    head = nullptr;
}


LinkedListPatientQueue::~LinkedListPatientQueue() {
    clear();
}


void LinkedListPatientQueue::clear() {
    // free memory of all patient nodes
    PatientNode* currP = head;
    PatientNode* trash = nullptr;
    while (currP != nullptr) {
        trash = currP;
        currP = currP->next;
        delete trash;
    }

    // Reset head and trash
    head = trash = nullptr; // don't want trash to be a dangling pointer.
}


string LinkedListPatientQueue::frontName() {
    if (isEmpty()) {
        error("Attempting to return front name in an EMPTY queue.");
    }
    return head->name;
}


int LinkedListPatientQueue::frontPriority() {
    if (isEmpty()) {
        error("Attempting to return front priority in an EMPTY queue.");
    }
    return head->priority;
}


bool LinkedListPatientQueue::isEmpty() {
    return head == nullptr;
}


void LinkedListPatientQueue::newPatient(string name, int priority) {
    PatientNode* newPatient = new PatientNode(name, priority, nullptr);
    PatientNode* currP = head;
    PatientNode* preP = nullptr;

    // find proper place to add new patient
    for (currP = head; currP != nullptr; currP = currP->next) {
        if (newPatient->priority < currP->priority) {
            break;
        }
        preP = currP;
    }

    // execute the add operation
    newPatient->next = currP;
    if (preP == nullptr) {     // either empty queue or non-empty queue but most urgent.
        head = newPatient;
    } else {                   // add in between or behind the last patient.
        preP->next = newPatient;
    }
}


string LinkedListPatientQueue::processPatient() {
    if (isEmpty()) {
        error("Attempting to process patient in an EMPTY queue.");
    }
    string nameToReturn  = head->name;
    PatientNode* trash = head;
    head = head->next;
    delete trash;

    return nameToReturn;
}


void LinkedListPatientQueue::upgradePatient(string name, int newPriority) {
    // if empty queue, throw exception
    if (isEmpty()) {
        error("Attempting to upgade a patient in an EMPTY queue.");
    }

    // search for name, if name occurs multiple times, update the most urgent's priority
    PatientNode* currP = head;
    PatientNode* preP = nullptr;
    while (currP != nullptr && currP->name != name) {
        preP = currP;
        currP = currP->next;
    }

    // if name matches, update priority.
    if (currP->name == name) {
        if (currP->priority > newPriority) {   // only update while the new priority is more ergent

            if (currP != head && newPriority < preP->priority) { // if the paitent needs to change position
                preP->next = currP->next;
                delete currP;                  // delete the current patient
                newPatient(name, newPriority); // re-insert the patient
            } else {                           // else just update the priority
                currP->priority = newPriority;
            }

        } else {
            error("Attempting to upgade a patient that is already MORE ERGENT.");
        }
    } else {
        // else no such a paitient name, throw exception
        error("Attempting to upgade a patient that DOES NOT EXIST in the queue.");
    }
}


string LinkedListPatientQueue::toString() {
    string output = "";
    PatientNode* currP = head;

    while (currP != nullptr) {
        if (currP != head) output += ", ";
        output += integerToString(currP->priority) + ":" + currP->name;
        currP = currP->next;
    }
    return "{" + output + "}";
}
