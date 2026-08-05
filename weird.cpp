#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;

class Airport {
public:
    string FAA;
    string name;
    string city;
    string state;
};

class Content {
public:
    int time;
    int cost;
    int miles;
    string airline;
    string flightID;
};

// Global storage
vector<Airport> airports;
unordered_map<string, int> airportIndex; // Maps FAA code to index
vector<vector<Content>> flightGraph; // 2D adjacency matrix

void loadAirports(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error opening airports file!" << endl;
        exit(1);
    }

    string line;
    int index = 0;
    while (getline(file, line)) {
        Airport tmp;
        size_t pos;

        pos = line.find(',');
        tmp.FAA = line.substr(0, pos);
        line = line.substr(pos + 1);

        pos = line.find(',');
        tmp.name = line.substr(0, pos);
        line = line.substr(pos + 1);

        pos = line.find(',');
        tmp.city = line.substr(0, pos);
        line = line.substr(pos + 1);

        tmp.state = line;

        airports.push_back(tmp);
        airportIndex[tmp.FAA] = index++; // Store index for quick lookup
    }

    // Initialize adjacency matrix
    flightGraph.resize(airports.size(), vector<Content>(airports.size()));
}

void loadFlights(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error opening flights file!" << endl;
        exit(1);
    }

    string src, dest, airline, flightID;
    int time, cost, miles;

    while (file >> src >> dest >> time >> miles >> cost >> flightID >> airline) {
        if (airportIndex.find(src) == airportIndex.end() || airportIndex.find(dest) == airportIndex.end()) {
            cout << "Error: Airport not found in map for flight " << src << " -> " << dest << endl;
            continue;
        }

        int srcIndex = airportIndex[src];
        int destIndex = airportIndex[dest];

        Content flight;
        flight.time = time;
        flight.cost = cost;
        flight.miles = miles;
        flight.airline = airline;
        flight.flightID = flightID;

        flightGraph[srcIndex][destIndex] = flight; // Store in adjacency matrix
    }
}

void findAirport() {
    string code;
    cout << "Enter 3-letter FAA code: ";
    cin >> code;

    if (airportIndex.find(code) == airportIndex.end()) {
        cout << "Airport not found!" << endl;
        return;
    }

    int idx = airportIndex[code];
    cout << "Airport Found: " << airports[idx].name << " in " << airports[idx].city << ", " << airports[idx].state << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <airports file> <flights file>\n";
        return 1;
    }

    loadAirports(argv[1]);
    loadFlights(argv[2]);

    int choice;
    do {
        cout << "Enter 1 to search for an airport, 0 to exit: ";
        cin >> choice;
        if (choice == 1) {
            findAirport();
        }
    } while (choice != 0);

    return 0;
}
