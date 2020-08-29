//
//  Created by Jian Zhong on 6/12/20.
//  Copyright © 2020 Jian Zhong. All rights reserved.
//


#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "hashmap.h"
#include "set.h"
#include "random.h"
#include "queue.h"
#include <locale>

using namespace std;
void greetings();
void inputMap(ifstream&);
HashMap<Vector<string>,Vector<string>> buildMap(const int&, ifstream&);
int getN();
void slideRandomWindow(Vector<string>& , Vector<string>&, string&,const HashMap<Vector<string>, Vector<string> >&);
bool getLenth(int&);
void NGrams(const HashMap<Vector<string>, Vector<string> >& map, const int& wordsLength);


int main() {
    ifstream  inFile;                            // input file stream
    HashMap<Vector<string>, Vector<string>> map; // Ngrams map
    int wordsLength;                             // # of random words to generate

    greetings();                     // print welcome messages
    inputMap(inFile);                // prompt user for file path of Ngrams map
    map = buildMap(getN(), inFile);  // prompt user for N, and setup Ngrams map
    while (getLenth(wordsLength)) {  // repeatdly prompt use for words length to generate random text until input is 0.
        NGrams(map, wordsLength);    // generate random text
    }

    cout << "Exiting."<< endl;
    return 0;
}


// Function to print greetings to user
void greetings() {
    cout << "Welcome to CS 106B Random Writer ('N-Grams'). " << endl
         << "This program makes random text based on a document. " << endl
         << "Give me an input file and an 'N' value for groups" << endl
         << "of words, and I'll create random text for you." << endl << endl;
}

// Function to open a file
// @param inFile file input stream
void inputMap(ifstream& inFile) {
    string fileName;
    while (true) {
        cout << "Input file name? ";
        getline(cin, fileName);
        inFile.open(fileName);
        if (!inFile.fail()) break;
        cout << "Canoot open the file. Try again." << endl;
    }
}

// Function to get N-Grams value
int getN() {
    int N;
    cout << "Value of N? ";
    cin >> N;
    while (!cin || cin.peek()!= '\n' || N < 2) {
        cin.clear();
        cin.ignore(200, '\n');
        cout << "Invalid input. Try again." << endl;
        cout << "Value of N? ";
        cin >> N;
    }
    return N;
}

// Function to build a Map for N-Grams
//
// @param N number of N-Grams
// @param inFile file input stream
HashMap<Vector<string>, Vector<string> > buildMap(const int& N, ifstream& inFile) {
    HashMap< Vector<string>, Vector<string> > map;
    Vector<string> window, suffix;
    Queue<string> wrapAround;
    string word;

    // Initalize window: read N - 1 words without prefixes.
    int windowSize = N - 1; // window size,
    for (int i = 0; i < windowSize; i++) {
        inFile >> word;  // to be
        window.add(word);
        // wrap around: save first N-1 words for later use
        wrapAround.enqueue(word);
    }

    // continue to read words until the end of file
    while (inFile >> word) {
        if (map.containsKey(window)) {
            map[window].add(word);
        } else {
            suffix.add(word);
            map.put(window, suffix); // add suffix
        }
        // reset window
        window.remove(0);
        window.add(word);
        suffix.clear(); // empty the sffix vector!!!
    }

    // now is the end of file, deal with wrap around:
    while (!wrapAround.isEmpty()) {
        word = wrapAround.dequeue();
        if (map.containsKey(window)) {
            map[window].add(word);
        } else {
            suffix.add(word);
            map.put(window, suffix); // add suffix
        }
        // reset window
        window.remove(0);
        window.add(word);
        suffix.clear(); // empty the sffix vector, before next loop!!!
    }

    return map;
}

// DescriptiongetLenth
//
// @param wordsLength # of random words to generate
bool getLenth(int& wordsLength) {
    cout << "# of random words to generate (0 to quit)? ";
    cin >> wordsLength;

    while (!cin || cin.peek()!= '\n' || wordsLength < 4) {

        if (cin && cin.peek() == '\n' && wordsLength == 0) {
            return false; // when input 0, return false.
        } else if (cin && cin.peek() == '\n' && wordsLength < 4) {
            cout << "Must be at least 4 words." << endl << endl;
        } else {
            cin.clear();
            cin.ignore(200, '\n');
            cout << "Invalid input. Try again." << endl << endl;
        }

        cout << "# of random words to generate (0 to quit)? ";
        cin >> wordsLength;
    }

    return true;
}

// Generating Random Text based on map
//
// @param map map of NGrams from buildMap function
// @param wordsLength # of random words to generate
void NGrams(const HashMap<Vector<string>, Vector<string> >& map, const int& wordsLength) {

    Vector<string> randomText = randomKey(map); // random text from random key
//    // Extra feature (1): find words of random text that starts with a Upper case letter.
//    char fLetter = randomText[0][0];
//    while (!isupper(fLetter)) {
//        // check if the first letter is upper case
//        randomText = randomKey(map);
//        fLetter = randomText[0][0];
//    }

    // Setup random window and random suffix:
    Vector<string> randomWindow = randomText;                 // setup random window
    string randomTextSuffix = randomElement(map[randomText]); // get a random suffix from map

    // Get the rest random text:
    int l = wordsLength - randomWindow.size(); // the length of rest test
    if (l <= 1){
        cout << "# of words should be greater than N." << endl << endl;
        return;
    }
    for (int i = 0; i < l; i++) {
        slideRandomWindow(randomText, randomWindow, randomTextSuffix, map);
    }

//    // Extra feature (2): continue to get words until found a punctuation character.
//    int suffixSize = randomTextSuffix.size();
//    char lLetter = randomTextSuffix[suffixSize - 1]; // get last letter of last word
//    // check if suffix has a punctuation character:
//    while (!ispunct(lLetter)) {
//        slideRandomWindow(randomText, randomWindow, randomTextSuffix, map);
//        // update last letter of last word!!!
//        suffixSize = randomTextSuffix.size();
//        lLetter = randomTextSuffix[suffixSize - 1];
//    }

    // Print the whole random text:
    cout << "... ";
    for (string w : randomText) {
        cout << w << " ";
    }
    cout << " ..." << endl << endl;
}

// Function to slide a random window of N-1 size in the Ngrams map
// then choose a reandom key and add values of the key into random text(final output).
//
// @param randomText final result of random text
// @param randomWindow  window of N-1 words
// @param randomTextSuffix suffixes of winodw
// @param map map of NGrams from buildMap function
void slideRandomWindow(Vector<string>& randomText, Vector<string>& randomWindow, string& randomTextSuffix,
                       const HashMap<Vector<string>, Vector<string> >& map) {
    // reset window
    randomWindow.remove(0);
    randomWindow.add(randomTextSuffix);
    // choose a random suffix from random window
    randomTextSuffix = randomElement(map[randomWindow]);
    // add random suffix into random text(output)
    randomText.add(randomTextSuffix);
}

