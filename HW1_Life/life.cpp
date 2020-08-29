//  life.cpp
//
//  Created by Jian Zhong on 5/29/20.
//  Copyright © 2020 Jian Zhong. All rights reserved.
//***********************************************************************

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "grid.h"
#include "gwindow.h"
#include "simpio.h"
#include "strlib.h"
#include "lifegui.h"
#include "gevents.h"
using namespace std;

/*** Function Prototypes ***/
/* Functions in main */
void welcome_Messages();
void input_File(string, ifstream&);
void colony_Initializer(Grid<char>&, Grid<char>&, int&, int&, ifstream&, LifeGUI&);
void menu(char&, Grid<char>&, Grid<char>&, bool, LifeGUI&);
bool wrapping_Indicator();

/* Sub-functions */
void display_All_Colony(const Grid<char>&);
void read_Colony(Grid<char>&, ifstream&, LifeGUI&);
int  neighbourNum_NonWrap(const Grid<char>&, int, int);
int  neighbourNum_Wrap(const Grid<char>&, int, int);
void colony_Iteration(Grid<char>&, Grid<char>&, bool, LifeGUI&);
void colony_Animation(Grid<char>&, Grid<char>&, int, bool);


/*** main function begins here ***/
int main() {    
    /* Print welcome messages */
    welcome_Messages();

    /* Input colony file */
    ifstream fin;       // for file input
    string filePath;    // input file path
    input_File(filePath, fin);

    /* Initialize cell colony with data in file */
    int row, col;             // row # and col # for each colony
    Grid<char> currColony;    // current cell colony
    Grid<char> nextColony;    // next generation of current colony
    LifeGUI gui;              // gui whindow for game of life
    colony_Initializer(currColony, nextColony, row, col, fin, gui);

    /* Menu */
    char option;        // menu option
    menu(option, currColony, nextColony, wrapping_Indicator(), gui);

    /* Ending */
    cout << "Have a nice Life!" << endl;
    fin.close();

    return 0;
} /*** main function ends here ***/


// print welcome messages
void welcome_Messages() {
    cout << "Welcome to the CS 106B Game of Life,\n"
            "a simulation of the lifecycle of a bacteria colony.\n"
            "Cells (X) live and die by the following rules:\n"
            "- A cell with 1 or fewer neighbors dies.\n"
            "- Locations with 2 neighbors remain stable.\n"
            "- Locations with 3 neighbors will create life.\n"
            "- A cell with 4 or more neighbors dies.\n\n";
}

// Input text file
void input_File(string filePath, ifstream& fin) {
    while(true) {
        filePath = getLine("Grid input file name? ");
        if (!fileExists(filePath)) {
            cout << "Can't locate the file, try again.\n";
        } else {
            openFile(fin, filePath);
            break;
        }
    }
}


void colony_Initializer(Grid<char>& currColony, Grid<char>& nextColony, int& row, int& col, ifstream& fin, LifeGUI& gui) {
    // Read in data from file:
    fin >> row >> col;
    currColony.resize(row, col);       // setup size for current generation grids
    nextColony.resize(row, col);       // setup size for next generation grids
    gui.resize(row, col);              // setup size for GUI window
    read_Colony(currColony, fin, gui); // Read in currColony
    nextColony.deepCopy(currColony);   // copy currColony into nextColony

    // Print grids for the first time:
    cout << endl;
    display_All_Colony(currColony);
}

// create annimation accroding to frame number
void colony_Animation(Grid<char>& currColony, Grid<char>& nextColony, int frames, bool wrapping, LifeGUI& gui) {
    for (int i = 0; i < frames; i++) {
        // how many frames would generate for the animation
        clearConsole();
        colony_Iteration(currColony, nextColony, wrapping, gui);
        currColony.deepCopy(nextColony);
        pause(50); // pause time
    }
}

// main menu
void menu(char& option, Grid<char>& currColony, Grid<char>& nextColony, bool wrapping, LifeGUI& gui) {
    int frames; // for how many new generations are shown

    cout << "\nA)inmate, T)ick, Q)uit? ";
    cin >> option;
    // input validation
    while (!cin || (cin.peek() != '\n')) {
        cout << "Invalid input, try again.\n";
        cin.clear();
        cin.ignore(999, '\n');
        cout << "\nA)inmate, T)ick, Q)uit? ";
        cin >> option;
    }

    switch (option) {
    case 'a':
    case 'A':
        // animation option:
        cout << "How many frames? ";
        cin >> frames;
        // input validation
        while (!cin || (cin.peek() != '\n')) {
            cout << "Invalid input, try again.\n";
            cin.clear();
            cin.ignore(999, '\n');
            cout << "How many frames? ";
            cin >> frames;
        }
        colony_Animation(currColony, nextColony, frames, wrapping, gui);
        menu(option, currColony, nextColony, wrapping, gui);
        break;

    case 't':
    case 'T':
        // tick option:
        colony_Animation(currColony, nextColony, 1, wrapping, gui);
        menu(option, currColony, nextColony, wrapping, gui);
        break;

    case 'q':
    case 'Q':
        // quit program
        break;

    default:
        // otherwise prompt again:
        cin.clear();
        cin.ignore(999, '\n');
        cout << "Invalid input, Try again.";
        menu(option, currColony, nextColony, wrapping, gui);
        break;
    }
}

// print colony
void display_All_Colony(const Grid<char>& g) {
    for (int i = 0; i < g.numRows(); i++) {
        for(int j = 0; j < g.numCols(); j++){
            cout << g[i][j];
       }
        cout << endl;
    }
}

// non-wrapping condition
int neighbourNum_NonWrap(const Grid<char>& g, int r, int c) {
    int neighbour = 0;

    // count neighbours around each cell's 8 directioins
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (g.inBounds(r + i, c + j) && g[r + i][c + j] == 'X') {
                neighbour++;
            }
        }
    }

    // do not count itself if cell exists
    if (g[r][c] == 'X') {
        neighbour--;
    }

    return neighbour;
}

// wrapping condition
int neighbourNum_Wrap(const Grid<char>& g, int r, int c) {
    int neighbour = 0;

    // count neighbours around each cell's 8 directioins
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (g[(r+i+g.numRows()) % g.numRows()][(c+j+g.numCols()) % g.numCols()] == 'X') {
                neighbour++;
            }
        }
    }

    // do not count itself if cell exists
    if (g[r][c] == 'X') {
        neighbour--;
    }

    return neighbour;
}

// generation iteration: each function call will update the whole grid by 1.
void colony_Iteration(Grid<char>& currColony, Grid<char>& nextColony, bool wrapping, LifeGUI& gui) {
    int neigbourNumber;  // number of neigbour for each cell

    // loop over each grid in the colony
    for (int i = 0; i < currColony.numRows(); i++) {
        // for each row
        for (int j = 0; j < currColony.numCols(); j++) {
            // for each grid

            // eable wrapping or not
            if (wrapping) {
                neigbourNumber = neighbourNum_Wrap(currColony, i, j);
            } else {
                neigbourNumber = neighbourNum_NonWrap(currColony, i, j);
            }

            // apply rules of game of life:
            if (neigbourNumber <= 1 || neigbourNumber >= 4) { // when neigbour # >= 4 or <= 1, die;
                nextColony.set(i, j, '-');
                gui.drawCell(i, j, false);
            } else if (neigbourNumber == 3) { // when neigbour # == 3, live;
                nextColony.set(i, j, 'X');
                gui.drawCell(i, j, true);
            }

        }
    }

    // after update, print colony
    display_All_Colony(nextColony);
}


// Prompt user for wrapping condition:
bool wrapping_Indicator() {
    char wrapOption;

    cout << "Should the simulation wrap around the grid (y/n)? ";
    cin >> wrapOption;

    // input validation
    while(!cin || cin.peek() != '\n') {
        cout << "Invalid input, try again.\n";
        cin.clear();
        cin.ignore(999, '\n');
        cout << "Should the simulation wrap around the grid (y/n)? ";
        cin >> wrapOption;
    }

    switch (wrapOption) {
    case 'y':
    case 'Y':
        return true;
        break;
    case 'n':
    case 'N':
        return false;
        break;
    default:
        cin.clear();
        cin.ignore(9999, '\n');
        cout << "Invalid input, try again. \n";
        return wrapping_Indicator();
        break;
    }
}

// read in colony grids
void read_Colony(Grid<char>& g, ifstream& fin, LifeGUI& gui) {
    for (int i = 0; i < g.numRows(); i++) {
        for(int j = 0; j < g.numCols(); j++){
            fin >> g[i][j];
            gui.drawCell(i, j, g[i][j] == 'X');
       }
    }
}
