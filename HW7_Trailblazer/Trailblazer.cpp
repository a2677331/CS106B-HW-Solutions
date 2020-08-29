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

        if (extended.contains(currLoc)) continue;             // if have already been extended, skip it   注意：想了很久，如果已经extend过了的node，肯定是重复的、longer path，要skip掉这个list！
        extended.add(currLoc);                                // store visited location
        currLoc->setColor(Color::GREEN);                      // set Green for each visited location

        if (currLoc == end) return pathToTry;                 // Return the path if path found!

        for (RoadNode* neighbor : graph.neighborsOf(currLoc)) {  // for each neighbor
            if (!extended.contains(neighbor)) {
                paths.enqueue(pathToTry + Path(1, neighbor)); // enqueue path + unvisited nieghbor into queue, 注意：这里有可能给每个path都添加了相同的neighbor，所以之前要用if (visited.contains(currLoc))来跳过相同的neighbors。
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
        currCost -= Heuristic(graph, currLoc, end);     // node's accumulative path cost    // 注意：A*里每一个点的cost so far都是等于该点的priority - 该点的heuristic。

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
    Queue<RoadEdge*> allEdges = edgesOf(graph, bestPath); // get edges from best path    // 注意：这里debug了很久，如果用set，for里出来的node顺序会不一样，容易产生错误，用queue出来的顺序才和放进去的一致！
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
    double diff = ( (double)nodesA.size()  / nodesB.size() );  // 注意：这里debug了很久，size不是double的，要先转换成double才能比较，要不都是int，结果全是0！
    return diff > SUFFICIENT_DIFFERENCE; // true if diff > 0.2
}



//// A* algorithm implementation.
//Path aStar123(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
//    PriorityQueue<Path> paths;
//    HashSet<RoadNode*> extended;
//    paths.enqueue(Path(1, start), Heuristic(graph, start, end));  // enqueue starting node with a priority of heuristic value
//    start->setColor(Color::YELLOW);

//    // Implementing Dijkstras Algorithm
//    while (!paths.isEmpty()) {
//        Path pathToTry = paths.dequeue();
//        double currCost = pathCostOf(graph, pathToTry);        // total path cost so far in pathToTry
//        RoadNode* currLoc = pathToTry[pathToTry.size() - 1];

//        if (extended.contains(currLoc)) continue;
//        extended.add(currLoc);
//        currLoc->setColor(Color::GREEN);

//        if (currLoc == end) return pathToTry;

//        for (RoadNode* neighbor : graph.neighborsOf(currLoc)) {
//            if (!extended.contains(neighbor)) {
//                double newCost = currCost + graph.edgeBetween(currLoc, neighbor)->cost(); // add current path cost with neighbor cost.
//                double combinedHeuristic = newCost + Heuristic(graph, neighbor, end); // estimated cost from start node to end node
//                paths.enqueue(pathToTry + Path(1, neighbor), combinedHeuristic);
//                neighbor->setColor(Color::YELLOW);
//            }
//        }
//    }

//    return Path(); // If arrive here, return empty path(no solution)
//}




//// called by breadthFirstSearch1(), finding path from preMap[], which uses node as key, and previous node as value.
//Path findPath(const HashMap<RoadNode*, RoadNode*>& preMap, const HashSet<RoadNode*>& visited,
//              RoadNode* start, RoadNode* end) {

//    // Add nodes between start and end into the path:
//    Stack<RoadNode*> pathStack;     // path stores nodes from end to start order
//    Path paths;                     // path stores nodes from start to end order
//    RoadNode* pre = preMap[end];    // end's pre-node

//    pathStack.push(end);            // push end onto stack
//    while (visited.contains(pre)) {
//        if (pre == start) {
//            pathStack.push(pre);    // push start onto stack
//            break;
//        }
//        pathStack.push(pre);        // push nodes between start and end onto stack
//        pre = preMap[pre];
//    }

//    // Reverse the nodes' order so that it's from start to end.
//    while (!pathStack.isEmpty()) {
//        paths.add(pathStack.pop());
//    }

//    return paths;
//}

//// BFS algorithm using Queue<RoadNode*> roads;
//Path breadthFirstSearch1(const RoadGraph& graph, RoadNode* start, RoadNode* end) {
//    Path paths;                              // path from start to end
//    HashSet<RoadNode*> visited;
//    HashMap<RoadNode*, RoadNode*> preMap;   // map for storing unvisited nodes and its previous node
//    Queue<RoadNode*> roads;                 // a queue of road nodes
//    start->setColor(Color::YELLOW);
//    roads.enqueue(start);

//    while(!roads.isEmpty()) {
//        RoadNode* roadToTry = roads.dequeue();
//        if (visited.contains(roadToTry)) continue; // if already visited, skip it and dequeue next
//        /*
//         * 注意：想了很久，如果已经visit过了的node，肯定是重复的、longer path，要skip掉这个list！
//         */

//        visited.add(roadToTry);
//        roadToTry->setColor(Color::GREEN);

//        if (roadToTry == end) {                    // if end node found, find path and return!
//            return findPath(preMap, visited, start, end);
//        }

//        // Examine each neighbor of current node:
//        for (RoadNode* neighbor : graph.neighborsOf(roadToTry)) {
//            if (!visited.contains(neighbor)) {        // just want un-visited neighbor
//                if (!preMap.containsKey(neighbor)) {  // don't want to add the same node twice，
//                    /*
//                     * 注意：想了很久，如果已经有node和它的pre保存在map里，就证明它是最短的，因为BFS是一个一个level来的，
//                     * 所以如果再遇到其他重复的node和它的pre，那就证明是绕路了（即从更深的level又返回到了较早level的node），
//                     * 因此map里的已经有最短了的，所以不能加入了！！
//                     */
//                    preMap.put(neighbor, roadToTry);
//                }
//                neighbor->setColor(Color::YELLOW);
//                roads.enqueue(neighbor);
//            }
//        }
//    }

//    return paths;
//}

//// BFS algorithm implementation, using Queue<Path> pathsQueue,
//// and finds solution when enqueueing neighbors into queue => slightly faster!
//Path breadthFirstSearch(const RoadGraph& graph, RoadNode* start, RoadNode* end) {

//    // Case 1: Start and End are the same, return start:
//    if (start == end) return Path(1, start);

//    Queue<Path> pathsQueue;                  // queue of possible paths
//    HashSet<RoadNode*> visited;              // set of explored nodes
//    pathsQueue.enqueue(Path(1, start));
//    start->setColor(Color::YELLOW);

//    // BFS Search Algorithm:
//    while (!pathsQueue.isEmpty()) {
//        Path pathToTry = pathsQueue.dequeue();                  // get first path from queue
//        RoadNode* currLoc = pathToTry[pathToTry.size() - 1];    // get the last location of the path

//        if (visited.contains(currLoc)) continue;       // don't explore when already been visited(这里想了很久：其实没有也没关系，不过会极大影响效率，会有多余的，更长的路，所以在graph中找路的解法要注意加上这个！！！
//        visited.add(currLoc);
//        currLoc->setColor(Color::GREEN);

//        for (RoadNode* neighbor : graph.neighborsOf(currLoc)) {
//            if (visited.contains(neighbor)) continue;  // don't explore the neighbor when already been visited

//            // Case 2: Return the path if path found:
//            if (neighbor == end) {
//                neighbor->setColor(Color::GREEN);
//                pathToTry.add(neighbor);
//                return pathToTry;
//            } else {
//                pathsQueue.enqueue(pathToTry + Path(1, neighbor));
//                neighbor->setColor(Color::YELLOW);
//            }
//        }
//    }

//    // Case 3: Path not found, return empty path:
//    return Path();
//}


