/*
 * File: grammarsolver.cpp
 * --------------------------
 * Name: Jian Zhong
 */

#include "grammarsolver.h"
#include "hashmap.h"
#include "random.h"

using namespace std;

// Function Prototypes:
Vector<string> grammarGenerate(istream&, const string&, int);
HashMap<string, Vector<string>> scanFileIntoMap(istream&);
void grammarHelper(const HashMap<string, Vector<string>>&, string, string&);

/**
 * Generates grammar for a given symbol a certain number of times given
 * a BNF input file.
 *
 * This will be called by grammarmain.cpp.
 *
 * @param input - Input stream of BNF file.
 * @param symbol - Symbol to generate
 * @param times - Number of times grammar is generated
 * @return Vector of strings of size times with random generations of symbol
 */
Vector<string> grammarGenerate(istream& input, string symbol, int times) {
    Vector<string> allResults;
    HashMap<string, Vector<string>> BNF = scanFileIntoMap(input);

    for (int i = 0; i < times; i++) {
        string result;
        grammarHelper(BNF, symbol, result);
        allResults.add(result);
    }

    return allResults;
}

/// Function to read the Input File and return a map of BNF content.
/// @param input - File input stream
/// @return a map of BNF content.
HashMap<string, Vector<string> > scanFileIntoMap(istream& input) {
    // scan the file line by line and stores contents into the BNFmap.
    string line;
    HashMap<string, Vector<string> > BNF;

    // while read a line in file,
    while(getline(input, line)) {
        // split symbol from rules by "::="
        Vector<string> splitedLine = stringSplit(line, "::=");

        // add symbols(splited[0]) and rules(splited[1]) into the BNFmap
        BNF.put(splitedLine[0], stringSplit(splitedLine[1], "|"));
    }
    return BNF;
}

/// Function to generate Random Expansions from the Grammar:
/// @param BNF - A map of BNF content
/// @param symbol - Symbol to generate
/// @param result - Grammer result
void grammarHelper(const HashMap<string, Vector<string>>& BNF, string symbol, string& result) {

    // if symbol is non-terminal symbol, randomly choose a sub-symbol and explorer it.
    if (BNF.containsKey(symbol)) {

        // find a random next symbol
        string symbolStr = randomElement(BNF[symbol]);

        // if the symbol contains spaces, multiple symbols exist.
        if (stringContains(symbolStr, " ")) {

            // split multiple symbols into a vector by whitespace.
            Vector<string> subSymbols = stringSplit(symbolStr, " ");

            // explorer each sub-symbol in subSymbols recursively.
            for (string subSymbol : subSymbols) {
                grammarHelper(BNF, subSymbol, result);
            }

        } else {
            // else, explorer the just 1 symbol recursively.
            grammarHelper(BNF, symbolStr, result);
        }

    } else {
        // found a terminal symbol!
        result += symbol + " "; // add it into result.
    }
}
