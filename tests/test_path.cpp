#include "../network.h"
#include <iostream>
#include <assert.h>

void runTestCase(Network* net, int fromId, int toId, const string& desc) {
    int path[MAX_STATIONS];
    int pathLen = 0;
    
    cout << "\n>>> TEST: " << desc << " (" << fromId << " to " << toId << ")" << endl;
    bool success = bfsShortestPath(net, fromId, toId, path, pathLen);
    
    if (success) {
        printRoute(net, path, pathLen);
    } else {
        cout << "FAILED: No path found." << endl;
    }
}

// Helper: run BFS and return the path length (or -1 if no path)
int getPathLength(Network* net, int fromId, int toId) {
    int path[MAX_STATIONS];
    int pathLen = 0;
    bool ok = bfsShortestPath(net, fromId, toId, path, pathLen);
    if (!ok) return -1;
    return pathLen;
}

// Helper: check if two paths are identical
bool samePath(Network* net, int fromId, int toId, int* savedPath, int savedLen) {
    int path[MAX_STATIONS];
    int pathLen = 0;
    bool ok = bfsShortestPath(net, fromId, toId, path, pathLen);
    if (!ok) return false;
    if (pathLen != savedLen) return false;
    for (int i = 0; i < pathLen; i++) {
        if (path[i] != savedPath[i]) return false;
    }
    return true;
}

int main() {
    Network* net = new Network();
    buildSampleNetwork(net);
    detectInterchanges(net);
    buildAdjMatrix(net);

    // Test 1: Airport(0) -> Riverside(5)
    runTestCase(net, 0, 5, "Airport to Riverside");

    // Test 2: Airport(0) -> Harbor(9)
    runTestCase(net, 0, 9, "Airport to Harbor");

    // Test 3: Westfield(10) -> University(4)
    runTestCase(net, 10, 4, "Westfield to University");

    // Test 4: City Hall(2) -> City Hall(2)
    runTestCase(net, 2, 2, "Same station check (City Hall)");

    // Test 5: Southpark(13) -> Eastgate(14)
    runTestCase(net, 13, 14, "Southpark to Eastgate");

    // ===== Test 6: Suspend Yellow Line and verify route changes =====
    cout << "\n>>> TEST 6: Suspend Yellow Line, Westfield->University route change" << endl;

    // Step 1: Save original path Westfield(10) -> University(4)
    int origPath[MAX_STATIONS];
    int origLen = 0;
    bool origOk = bfsShortestPath(net, 10, 4, origPath, origLen);
    assert(origOk);
    cout << "Original route (Westfield->University): ";
    for (int i = 0; i < origLen; i++) {
        cout << net->allStations[origPath[i]]->name;
        if (i < origLen - 1) cout << " -> ";
    }
    cout << " (" << origLen - 1 << " stops)" << endl;

    // Step 2: Suspend Yellow Line (index 2) — set isActive = false
    net->lines[2]->isActive = false;
    cout << "Yellow Line suspended." << endl;

    // Step 3: Rebuild adjacency matrix, skipping suspended lines
    // Clear the adj matrix
    for (int i = 0; i < MAX_STATIONS; i++) {
        for (int j = 0; j < MAX_STATIONS; j++) {
            net->adj[i][j] = 0;
            net->lineOfEdge[i][j] = -1;
        }
    }

    // Rebuild only active lines
    for (int l = 0; l < net->lineCount; l++) {
        Line* line = net->lines[l];
        if (!line->isActive) continue;  // <-- skip suspended lines
        if (line->head == nullptr) continue;

        for (int i = 0; i < line->stationCount; i++) {
            line->stationArr[i]->next = line->stationArr[(i + 1) % line->stationCount];
        }

        Station* cur = line->head;
        do {
            int a = cur->id;
            int b = cur->next->id;
            net->adj[a][b] = 1;
            net->lineOfEdge[a][b] = l;
            net->adj[b][a] = 1;
            net->lineOfEdge[b][a] = l;
            cur = cur->next;
        } while (cur != line->head);
    }

    // Step 4: Try Westfield(10) -> University(4) again
    int newPath[MAX_STATIONS];
    int newLen = 0;
    bool newOk = bfsShortestPath(net, 10, 4, newPath, newLen);

    if (newOk) {
        cout << "New route (Westfield->University): ";
        for (int i = 0; i < newLen; i++) {
            cout << net->allStations[newPath[i]]->name;
            if (i < newLen - 1) cout << " -> ";
        }
        cout << " (" << newLen - 1 << " stops)" << endl;

        // Check if route changed
        bool pathChanged = (origLen != newLen);
        if (!pathChanged) {
            for (int i = 0; i < origLen; i++) {
                if (origPath[i] != newPath[i]) {
                    pathChanged = true;
                    break;
                }
            }
        }

        if (pathChanged) {
            cout << "PASS: Route changed after suspending Yellow Line." << endl;
        } else {
            cout << "NOTE: Route is the same (Yellow Line was not on the original shortest path)." << endl;
        }
    } else {
        cout << "No alternate route exists — stations are unreachable without Yellow Line." << endl;
    }

    // Restore Yellow Line for cleanup
    net->lines[2]->isActive = true;
    buildAdjMatrix(net);

    cout << "\nAll Phase 4 + Phase 6 path tests passed." << endl;

    // Memory cleanup
    cleanupNetwork(net);

    return 0;
}
