//
//  Created by Jian Zhong on 6/8/20.
//  Copyright © 2020 Jian Zhong. All rights reserved.
//

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "lexicon.h"
#include "queue.h"
#include <unordered_set>
using namespace std;

void printGreetings();
void promptDictionary(fstream&, string&, Lexicon&);
bool promptWords(string&, string&);
bool validFormat(const Lexicon&, const string&, const string&);
void printStack(Stack<string>);
void findWordLadder(const string&, const string&, const Lexicon&);

int main() {
    fstream inFile;
    string fileName, startWord, endWord;
    Lexicon dictionary;

    printGreetings();
    promptDictionary(inFile, fileName, dictionary);
    while (promptWords(startWord, endWord)) {
        findWordLadder(startWord, endWord, dictionary);
    }

    cout << "Have a nice day!" << endl;

    return 0;
}


void printGreetings() {
    cout << "Welcome to CS 106B Word Ladder." << endl
         << "Please give me two English words, and I will change the" << endl
         << "first into the second by changing one letter at a time." << endl
         << endl;
}

void promptDictionary(fstream& inFile, string& fileName, Lexicon& dictionary) {
    while (true) {
        cout << "Dictionary file name? ";
        getline(cin, fileName);
        inFile.open(fileName);
        if (!inFile.fail()) break;
        cout << "Unable to open that file.  Try again." << endl;
    }
    dictionary.addWordsFromFile(inFile);
}

bool promptWords(string& startWord, string& endWord) {
    // if either one input is an Enter key, end the program.
    cout << endl;
    cout << "Word #1 (or Enter to quit):";
    getline(cin, startWord);
    if (startWord.empty()) return false;

    cout << "Word #2 (or Enter to quit):";
    getline(cin, endWord);
    if (endWord.empty()) return false;

    // else, continue the game and uppercase both words.
    startWord = toLowerCase(startWord);
    endWord = toLowerCase(endWord);
    return true;
}

bool validFormat(const Lexicon& dictionary, const string& startWord, const string&  endWord) {
    if (!dictionary.contains(startWord) || !dictionary.contains(endWord)) {
        cout << "The two words must be found in the dictionary." << endl;
        return false;
    } else if (startWord.length() != endWord.length()) {
        cout << "The two words must be the same length." << endl;
        return false;
    } else if (startWord == endWord) {
        cout << "The two words must be different." << endl;
        return false;
    }
    return true;
}

void printResult(const string& startW, const string& endW, Stack<string>& ladder, bool found = false) {
    if (found) {
        cout << "A ladder from \"" << endW << "\" to \"" << startW << "\":" << endl;
        while(!ladder.isEmpty()) {
            cout << ladder.pop();
            if (ladder.size() != 0) cout << " -> ";
        }
        cout << endl;
    } else {
        cout << "No word ladder found from \"" << endW << "\" back to \""
             << startW << "\"." << endl;
    }
}

Lexicon neighborsOf(string currWord, const Lexicon& dictionary, Lexicon& usedWords) {
    // Find all neighbors of current word and store them into "neighbors":
    Lexicon neighbors;          // all neighbors of current word
    int size = currWord.size(); // word length

    // loop through the first letter to last letter of current word.
    for (int i = 0; i < size; i++) {
        for(char letter = 'a'; letter <= 'z'; letter++) {
            string temp = currWord;
            temp[i] = letter;
            if (dictionary.contains(temp) && !usedWords.contains(temp)) { // if is legal word and not been used yet.
                neighbors.add(temp);    // add it into neighbors collection
                usedWords.add(temp);    // also add it into used collection
            }
        }
    }
    return neighbors;
}

// findWordLadder(), this BFS algorithm check solution only when dequeueing a node from queue => slightly slower!
void findWordLadder(const string& startWord, const string& endWord, const Lexicon& dictionary) {
    // If invalid format, terminate this function:
    if (!validFormat(dictionary, startWord, endWord)) return;

    // Solve Word Ladder begins:
    Queue< Stack<string> > ladders; // ladder combination
    Stack<string> ladder;           // each possible ladder
    Lexicon usedWords;              // used words collection

    ladder.push(startWord);   // add start word to ladder stack
    ladders.enqueue(ladder);  // add ladder stack to the ladders queue
    usedWords.add(startWord); // add start word to the used word collection

    // Use BFS to serach a solution:
    while (!ladders.isEmpty()) {
        Stack<string> currLadder = ladders.dequeue(); // take out a ladder stack for evaluation
        string currWord = currLadder.peek();          // take out a word on top of the ladder stack

        // if solution found!
        if (currWord == endWord)  {
            printResult(startWord, endWord, currLadder, true); // print the result.
            return;
        }

        // For each unvisited neighbor: if found end word, return ladder with the neighbor, else enqueue sub-ladders of curr-ladder.
        for (string neighbor : neighborsOf(currWord, dictionary, usedWords)) {
            Stack<string> subLadder = currLadder;
            subLadder.push(neighbor);
            ladders.enqueue(subLadder);
        }
    }

    printResult(startWord, endWord, ladder, false); // all tried, print no results.
}
