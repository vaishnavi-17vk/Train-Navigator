#include "network.h"

// FUNCTION 1: createStation
Station* createStation(int id, const string& name) {
    Station* s = new Station();
    s->id = id;
    s->name = name;
    return s;
}

// FUNCTION 2: insertStation
void insertStation(Line* line, Station* s, int lineId) {
    if (!line || !s) return;

    // Record lineId
    if (s->lineCount < MAX_LINES) {
        s->lineIds[s->lineCount] = lineId;
        s->lineCount++;
    }

    if (line->head == nullptr) {
        // Empty list: self-loop
        s->next = s;
        s->prev = s;
        line->head = s;
    } else {
        // Find tail
        Station* tail = line->head->prev;
        
        // Wire in s between tail and head
        tail->next = s;
        s->prev = tail;
        s->next = line->head;
        line->head->prev = s;
    }

    // Store in stationArr
    if (line->stationCount < MAX_STATIONS) {
        line->stationArr[line->stationCount] = s;
        line->stationCount++;
    }
}

// FUNCTION 3: deleteStation
void deleteStation(Line* line, Station* target) {
    if (!line || !line->head || !target) return;

    if (line->stationCount == 1) {
        if (line->head == target) {
            line->head = nullptr;
        } else {
            return; // target not found
        }
    } else {
        // Bypass target
        target->prev->next = target->next;
        target->next->prev = target->prev;

        if (line->head == target) {
            line->head = target->next;
        }
    }

    line->stationCount--;

    // Rebuild stationArr[]
    for (int i = 0; i < MAX_STATIONS; i++) {
        line->stationArr[i] = nullptr;
    }
    
    if (line->head != nullptr) {
        Station* cur = line->head;
        for (int i = 0; i < line->stationCount; i++) {
            line->stationArr[i] = cur;
            cur = cur->next;
        }
    }

    // Only free if not an interchange node
    if (!target->isInterchange) {
        delete target;
    }
}

// FUNCTION 4: traverseLine
void traverseLine(const Line* line) {
    if (!line || line->head == nullptr) {
        cout << "(empty line)" << endl;
        return;
    }

    cout << "[" << line->name << "] ";
    Station* cur = line->head;
    do {
        cout << cur->name;
        if (cur->isInterchange) {
            cout << "(*)";
        }
        
        if (cur->next != line->head) {
            cout << " -> ";
        }
        
        cur = cur->next;
    } while (cur != line->head);

    cout << " -> (loop)" << endl;
}

// FUNCTION 5: reverseTraverse
void reverseTraverse(const Line* line) {
    if (!line || line->head == nullptr) {
        cout << "(empty line)" << endl;
        return;
    }

    Station* cur = line->head->prev; // the tail
    for (int i = 0; i < line->stationCount; i++) {
        cout << cur->name;
        if (i < line->stationCount - 1) {
            cout << " <- ";
        }
        cur = cur->prev;
    }
    cout << endl;
}

// FUNCTION 6: findOnLine
Station* findOnLine(const Line* line, const string& name) {
    if (!line || line->head == nullptr) return nullptr;

    Station* cur = line->head;
    do {
        if (cur->name == name) {
            return cur;
        }
        cur = cur->next;
    } while (cur != line->head);

    return nullptr;
}

// FUNCTION 7: addLine
void addLine(Network* net, const string& name, const string& color) {
    if (!net || net->lineCount >= MAX_LINES) return;

    Line* l = new Line();
    l->id = net->lineCount;
    l->name = name;
    l->color = color;
    
    net->lines[net->lineCount] = l;
    net->lineCount++;
}

// FUNCTION 1: buildSampleNetwork
void buildSampleNetwork(Network* net) {
    if (!net) return;

    // Step A — Add 4 lines
    addLine(net, "Purple Line", "Purple");
    addLine(net, "Green Line",  "Green");
    addLine(net, "Yellow Line", "Yellow");
    addLine(net, "Red Line",    "Red");

    // Step B — Create exactly 15 Station objects
    string names[] = {
        "Airport", "Central Park", "City Hall", "Tech Hub", "University",
        "Riverside", "Northgate", "Market Square", "Stadium", "Harbor",
        "Westfield", "Arts District", "Old Town", "Southpark", "Eastgate"
    };

    // Step C — Register all 15 stations in net->allStations[]
    for (int i = 0; i < 15; i++) {
        Station* s = createStation(i, names[i]);
        net->allStations[i] = s;
        net->totalStations++;
    }

    // Step D — Insert stations into lines
    // Pointers for readability
    Station** s = net->allStations;
    Line** l = net->lines;

    // Purple line (ID 0)
    insertStation(l[0], s[0], 0);  // Airport
    insertStation(l[0], s[1], 0);  // Central Park
    insertStation(l[0], s[2], 0);  // City Hall
    insertStation(l[0], s[3], 0);  // Tech Hub
    insertStation(l[0], s[4], 0);  // University
    insertStation(l[0], s[12], 0); // Old Town
    insertStation(l[0], s[5], 0);  // Riverside

    // Green line (ID 1)
    insertStation(l[1], s[6], 1);  // Northgate
    insertStation(l[1], s[4], 1);  // University
    insertStation(l[1], s[7], 1);  // Market Square
    insertStation(l[1], s[8], 1);  // Stadium
    insertStation(l[1], s[5], 1);  // Riverside
    insertStation(l[1], s[13], 1); // Southpark
    insertStation(l[1], s[9], 1);  // Harbor

    // Yellow line (ID 2)
    insertStation(l[2], s[10], 2); // Westfield
    insertStation(l[2], s[3], 2);  // Tech Hub
    insertStation(l[2], s[7], 2);  // Market Square
    insertStation(l[2], s[2], 2);  // City Hall
    insertStation(l[2], s[11], 2); // Arts District
    insertStation(l[2], s[14], 2); // Eastgate

    // Red line (ID 3)
    insertStation(l[3], s[9], 3);  // Harbor
    insertStation(l[3], s[11], 3); // Arts District
    insertStation(l[3], s[1], 3);  // Central Park
    insertStation(l[3], s[6], 3);  // Northgate
    insertStation(l[3], s[0], 3);  // Airport
    insertStation(l[3], s[8], 3);  // Stadium
    insertStation(l[3], s[10], 3); // Westfield
}

// FUNCTION 2: detectInterchanges
void detectInterchanges(Network* net) {
    if (!net) return;
    for (int i = 0; i < net->totalStations; i++) {
        if (net->allStations[i]->lineCount > 1) {
            net->allStations[i]->isInterchange = true;
        }
    }
}

// FUNCTION 3: printAllInterchanges
void printAllInterchanges(Network* net) {
    if (!net) return;
    cout << "Interchange stations:" << endl;
    for (int i = 0; i < net->totalStations; i++) {
        if (net->allStations[i]->isInterchange) {
            cout << "- " << net->allStations[i]->name << " (Lines: ";
            for (int j = 0; j < net->allStations[i]->lineCount; j++) {
                int lineId = net->allStations[i]->lineIds[j];
                cout << net->lines[lineId]->name;
                if (j < net->allStations[i]->lineCount - 1) cout << ", ";
            }
            cout << ")" << endl;
        }
    }
}

// FUNCTION 1: buildAdjMatrix
void buildAdjMatrix(Network* net) {
    if (!net) return;
    
    // Clear matrix first
    for (int i = 0; i < MAX_STATIONS; i++) {
        for (int j = 0; j < MAX_STATIONS; j++) {
            net->adj[i][j] = 0;
            net->lineOfEdge[i][j] = -1;
            net->weight[i][j] = 0;
        }
    }

    for (int l = 0; l < net->lineCount; l++) {
        Line* line = net->lines[l];
        if (!line->isActive || line->head == nullptr) continue; // Skip suspended lines here or edge by edge? User says "If line is suspended -> skip edge"

        // Restore links for this line snapshot
        for (int i = 0; i < line->stationCount; i++) {
            line->stationArr[i]->next = line->stationArr[(i + 1) % line->stationCount];
        }

        Station* cur = line->head;
        do {
            int a = cur->id;
            int b = cur->next->id;
            
            // Mark edge as existing and record which line owns it
            net->adj[a][b] = 1;
            net->lineOfEdge[a][b] = l;
            
            // Calculate weight
            int w = 2; // Base weight
            if (cur->next->isInterchange) w += 5; // Penalty for entering interchange
            if (line->isDelayed) w += 3;
            
            net->weight[a][b] = w;

            // For undirected graph
            net->adj[b][a] = 1;
            net->lineOfEdge[b][a] = l;
            net->weight[b][a] = w; // Assumes symmetric weight for simplicity

            cur = cur->next;
        } while (cur != line->head);
    }
}

// FUNCTION 2: bfsShortestPath
bool bfsShortestPath(Network* net, int fromId, int toId, int* path, int& pathLen) {
    if (!net || fromId < 0 || toId < 0) return false;
    
    int dist[MAX_STATIONS];
    int prev[MAX_STATIONS];
    bool vis[MAX_STATIONS];
    int queue[MAX_STATIONS * 2];
    int qHead = 0, qTail = 0;

    for (int i = 0; i < MAX_STATIONS; i++) {
        dist[i] = -1;
        prev[i] = -1;
        vis[i] = false;
    }

    // Edge case: same station
    if (fromId == toId) {
        path[0] = fromId;
        pathLen = 1;
        return true;
    }

    dist[fromId] = 0;
    vis[fromId] = true;
    queue[qTail++] = fromId;

    while (qHead < qTail) {
        int u = queue[qHead++];
        
        if (u == toId) break;

        for (int i = 0; i < net->totalStations; i++) {
            int v = net->allStations[i]->id;
            if (net->adj[u][v] > 0 && !vis[v]) {
                // Skip edge if the owning line is suspended
                int eid = net->lineOfEdge[u][v];
                if (eid != -1 && !net->lines[eid]->isActive) {
                    continue; // line suspended — skip this edge
                }
                vis[v] = true;
                dist[v] = dist[u] + 1;
                prev[v] = u;
                queue[qTail++] = v;
            }
        }
    }

    if (dist[toId] == -1) return false;

    // Reconstruct path
    pathLen = 0;
    int c = toId;
    while (c != -1) {
        path[pathLen++] = c;
        c = prev[c];
    }

    // Reverse the path using swap
    for (int i = 0; i < pathLen / 2; i++) {
        int temp = path[i];
        path[i] = path[pathLen - 1 - i];
        path[pathLen - 1 - i] = temp;
    }

    return true;
}

// Phase 4: Dijkstra's Algorithm (Weighted Pathfinding)
bool dijkstraShortestPath(Network* net, int src, int dest, int* path, int& pathLen, int& totalCost) {
    if (!net || src < 0 || dest < 0 || src >= MAX_STATIONS || dest >= MAX_STATIONS) return false;

    int dist[MAX_STATIONS];
    bool vis[MAX_STATIONS];
    int prev[MAX_STATIONS];

    for (int i = 0; i < MAX_STATIONS; i++) {
        dist[i] = 1e9; // infinity
        vis[i] = false;
        prev[i] = -1;
    }

    dist[src] = 0;

    for (int i = 0; i < net->totalStations; i++) {
        // Find unvisited node with minimum distance
        int u = -1;
        int minDist = 1e9;
        
        for (int j = 0; j < net->totalStations; j++) {
            int sid = net->allStations[j]->id;
            if (!vis[sid] && dist[sid] < minDist) {
                minDist = dist[sid];
                u = sid;
            }
        }

        if (u == -1 || u == dest) break;
        vis[u] = true;

        // Relax neighbors
        for (int v = 0; v < MAX_STATIONS; v++) {
            if (net->adj[u][v] > 0 && !vis[v]) {
                int weight = net->weight[u][v];
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[dest] == 1e9) return false;

    totalCost = dist[dest];
    
    // Reconstruct path
    pathLen = 0;
    int c = dest;
    while (c != -1) {
        path[pathLen++] = c;
        c = prev[c];
    }

    // Reverse path
    for (int i = 0; i < pathLen / 2; i++) {
        int tmp = path[i];
        path[i] = path[pathLen - 1 - i];
        path[pathLen - 1 - i] = tmp;
    }

    return true;
}

// FUNCTION 3: printRoute
void printRoute(Network* net, int* path, int pathLen) {
    if (pathLen == 0) {
        cout << "No route found." << endl;
        return;
    }

    cout << "\nRoute found:" << endl;
    int changes = 0;

    for (int i = 0; i < pathLen; i++) {
        Station* s = net->allStations[path[i]];
        cout << "  " << s->name;
        if (s->isInterchange) cout << " [interchange]";

        if (i < pathLen - 1) {
            int a = path[i];
            int b = path[i + 1];
            int lineB = net->lineOfEdge[a][b];
            int lineA = (i > 0) ? net->lineOfEdge[path[i-1]][a] : lineB;

            if (lineA != lineB && lineA != -1 && lineB != -1) {
                cout << "\n    >> Change to " << net->lines[lineB]->name;
                changes++;
            }
            cout << " ->" << endl;
        } else {
            cout << endl;
        }
    }

    int stops = pathLen - 1;
    int estTime = stops * 2 + changes * 3;
    cout << "------------------------------------------------" << endl;
    cout << "Total stops: " << stops << "  |  Line changes: " << changes 
         << "  |  Est. time: " << estTime << " min" << endl;
}

// FUNCTION 1: countStopsClockwise
int countStopsClockwise(Line* line, Station* from, Station* to) {
    if (!line || !from || !to) return -1;
    int count = 0;
    Station* cur = from;
    do {
        if (cur == to) return count;
        cur = cur->next;
        count++;
    } while (cur != from);
    return -1;
}

// FUNCTION 2: countStopsAnticlockwise
int countStopsAnticlockwise(Line* line, Station* from, Station* to) {
    if (!line || !from || !to) return -1;
    int count = 0;
    Station* cur = from;
    do {
        if (cur == to) return count;
        cur = cur->prev;
        count++;
    } while (cur != from);
    return -1;
}

// FUNCTION 3: countStopsShortest
int countStopsShortest(Line* line, Station* from, Station* to) {
    int cw = countStopsClockwise(line, from, to);
    int acw = countStopsAnticlockwise(line, from, to);

    if (cw == -1) return acw;
    if (acw == -1) return cw;

    if (cw <= acw) {
        cout << "Direction: Clockwise is shorter (or equal)." << endl;
        return cw;
    } else {
        cout << "Direction: Anticlockwise is shorter." << endl;
        return acw;
    }
}

// FUNCTION 4: printNetworkStats
void printNetworkStats(Network* net) {
    if (!net) return;
    int interCount = 0;
    for (int i = 0; i < net->totalStations; i++) {
        if (net->allStations[i]->isInterchange) interCount++;
    }

    int edgeCount = 0;
    for (int i = 0; i < MAX_STATIONS; i++) {
        for (int j = 0; j < MAX_STATIONS; j++) {
            if (net->adj[i][j] > 0) edgeCount++;
        }
    }

    cout << "\n--- NETWORK SUMMARY REPORT ---" << endl;
    cout << "Total Lines: " << net->lineCount << endl;
    cout << "Total Stations: " << net->totalStations << " (Interchanges: " << interCount << ")" << endl;
    cout << "Total Network Edges: " << edgeCount / 2 << endl;
    cout << "\nLine Details:" << endl;
    for (int i = 0; i < net->lineCount; i++) {
        Line* l = net->lines[i];
        cout << "- " << l->name << ": " << l->stationCount << " stations | Status: ";
        if (!l->isActive) cout << "SUSPENDED";
        else if (l->isDelayed) cout << "DELAYED";
        else cout << "ACTIVE";
        cout << endl;
    }
}

// FUNCTION 5: cleanupNetwork
/* 
Two-Step Cleanup Strategy:
Interchange nodes are shared by multiple Line objects. If we delete all stations 
while walking each line, we would hit a "double-free" error on interchanges.
Step 1: Delete only unique (non-interchange) stations during line walking.
Step 2: Use the master allStations table to delete all interchange nodes exactly once.
*/
void cleanupNetwork(Network* net) {
    if (!net) return;

    // Step 1: Delete unique stations and Line objects
    for (int i = 0; i < net->lineCount; i++) {
        Line* l = net->lines[i];
        if (!l) continue;

        if (l->head != nullptr) {
            Station* cur = l->head;
            int count = l->stationCount;
            for (int j = 0; j < count; j++) {
                Station* nx = cur->next;
                if (!cur->isInterchange) {
                    delete cur;
                }
                cur = nx;
            }
        }
        delete l;
    }

    // Step 2: Delete interchange stations
    for (int i = 0; i < net->totalStations; i++) {
        if (net->allStations[i] && net->allStations[i]->isInterchange) {
            delete net->allStations[i];
            net->allStations[i] = nullptr;
        }
    }

    // Step 3: Delete network
    delete net;
    cout << "Memory cleanup complete." << endl;
}
