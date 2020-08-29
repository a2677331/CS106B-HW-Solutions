//
//  encoding.cpp
//
//  Created by Jian Zhong on 8/10/20.
//
//  Copyright © 2020 Jian Zhong. All rights reserved.
//

#include "encoding.h"
#include "HuffmanNode.h"    // HuffmanNode
#include "pqueue.h"         // PriorityQueue<HuffmanNode* > PQ;
#include "filelib.h"        // rewindStream(input);

// To build a Frequency Table
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqMap;  // frequency map

    // read each character in input file, add it into frequence map until EOF.
    for (int ch = input.get(); ch != -1; ch = input.get()) {
        if (!freqMap.containsKey(ch)) {
            freqMap.put(ch, 1);
        } else {
            freqMap[ch]++;
        }
    }

    freqMap.put(PSEUDO_EOF, 1); // add EOF character
    return freqMap;
}

// Called by buildEncodingTree, to build an encoding tree.
HuffmanNode* recEncodingTree(PriorityQueue<HuffmanNode*> & PQ) {
    // Base Case: return node if only one node in PQ
    if (PQ.size() == 1) {
        return PQ.dequeue();
    }

    // Recursive Case: repeatly dequeue two nodes as children of a new node, then re-insert.
    HuffmanNode *left = PQ.dequeue(),
                *right = PQ.dequeue(),
                *newSum = new HuffmanNode(NOT_A_CHAR, left->count + right->count, left, right);

    // Re-insert into PQ
    PQ.enqueue(newSum, newSum->count);

    return recEncodingTree(PQ);         // dequeue the rest in PQ by recursion
}

// To build an encoding tree, recEncodingTree does the actual building work.
HuffmanNode* buildEncodingTree(const Map<int, int>& freqMap) {
    PriorityQueue<HuffmanNode*> PQ;  // priority queue of node pointers

    // create PQ from freqMap
    for (int ch : freqMap) {
        HuffmanNode* letter = new HuffmanNode(ch, freqMap[ch], NULL, NULL);
        PQ.enqueue(letter, letter->count);
    }

    // return Huffman Tree based on PQ
    return recEncodingTree(PQ);
}

// Fecursively build encoding map
Map<int, string> recEncodingMap(HuffmanNode* encodingTree, Map<int, string>& encodingMap, string encode) {
    if (encodingTree->isLeaf()) {                         // when meets leaf
        encodingMap.put(encodingTree->character, encode); // add the character into encoding map
    } else {
        recEncodingMap(encodingTree->zero, encodingMap, encode + "0");
        recEncodingMap(encodingTree->one, encodingMap, encode + "1");
    }

    return encodingMap;
}

// Call recEncodingMap(), which does the actual work to build encoding map.
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;

    // Build encoding map. by traversing the Huffman Tree.
    return recEncodingMap(encodingTree, encodingMap, "");
}


// Encode a file's text into a compressed binary representation using the encoding map.
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    for (int ch = input.get(); ch != -1; ch = input.get()) { // read each character in file
        if (encodingMap.containsKey(ch)) {
            string encode = encodingMap.get(ch);             // find encode in encoding map
            for (unsigned i = 0; i < encode.size(); i++) {
                output.writeBit(encode[i] - '0');            // write encode to output bit by bit
            }
        }
    }

    // write delimiter binary representation.
    string delimiter = encodingMap.get(PSEUDO_EOF);
    for (unsigned i = 0; i < delimiter.size(); i++) { // take out one bit per time
        output.writeBit(delimiter[i] - '0');          // write to output file
    }
}

// Decode text that was previously encoded with its binary patterns
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    HuffmanNode* curr = encodingTree;
    int bitToRead = input.readBit();

    // Read bits until the EOF
    while (bitToRead != -1) {
        if (curr != NULL) { // make sure continue to read bit if empty encoding Tree

            // Moving
            if (bitToRead == 0)
                curr = curr->zero;
            else
                curr = curr->one;

            // Character found!
            if (curr->isLeaf()) {
                if (curr->character == PSEUDO_EOF)  break;  // don't want to output EOF
                output << char(curr->character);            // output character
                curr = encodingTree;                        // move back to find another
            }

        }
        bitToRead = input.readBit();  // Read next bit
    }
}

// To compress a given input file into a given output file.
void compress(istream& input, obitstream& output) {
    // Build encoding of its contents
    Map<int, int> F_Map = buildFrequencyTable(input);
    HuffmanNode* H_Tree = buildEncodingTree(F_Map);
    Map<int, string> E_Map = buildEncodingMap(H_Tree);

    // Write encoded data to output file
    output << F_Map;                    // write header
    rewindStream(input);                // move input stream back for encoding data(important!)
    encodeData(input, E_Map, output);   // write compressed content
}


// To decompress a given input file into a given output file.
void decompress(ibitstream& input, ostream& output) {
    // Read header(frequency map) in input file
    Map<int, int> F_Map;
    input >> F_Map;

    // Build encoding tree and encoding map
    HuffmanNode* H_Tree = buildEncodingTree(F_Map);

    // Write decoded data to output file
    decodeData(input, H_Tree, output);
}


// Free memory occupied by encoding tree
void freeTree(HuffmanNode* node) {
    if (node != NULL) {
        freeTree(node->zero);
        freeTree(node->one);
        delete node;
    }
}


/********************************* Extra ***************************************************/

/*
 * Recursive Version of Decoding Data(CANNOT work on file that is too large).
 *
 * Used in decodeData();
 *
 * Usage: recDecodeData(input, encodingTree, output, encodingTree);
 *
 */
void recDecodeData(ibitstream& input, HuffmanNode* tree, ostream& output, HuffmanNode* root) {
    // if hitting a leaf
    if (tree->isLeaf()) {
        if (tree->character == PSEUDO_EOF) return; // stop if meets EOF
        output << char(tree->character);           // else output the character
        recDecodeData(input, root, output, root);  // move back to find another character
    } else {
        // traverse Huffman tree to find character
        int bitToRead = input.readBit();                    // get one bit from file text
        if (bitToRead == 0) {
            recDecodeData(input, tree->zero, output, root); // if read 0, move left.
        } else if (bitToRead == 1){
            recDecodeData(input, tree->one, output, root);  // if read 1, move right.
        } else {
            return;   // stop if detect EOF or non-binary digits in input file
        }
    }
}
