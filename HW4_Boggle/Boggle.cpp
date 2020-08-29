//
//  boggle.cpp
//
//  Created by Jian Zhong on 7/14/20.
//  Copyright © 2020 Jian Zhong. All rights reserved.
//

#include "Boggle.h"

// letters on all 6 sides of every cube
static string CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

Boggle::Boggle(Lexicon& dictionary, string& boardText) {
    boggleBoard.resize(4,4); // initialize a 4x4 game board
    boggleDict = dictionary; // setup boggle dictionary

    // BoggleGUI::labelAllCubes(boardText);

    // generated ramdom board text
    if (boardText.empty()) {
        shuffle(CUBES, 16); // shuffle the 16 cube loations in CUBES
        int pos = 0;        // shuffle the cube letter
        for (int r = 0; r < boggleBoard.nRows; r++) {
            for(int c = 0; c < boggleBoard.nCols; c++) {
                boggleBoard[r][c] = CUBES[pos][randomInteger(0, 5)];
                boardText += boggleBoard[r][c];
                pos++;
            }
        }
    } else {
    // user mannual input board text
         boardText = toUpperCase(boardText); // just uppcase all letters
         int pos = 0;
         for (int r = 0; r < boggleBoard.nRows; r++) {
             for(int c = 0; c < boggleBoard.nCols; c++) {
                 boggleBoard[r][c] = boardText[pos];
                 pos++;
             }
         }
    }

}

char Boggle::getLetter(int row, int col) {
    if (!boggleBoard.inBounds(row, col)) throw int(); // if out of bounds
    return boggleBoard[row][col];
}


bool Boggle::checkWord(string word) {
    word = toUpperCase(word);          // case-insensity
    
    if (word.length() >= 4 &&          // valid Boggle word
        !wordsHuman.contains(word) &&  // has not already been found
        boggleDict.contains(word)) {   // is in boggleDict
        return true;
    }
    return false;
}

bool Boggle::humanWordSearch(string word) {
    BoggleGUI::setAnimationDelay(100);  // set GUI delay time
    word = toUpperCase(word);      // case-insensitive
    char first = word[0];          // letter to be found
    Grid<char> marked;             // creat a separate board for marking
    marked.resize(4, 4);           // a separate grid to record used grid, 'X' means used.

    // loop through the board grids
    for (int r = 0; r < boggleBoard.nRows; r++) {
        for (int c = 0; c < boggleBoard.nCols; c++) {

            // hightlight each grid while searching
            BoggleGUI::setHighlighted(r, c, true);
            marked.set(r, c, 'X'); // mark the grid as used.

            // if first letter of word found on a grid:
            if (first == boggleBoard[r][c]) {
                
                // try to form word based on boggleBoard[r][c] by recursive bactrack
                if (humanHelper(word, word.substr(0, 1), r, c, marked)) { // if can be formed!
                    wordsHuman.add(word);
                    return true;
                } else {    // if can't be formed
                    BoggleGUI::setHighlighted(r, c, false); // un-hightlight the grid
                    marked.set(r, c, ' ');                  // un-mark the grid
                }
                
            } else { // if not found on that grid
                BoggleGUI::setHighlighted(r, c, false); // un-hightlight the grid
                marked.set(r, c, ' ');                  // un-mark the grid
            }
            
        }
    }
    return false; // finish loopping the entire board, if no word can be formed, return false.
}

bool Boggle::humanHelper(string word, string formed, int row, int col, Grid<char>& marked) const {
    char next = word[1];  // get the second letter of word

    // Base Cases: if word only 1 letter, it means formed now is a complete word, so word found!
    if (word.length() == 1) return true;

    // generate all available neighbors of current gird
    Vector<neigborT> nList = neigborsOf(row, col);

    // for each available neighbor of current grid
    for (int i = 0; i < nList.size(); i++) {
        char neiToTry = nList[i].letter;   // choose a availabe negihbor
        int nRow = nList[i].row;           // neigbor's row
        int nCol = nList[i].col;           // neigbor's col

        // each time meets a marked(used) neighbor, skip it.
        if (marked[nRow][nCol] == 'X') continue;

        BoggleGUI::setHighlighted(nRow, nCol, true);       // hightlight the chosen neighbor

        // if the neighbor is exactly the next letter , fomred is in dictionary, and unmarked:
        if (neiToTry == next && boggleDict.containsPrefix(formed + next)) {

            // Choose:
            marked.set(nRow, nCol, 'X');                   // mark the neigbor
            BoggleGUI::setHighlighted(nRow, nCol, true);   // hightlight the choosen negihbor

            // Explore: the rest by recursion, return true if found!
            if (humanHelper(word.substr(1), formed + next, nRow, nCol, marked)) return true;

            // Un-choose:
            marked.set(nRow, nCol, ' ');                   // un-mark the neigbor
            BoggleGUI::setHighlighted(nRow, nCol, false);  // un-hightlight every un-choosen path
        }

        BoggleGUI::setHighlighted(nRow, nCol, false);      // un-hightlight the choosen negihbor
    }
    return false;  // all neigbors have tried, return false.
}

Vector<neigborT> Boggle::neigborsOf(int row, int col) const {
    Vector<neigborT> neighborList;
    neigborT newNeigbor;

    for (int r = row-1; r <= row+1; r++) {
        for (int c = col-1; c <= col+1; c++) {
            if (r != row || c != col ) {            // don't want itself
                if (boggleBoard.inBounds(r, c)) {   // must be inside the board
                    newNeigbor.row = r;
                    newNeigbor.col = c;
                    newNeigbor.letter = boggleBoard[r][c];
                    neighborList.add(newNeigbor);
                }
            }
        }
    }
    return neighborList;
}

Set<string> Boggle::computerWordSearch() {
    Set<string> result; // all words found by computer
    Grid<char> used;    // a separate grid to record used grid, 'X' means used.
    used.resize(4, 4);

    for (int c = 0; c < boggleBoard.nCols; c++) {
        for (int r = 0; r < boggleBoard.nRows; r++) {
            string formed = "";
            formed += getLetter(c, r); // get the visted formed letter
            used.set(c, r, 'X');       // marked it as used
            computerHelper(c, r, formed, used, result);
            used.set(c, r, ' ');       // un-marked it
        }
    }
    wordsComputer = result; // update computer's word set.

    return result;
}


void Boggle::computerHelper(int row, int col, string formed,
                            Grid<char>& used, Set<string>& result) {
    // Base cases:
    if (checkWord(formed)) result.add(formed); // Found word! add into words' set!

    // Find all available neigbors of current gird.
    Vector<neigborT> neigList = neigborsOf(row, col);
    int neigSize = neigList.size();

    // Keep searching if board[row][col] have neigbors,
    if (neigSize > 0){
        
        // for each neighbor
        for (int i = 0; i < neigSize; i++) {
            // choose a neigbor
            char neigh = neigList[i].letter;
            int nRow = neigList[i].row;
            int nCol = neigList[i].col;
            if (boggleDict.containsPrefix(formed + neigh) && used[nRow][nCol] != 'X') {
                // if is the neighbor contians valid prefix and not used
                used.set(nRow, nCol, 'X'); // marked as used
                computerHelper(nRow, nCol, formed + neigh, used, result); // explore next by recursion
                used.set(nRow, nCol, ' '); // unmark it
            }
        }
        
    }

}

int Boggle::scoreAward(const Set<string>& wordsList) const {
    int score = 0;
    for (string word : wordsList) {
        score += word.length() - 3;
    }
    return score;
}

int Boggle::getScoreHuman() {
    return scoreAward(wordsHuman);
}

int Boggle::getScoreComputer() {
    return scoreAward(wordsComputer);
}

Set<string> Boggle::getWordsFound(const PlayerT& player) const {
    if (player == Human) {
        return wordsHuman;
    } else if (player == Computer) {
        return wordsComputer;
    }
    throw "The player type is invalid.";
}

ostream& operator<<(ostream& out, Boggle& boggle) {
    for (int r = 0; r < 4; r++) {
        for(int c = 0; c < 4; c++) {
            out << boggle.getLetter(r, c);
            if ((c+1) % 4 == 0) out << endl;
        }
    }
    return out;
}
