#include "../network.h"
#include <iostream>
#include <assert.h>

int main() {
    Network* net = new Network();

    // 1. Create a Network and call addLine() for one test line
    addLine(net, "Purple Line", "Purple");
    Line* line = net->lines[0];

    // 2. Insert 4 stations: "Alpha", "Beta", "Gamma", "Delta"
    Station* s1 = createStation(1, "Alpha");
    Station* s2 = createStation(2, "Beta");
    Station* s3 = createStation(3, "Gamma");
    Station* s4 = createStation(4, "Delta");

    insertStation(line, s1, line->id);
    insertStation(line, s2, line->id);
    insertStation(line, s3, line->id);
    insertStation(line, s4, line->id);

    // 3. Call traverseLine()
    cout << "Traversing after insertion:" << endl;
    traverseLine(line);

    // 4. Assert stationCount == 4
    assert(line->stationCount == 4);

    // 5. Assert head->prev->name == "Delta" (tail check)
    assert(line->head->prev->name == "Delta");

    // 6. Assert head->prev->next == head (circular check)
    assert(line->head->prev->next == line->head);

    // 7. Delete "Beta", traverse again
    Station* target = findOnLine(line, "Beta");
    assert(target != nullptr);
    deleteStation(line, target);

    cout << "\nTraversing after deleting Beta:" << endl;
    traverseLine(line);
    assert(line->stationCount == 3);

    // 8. Call reverseTraverse()
    cout << "\nReverse traversing:" << endl;
    reverseTraverse(line);

    // 9. Prints "All Phase 2 tests passed."
    cout << "\nAll Phase 2 tests passed." << endl;

    // ===== Phase 6 Extension Tests =====

    // Test countStopsClockwise for Alpha -> Delta
    // After deleting Beta, the line is: Alpha -> Gamma -> Delta -> (loop)
    // Clockwise from Alpha to Delta: Alpha->Gamma(1), Gamma->Delta(2) = 2 stops
    int cwStops = countStopsClockwise(line, s1, s4);
    cout << "\ncountStopsClockwise(Alpha -> Delta): " << cwStops << endl;
    assert(cwStops == 2);

    // Test countStopsAnticlockwise for Alpha -> Delta
    // Anticlockwise from Alpha: Alpha->Delta(1) = 1 stop
    int acwStops = countStopsAnticlockwise(line, s1, s4);
    cout << "countStopsAnticlockwise(Alpha -> Delta): " << acwStops << endl;
    assert(acwStops == 1);

    // Test countStopsShortest — should pick anticlockwise (1 < 2)
    cout << "countStopsShortest(Alpha -> Delta): ";
    int shortest = countStopsShortest(line, s1, s4);
    cout << "Result: " << shortest << endl;
    assert(shortest == 1);

    // Test countStopsClockwise for station not on line — returns -1
    Station* ghost = createStation(99, "Ghost");
    int ghostResult = countStopsClockwise(line, s1, ghost);
    assert(ghostResult == -1);
    cout << "countStopsClockwise(Alpha -> Ghost): " << ghostResult << " (expected -1)" << endl;
    delete ghost;

    cout << "\nAll Phase 6 extension tests passed." << endl;

    // Memory cleanup — must not crash
    // We need to clean up manually since we have a simple network
    // Delete remaining stations (s1, s3, s4 — s2 was already deleted by deleteStation)
    // and the line, then the network
    delete s1;
    delete s3;
    delete s4;
    delete line;
    delete net;
    cout << "Memory cleanup complete — no crash." << endl;

    return 0;
}
