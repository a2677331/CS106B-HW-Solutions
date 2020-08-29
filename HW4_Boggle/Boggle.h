
//
//  boggle.h
//
//  files for a Boggle class representing the state of the current Boggle game
//
//  Created by Jian Zhong on 7/14/20.
//  Copyright © 2020 Jian Zhong. All rights reserved.
//

#ifndef _boggle_h
#define _boggle_h

#include <iostream>
#include <string>
#include "lexicon.h"
#include "grid.h"
#include "bogglegui.h"
#include "shuffle.h"

using namespace std;

enum PlayerT {Human, Computer};

struct neigborT {
    char letter;
    int row = 0;
    int col = 0;
};

class Boggle {
public:
    /*
     * Constrct a game board according to boardText string.
     */
    Boggle(Lexicon& dictionary, string& boardText);

    /*
     * Check if the given word is suitable to search for.
     */
    bool checkWord(string word);

    /*
     * Cheack if the user-input word can be formed in boggle board.
     */
    bool humanWordSearch(string word);

    /*
     * perform a search on the board for all words that can be formed
     */
    Set<string> computerWordSearch();

    /*
     * Get human score
     */
    int getScoreHuman();

    /*
     * Get computer score
     */
    int getScoreComputer();

    /*
     * Get words found so far by player Human or by Computer.
     */
    Set<string> getWordsFound(const PlayerT& player) const;

    /*
     * Get the letter on board[row][col].
     */
    char getLetter(int row, int col);

    /*
     * Operator to print the boggle object
     */
    friend ostream& operator<<(ostream& out, Boggle& boggle);

private:
    /*
     * Helper for humanWordSearch().
     */
    bool humanHelper(string word, string formed,
                     int row, int col, Grid<char>& marked) const;

    /*
     * Helper for computerWordSearch().
     */
    void computerHelper(int row, int col, string formed,
                        Grid<char>& used, Set<string>& result);

    /*
     * Find all neighbors of a specified grid in boggle board, called by humanHelper().
     */
    Vector<neigborT> neigborsOf(int row, int col) const;

    /*
     * Adward score by words so far, called by getScoreHuman() and getScoreComputer().
     */
    int scoreAward(const Set<string>& wordsList) const;

    Lexicon boggleDict;         // boggle dictionary
    Grid<char> boggleBoard;     // boggle board
    Set<string> wordsHuman;     // human words list
    Set<string> wordsComputer;  // human words list
};

#endif // _boggle_h
