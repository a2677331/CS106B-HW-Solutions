//  boggleplay.cpp
//
//  client to perform console UI and work with Boggle class to play a game
//
//  Created by Jian Zhong on 7/14/20.
//
//  Copyright © 2020 Jian Zhong. All rights reserved.

#include "lexicon.h"   // store dictionary file
#include "simpio.h"    // getYesOrNo();
#include "bogglegui.h" // BoggleGUI
#include "Boggle.h"    // Boggle class
#include "console.h"   // clearConsole(): to clear the screen
#include <ctype.h>     // isalpha()
using namespace std;


// Function prototypes:
void blankBoard();
string getBoardText();
void printBoard(const string&);
void playBoggle(Boggle&, string = "FirstPrompt");
void announceWinner(Boggle&);
bool isValidText(string&);
string mannualInput();
void printState(Boggle&, const PlayerT&);


/*
 * playOneGame()
 */
void playOneGame(Lexicon& dictionary) {
    // Setup blank Boggle GUI
    blankBoard();

    // Get board text for the Boggle.
    string boardText = getBoardText();

    // Setup Boggle Board with board text
    Boggle board(dictionary, boardText);

    // Setup and print Boggle GUI
    printBoard(boardText);

    // Play Boggle game one time
    playBoggle(board);

    // Announce Winner
    announceWinner(board);
}


// Function Definitions:
bool isValidBoardText(string& boardText) {
    boardText = getLine("Type the 16 letters to appear on the board: ");

    if (boardText.length() != 16) return false;

    for (unsigned i = 0; i < boardText.length(); i++) {
        if (!isalpha(boardText[i])) return false;
    }

    return true;
}

string mannualInput() {
    string boardText;
    while (!isValidBoardText(boardText)) {
        cout << "That is not a valid 16-letter board string. Try again." << endl;
    }

    return boardText;
}

string getBoardText() {
    return (getYesOrNo("Do you want to generate a random board?")) ? "" : mannualInput();
}

void blankBoard() {
    if (!BoggleGUI::isInitialized()) {
        BoggleGUI::initialize(4,4);
    } else {
        BoggleGUI::reset();
    }
}

void printBoard(const string& boardText) {
    BoggleGUI::labelAllCubes(boardText);
    BoggleGUI::setScore(0, BoggleGUI::HUMAN);
}

void printState(Boggle& board, const PlayerT& whichPlayer) {
    if (whichPlayer == Human) {
        cout << board << endl; // print boggle board
        cout << "Your words (" << board.getWordsFound(Human).size() << "): "
             << board.getWordsFound(Human) << endl;
        cout << "Your score: " << board.getScoreHuman() << endl;
    } else if (whichPlayer == Computer) {

        cout << endl << "It's my turn!" << endl;
        cout << "My words (" << board.getWordsFound(Computer).size() << "): "
             << board.getWordsFound(Computer) << endl;
        cout << "My score: " << board.getScoreComputer() << endl;

    } else {
        throw "Invaild Player type.";
    }
}


void playBoggle(Boggle& board, string word) {
    bool QUIT = false;
    string response;

    // Human turn comes first, after enter key is pressed, then starts the computer turn.
    do {
        // Human Turn:
        clearConsole();  // clean up console
        if (word == "FirstPrompt") {
            response = "It's your turn!";
        } else if (!board.checkWord(word)) {
            response = "You must enter an unfound 4+ letter word from the dictionary.";
        } else if (!board.humanWordSearch(word)) {
            response = "That word can't be formed on this board.";
        } else {        // word found !
            BoggleGUI::recordWord(word, BoggleGUI::HUMAN);
            response = "You found a new word! \"" + toUpperCase(word) + "\"";
        }
        cout << response << endl;
        BoggleGUI::setStatusMessage(response);              // print GUI message
        printState(board, Human);                           // print state
        word = getLine("Type a word (or Enter to stop): "); // re-prompt for word
        BoggleGUI::clearHighlighting();                     // clean GUI Highlighting

        // Computer Turn:
        if (word == "") {
            // let computer to find all words on board
            board.computerWordSearch();

            // print computer state on console
            printState(board, Computer);

            // print computer words so far on GUI window.
            for (string word : board.getWordsFound(Computer)) {
                BoggleGUI::recordWord(word, BoggleGUI::COMPUTER);
            }

            // end the game for one round
            QUIT = true;
        }
    } while(!QUIT);
}

void announceWinner(Boggle& board) {
    string response;
    if (board.getScoreHuman() < board.getScoreComputer()) {
        response = "Ha ha ha, I destroyed you. Better luck next time, puny human!";
    } else {
        response = "WOW, you defeated me! Congratulations!";
    }
    cout << response << endl;
    BoggleGUI::setStatusMessage(response);
}
