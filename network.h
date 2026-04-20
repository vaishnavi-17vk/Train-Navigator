#pragma once
#include <iostream>
#include <string>

using namespace std;

const int MAX_STATIONS = 50;
const int MAX_LINES    = 8;

struct Station {
    int      id;
    string   name;
    bool     isInterchange;     // true if station is on 2 or more lines
    int      lineIds[MAX_LINES];  // IDs of lines this station appears on
    int      lineCount;
    Station* next;              // forward pointer — circular linked list
    Station* prev;              // backward pointer — circular linked list

    // Default constructor
    Station() {
        id = -1;
        name = "";
        isInterchange = false;
        lineCount = 0;
        next = nullptr;
        prev = nullptr;
        for (int i = 0; i < MAX_LINES; i++) {
            lineIds[i] = -1;
        }
    }
};

struct Line {
    int      id;
    string   name;
    string   color;
    bool     isActive;          // false = line suspended
    bool     isDelayed;
    int      stationCount;
    Station* head;              // head of the circular doubly linked list
    Station* stationArr[MAX_STATIONS];   // flat array copy for O(1) index access

    // Default constructor
    Line() {
        id = -1;
        name = "";
        color = "";
        isActive = true;
        isDelayed = false;
        stationCount = 0;
        head = nullptr;
        for (int i = 0; i < MAX_STATIONS; i++) {
            stationArr[i] = nullptr;
        }
    }
};

struct Network {
    Line*    lines[MAX_LINES];
    int      lineCount;
    Station* allStations[MAX_STATIONS];   // flat lookup table by station ID
    int      totalStations;
    int      adj[MAX_STATIONS][MAX_STATIONS];        // adjacency matrix: 1=edge, 0=none
    int      lineOfEdge[MAX_STATIONS][MAX_STATIONS]; // which line ID owns each edge

    // Default constructor
    Network() {
        lineCount = 0;
        totalStations = 0;
        for (int i = 0; i < MAX_LINES; i++) {
            lines[i] = nullptr;
        }
        for (int i = 0; i < MAX_STATIONS; i++) {
            allStations[i] = nullptr;
            for (int j = 0; j < MAX_STATIONS; j++) {
                adj[i][j] = 0;
                lineOfEdge[i][j] = -1;
            }
        }
    }
};

// Phase 2: Linked List Operations
Station* createStation(int id, const string& name);
void insertStation(Line* line, Station* s, int lineId);
void deleteStation(Line* line, Station* target);
void traverseLine(const Line* line);
void reverseTraverse(const Line* line);
Station* findOnLine(const Line* line, const string& name);
void addLine(Network* net, const string& name, const string& color);

// Phase 3: Network setup & Interchange detection
void buildSampleNetwork(Network* net);
void detectInterchanges(Network* net);
void printAllInterchanges(Network* net);

// Phase 4: BFS Pathfinding
void buildAdjMatrix(Network* net);
bool bfsShortestPath(Network* net, int fromId, int toId, int* path, int& pathLen);
void printRoute(Network* net, int* path, int pathLen);

// Phase 6: Extensions & Memory Management
int countStopsClockwise(Line* line, Station* from, Station* to);
int countStopsAnticlockwise(Line* line, Station* from, Station* to);
int countStopsShortest(Line* line, Station* from, Station* to);
void printNetworkStats(Network* net);
void cleanupNetwork(Network* net);
