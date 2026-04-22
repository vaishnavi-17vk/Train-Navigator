#include "network.h"
#include <iostream>
#include <string>

using namespace std;

// FUNCTION 1: getStationId
int getStationId(Network* net, const string& name) {
    if (!net) return -1;
    for (int i = 0; i < net->totalStations; i++) {
        if (net->allStations[i]->name == name) {
            return net->allStations[i]->id;
        }
    }
    return -1;
}

// FUNCTION 2: getLineIndex
int getLineIndex(Network* net, const string& name) {
    if (!net) return -1;
    for (int i = 0; i < net->lineCount; i++) {
        if (net->lines[i]->name == name) {
            return i;
        }
    }
    return -1;
}

// FUNCTION 3: printMenu
void printMenu() {
    cout << "\n==============================" << endl;
    cout << " Train Route Network Navigator" << endl;
    cout << "==============================" << endl;
    cout << " 1. Show all lines and stations" << endl;
    cout << " 2. Find route between two stations" << endl;
    cout << " 3. Show all interchange stations" << endl;
    cout << " 4. Insert a new station into a line" << endl;
    cout << " 5. Delete a station from a line" << endl;
    cout << " 6. Toggle line status (active/delayed/suspended)" << endl;
    cout << " 7. Count stops between two stations (same line)" << endl;
    cout << " 8. Show network statistics" << endl;
    cout << " 9. Find fastest route (Dijkstra)" << endl;
    cout << " 0. Exit" << endl;
    cout << "Enter choice: ";
}

int main() {
    Network* net = new Network();
    buildSampleNetwork(net);
    detectInterchanges(net);
    buildAdjMatrix(net);

    cout << "Network loaded. 4 lines, 15 stations ready." << endl;

    int choice;
    do {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore(); // Consume newline

        switch (choice) {
            case 1: {
                cout << "\n--- ALL TRAIN LINES ---" << endl;
                for (int i = 0; i < net->lineCount; i++) {
                    Line* l = net->lines[i];
                    traverseLine(l);
                    cout << "   Stations: " << l->stationCount << " | Status: ";
                    if (!l->isActive) cout << "SUSPENDED";
                    else if (l->isDelayed) cout << "DELAYED";
                    else cout << "ACTIVE";
                    cout << endl;
                }
                break;
            }
            case 2: {
                string from, to;
                cout << "From station: "; getline(cin, from);
                cout << "To station  : "; getline(cin, to);

                int startId = getStationId(net, from);
                int endId = getStationId(net, to);

                if (startId == -1 || endId == -1) {
                    cout << "Station not found. Check spelling." << endl;
                } else if (startId == endId) {
                    cout << "You are already at this station." << endl;
                } else {
                    int path[MAX_STATIONS];
                    int pathLen = 0;
                    if (bfsShortestPath(net, startId, endId, path, pathLen)) {
                        printRoute(net, path, pathLen);
                    } else {
                        cout << "No route possible." << endl;
                    }
                }
                break;
            }
            case 3: {
                cout << "\nInterchange stations:" << endl;
                for (int i = 0; i < net->totalStations; i++) {
                    if (net->allStations[i]->isInterchange) {
                        cout << "- " << net->allStations[i]->name 
                             << " (Connects " << net->allStations[i]->lineCount << " lines)" << endl;
                    }
                }
                break;
            }
            case 4: {
                string lineName, statName;
                cout << "Enter line name: "; getline(cin, lineName);
                cout << "New station name: "; getline(cin, statName);

                int lIdx = getLineIndex(net, lineName);
                if (lIdx == -1) {
                    cout << "Line not found." << endl;
                } else {
                    Station* s = createStation(net->totalStations, statName);
                    net->allStations[net->totalStations] = s;
                    net->totalStations++;
                    insertStation(net->lines[lIdx], s, lIdx);
                    buildAdjMatrix(net);
                    cout << "Station inserted successfully." << endl;
                }
                break;
            }
            case 5: {
                string statName, lineName;
                cout << "Station name to delete: "; getline(cin, statName);
                cout << "From line name: "; getline(cin, lineName);

                int lIdx = getLineIndex(net, lineName);
                if (lIdx == -1) {
                    cout << "Line not found." << endl;
                } else {
                    Station* target = findOnLine(net->lines[lIdx], statName);
                    if (!target) {
                        cout << "Station not found on this line." << endl;
                    } else {
                        deleteStation(net->lines[lIdx], target);
                        buildAdjMatrix(net);
                        cout << "Station deleted." << endl;
                    }
                }
                break;
            }
            case 6: {
                string lineName, status;
                cout << "Enter line name: "; getline(cin, lineName);
                cout << "Enter new status (active/delayed/suspended): "; getline(cin, status);

                int lIdx = getLineIndex(net, lineName);
                if (lIdx == -1) {
                    cout << "Line not found." << endl;
                } else {
                    Line* l = net->lines[lIdx];
                    if (status == "active") {
                        l->isActive = true;
                        l->isDelayed = false;
                    } else if (status == "delayed") {
                        l->isActive = true;
                        l->isDelayed = true;
                    } else if (status == "suspended") {
                        l->isActive = false;
                        l->isDelayed = false;
                    }
                    cout << "Line status updated." << endl;
                }
                break;
            }
            case 7: {
                string lineName, start, end;
                cout << "Enter line name: "; getline(cin, lineName);
                cout << "Station A: "; getline(cin, start);
                cout << "Station B: "; getline(cin, end);

                int lIdx = getLineIndex(net, lineName);
                if (lIdx == -1) {
                    cout << "Line not found." << endl;
                } else {
                    Line* l = net->lines[lIdx];
                    Station* sA = findOnLine(l, start);
                    Station* sB = findOnLine(l, end);

                    if (!sA || !sB) {
                        cout << "Both stations must exist on the same line." << endl;
                    } else if (sA == sB) {
                        cout << "Stops between same station: 0" << endl;
                    } else {
                        // Clockwise (A -> B)
                        int cw = 0;
                        Station* cur = sA;
                        do { cur = cur->next; cw++; } while (cur != sB && cur != sA);
                        
                        // Anticlockwise (B -> A)
                        int acw = 0;
                        cur = sB;
                        do { cur = cur->next; acw++; } while (cur != sA && cur != sB);

                        cout << "Direction A -> B: " << cw << " stops" << endl;
                        cout << "Direction B -> A: " << acw << " stops" << endl;
                        cout << "Shorter path: " << (cw < acw ? cw : acw) << " stops" << endl;
                    }
                }
                break;
            }
            case 9: {
                string from, to;
                cout << "From station: "; getline(cin, from);
                cout << "To station  : "; getline(cin, to);

                int startId = getStationId(net, from);
                int endId = getStationId(net, to);

                if (startId == -1 || endId == -1) {
                    cout << "Station not found." << endl;
                } else {
                    int path[MAX_STATIONS];
                    int pathLen = 0, totalCost = 0;
                    buildAdjMatrix(net); // Ensure weights are up to date
                    if (dijkstraShortestPath(net, startId, endId, path, pathLen, totalCost)) {
                        cout << "\nFastest Route (Dijkstra):" << endl;
                        for (int i = 0; i < pathLen; i++) {
                            cout << net->allStations[path[i]]->name;
                            if (i < pathLen - 1) cout << " -> ";
                        }
                        cout << "\nTotal Cost: " << totalCost << endl;
                    } else {
                        cout << "No route possible." << endl;
                    }
                }
                break;
            }
            case 0:
                cout << "Goodbye." << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    } while (choice != 0);

    cleanupNetwork(net);
    return 0;
}
