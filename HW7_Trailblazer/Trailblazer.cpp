//
//  Trailblazer.cpp
//
//  Created by Jian Zhong on 8/24/20.
//
//  Copyright © 2020 Jian Zhong. All rights reserved.
//

#include "Trailblazer.h"
#include "queue.h"
#include "priorityqueue.h"
#include "stack.h"
#include "hashset.h"
#include "set.h"

using namespace std;

static const double SUFFICIENT_DIFFERENCE = 0.2;

/*
 * Prototypes of Helper Functions
 */

// get Heuristic cost speicified by (crow-fly-distance / max-speed) between S and E(goal node).
double Heuristic(const RoadGraph& g, RoadNode* s, RoadNode* e);
// get total path cost by adding up all edges' costs in a path.
double pathCostOf(const RoadGraph& graph, const Path& path);
// get all edges from a path
Queue<RoadEdge*> edgesOf(const RoadGraph& graph, Path& path);
// Return true if a path has enough different nodes from another path.
bool isLegalPath(Path& pathA, Path& pathB);


// BFS algorithm uses Queue<Path> pathsQueue,
// and finds solution only when dequeuing a node from queue.
// It also uses extended set to greatly improve efficiency.
Path breadthFirstSearch(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    Queue<Path> paths;              // queue of possible paths
    HashSet<RoadNode*> extended;    // set for storing extended nodes
    paths.enqueue(Path(1, start));  // initialize a path storing the start node
    start->setColor(Color::YELLOW);

    // BFS Search Algorithm:
    while (!paths.isEmpty()) {
        Path pathToTry = paths.dequeue();                     // get first path from queue
        RoadNode* currLoc = pathToTry[pathToTry.size() - 1];  // get the last location of the path

        if (extended.contains(currLoc)) continue;             // if have already been extended, skip it
        extended.add(currLoc);                                // store visited location
        currLoc->setColor(Color::GREEN);                      // set Green for each visited location

        if (currLoc == end) return pathToTry;                 // Return the path if path found!

        for (RoadNode* neighbor : graph.neighborsOf(currLoc)) {  // for each neighbor
            if (!extended.contains(neighbor)) {
                paths.enqueue(pathToTry + Path(1, neighbor)); // enqueue path + unvisited nieghbor into queue
                neighbor->setColor(Color::YELLOW);
            }
        }
    }

    return Path(); // If arrive here, return empty path(no solution)
}


// Dijkstras algorithm implementation.
Path dijkstrasAlgorithm(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    PriorityQueue<Path> paths;
    HashSet<RoadNode*> extended;
    paths.enqueue(Path(1, start), 0);  // enqueue starting node with a priority of 0 cost
    start->setColor(Color::YELLOW);

    // Implementing Dijkstras Algorithm
    while (!paths.isEmpty()) {
        double currCost = paths.peekPriority();
        Path pathToTry = paths.dequeue();
        RoadNode* currLoc = pathToTry[pathToTry.size() - 1];

        if (extended.contains(currLoc)) continue;
        extended.add(currLoc);
        currLoc->setColor(Color::GREEN);

        if (currLoc == end) return pathToTry;

        for (RoadNode* neighbor : graph.neighborsOf(currLoc)) {
            if (!extended.contains(neighbor)) {
                double newCost = currCost + graph.edgeBetween(currLoc, neighbor)->cost();
                paths.enqueue(pathToTry + Path(1, neighbor), newCost);
                neighbor->setColor(Color::YELLOW);
            }
        }
    }

    return Path(); // If arrive here, return empty path(no solution)
}


// An A* based algorithm which you can choose to implement Dijkstras, or A*, or Alternate Route algorithm.
// Implement A* algorithm by default.
Path aStarBasedSearch(const RoadGraph& graph, RoadNode* start, RoadNode* end,
                      bool FindAlternate = false, RoadEdge* edgeToRemove = NULL) {
    PriorityQueue<Path> paths;
    HashSet<RoadNode*> extended;
    paths.enqueue(Path(1, start), Heuristic(graph, start, end));  // enqueue starting node with a priority of heuristic value
    start->setColor(Color::YELLOW);

    // Implementing Dijkstras Algorithm
    while (!paths.isEmpty()) {
        double currCost = paths.peekPriority();         // node's accumulative path cost combined with its Heuristc
        Path pathToTry = paths.dequeue();
        RoadNode* currLoc = pathToTry[pathToTry.size() - 1];
        currCost -= Heuristic(graph, currLoc, end);     // node's accumulative path cost

        if (extended.contains(currLoc)) continue;       // improve efficiency, don't want to explore longer repeated route
        extended.add(currLoc);
        currLoc->setColor(Color::GREEN);

        if (currLoc == end) return pathToTry;           // Return the path if found!

        for (RoadNode* neighbor : graph.neighborsOf(currLoc)) {
            if (!extended.contains(neighbor)) {

                // Specified codes for finding alternative path
                if (FindAlternate) {
                     if (edgeToRemove->from() == currLoc && edgeToRemove->to() == neighbor) continue;
                }

                double newCost = currCost + graph.edgeBetween(currLoc, neighbor)->cost(); // new path cost from start to neighbor.
                double combinedHeuristic = newCost + Heuristic(graph, neighbor, end);     // estimated cost from start node to end
                paths.enqueue(pathToTry + Path(1, neighbor), combinedHeuristic);
                neighbor->setColor(Color::YELLOW);
            }
        }
    }

    return Path(); // If arrive here, return empty path(no solution)
}


// A* algorithm implementation.
Path aStar(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    return aStarBasedSearch(graph, start, end); // If arrive here, return empty path(no solution)
}


// Alternative Route algorithm implementation: find a path that is at least 20% different than another path.
Path alternativeRoute(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
    Path bestPath = aStar(graph, start, end);             // get the best path
    Queue<RoadEdge*> allEdges = edgesOf(graph, bestPath); // get edges from best path
    PriorityQueue<Path> alternativePathsQueue;            // alternative candidates with cost as its priority

    // for each edge, find the shortest path doesn't use that edge and add it into PQ:
    while (!allEdges.isEmpty()) {
        RoadEdge* edgeToSkip = allEdges.dequeue();
        Path alternativePath = aStarBasedSearch(graph, start, end, true, edgeToSkip);
        if (isLegalPath(alternativePath, bestPath)) {
            alternativePathsQueue.add(alternativePath, pathCostOf(graph, alternativePath) );
        }
    }

    // If empty queue, then no solutions, else return alternative path.
    if (alternativePathsQueue.isEmpty()) {
        return Path();
    }
    else {
        return alternativePathsQueue.dequeue();
    }
}


/*** Definitions of Helper Functions ***/

// get Heuristic cost from s node to e node in a graph.
double Heuristic(const RoadGraph& g, RoadNode* s, RoadNode* e) {
    return g.crowFlyDistanceBetween(s, e) / g.maxRoadSpeed();  // the fastest time from s node to e node
}

// get total path cost by adding up all edges' costs in a path.
double pathCostOf(const RoadGraph& graph, const Path& path) {
    double cost = 0;
    for(int i = 0; i < path.size() - 1; i++) {
        cost += graph.edgeBetween(path[i], path[i+1])->cost();
    }
    return cost;
}

// get all edges from a path
Queue<RoadEdge*> edgesOf(const RoadGraph& graph, Path& path) {
    Queue<RoadEdge*>  edges;
    for (int i = 0; i < path.size() - 1; i++) {
        edges.enqueue(graph.edgeBetween(path[i], path[i+1]) );
    }
    return edges;
}

// Return true if a path that is at least 20% different than another path.
bool isLegalPath(Path& pathA, Path& pathB) {
    HashSet<RoadNode*> nodesA; // nodes in path A
    HashSet<RoadNode*> nodesB; // nodes in path B

    for (RoadNode* node : pathA) {
        nodesA.add(node);
    }
    for (RoadNode* node : pathB) {
        nodesB.add(node);
    }

    nodesA -= nodesB;          // nodes in path A but not in path B
    double diff = ( (double)nodesA.size()  / nodesB.size() );
    return diff > SUFFICIENT_DIFFERENCE; // true if diff > 0.2
}


