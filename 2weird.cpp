#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

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

vector<Airport> airports;
unordered_map<string, int> airportIndex;
vector<vector<Content>> flightGraph;

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
        airportIndex[tmp.FAA] = index++;
    }

    flightGraph.resize(airports.size(), vector<Content>(airports.size()));
}

void loadFlights(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error opening flights file!" << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string src, dest, airline, flightID;
        int time, cost, miles;

        if (!(iss >> src >> dest >> time >> miles >> cost >> flightID >> airline)) {
            cout << "Error reading flight data: " << line << endl;
            continue;
        }

        if (airportIndex.find(src) == airportIndex.end() || airportIndex.find(dest) == airportIndex.end()) {
            cout << "Warning: Airport not found for flight (" << src << " -> " << dest << ")" << endl;
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

        flightGraph[srcIndex][destIndex] = flight;
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

void listAirportsInState() {
    string state;
    cout << "Enter the state abbreviation (e.g., 'CA' for California): ";
    cin >> state;

    int count = 0;
    for (const auto& airport : airports) {
        if (airport.state == state) {
            cout << airport.FAA << " - " << airport.name << " in " << airport.city << endl;
            count++;
        }
    }
    cout << "Total airports in " << state << ": " << count << endl;
}

void listFlightsFromSource() {
    string src;
    cout << "Enter the 3-letter FAA code of the source airport: ";
    cin >> src;

    if (airportIndex.find(src) == airportIndex.end()) {
        cout << "Airport not found!" << endl;
        return;
    }

    int srcIndex = airportIndex[src];
    int count = 0;
    for (int i = 0; i < airports.size(); ++i) {
        if (flightGraph[srcIndex][i].time > 0) {  
            cout << "Flight to " << airports[i].FAA << " (ID: " << flightGraph[srcIndex][i].flightID << "), Airline: " << flightGraph[srcIndex][i].airline << endl;
            count++;
        }
    }
    cout << "Total flights departing from " << src << ": " << count << endl;
}

void listFlightsToDestination() {
    string dest;
    cout << "Enter the 3-letter FAA code of the destination airport: ";
    cin >> dest;

    if (airportIndex.find(dest) == airportIndex.end()) {
        cout << "Airport not found!" << endl;
        return;
    }

    int destIndex = airportIndex[dest];
    int count = 0;
    for (int i = 0; i < airports.size(); ++i) {
        if (flightGraph[i][destIndex].time > 0) {
            cout << "Flight from " << airports[i].FAA << " (ID: " << flightGraph[i][destIndex].flightID << "), Airline: " << flightGraph[i][destIndex].airline << endl;
            count++;
        }
    }
    cout << "Total flights arriving at " << dest << ": " << count << endl;
}

void listFlightsBetween() {
    string src, dest;
    cout << "Enter the 3-letter FAA code of the source airport: ";
    cin >> src;
    cout << "Enter the 3-letter FAA code of the destination airport: ";
    cin >> dest;

    if (airportIndex.find(src) == airportIndex.end() || airportIndex.find(dest) == airportIndex.end()) {
        cout << "One or both airports not found!" << endl;
        return;
    }

    int srcIndex = airportIndex[src];
    int destIndex = airportIndex[dest];
    int count = 0;

    if (flightGraph[srcIndex][destIndex].time > 0) {
        cout << "Flight from " << src << " to " << dest << " (ID: " << flightGraph[srcIndex][destIndex].flightID << "), Airline: " << flightGraph[srcIndex][destIndex].airline << endl;
        count++;
    }

    cout << "Total flights from " << src << " to " << dest << ": " << count << endl;
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
        cout << "\n--------------------------------------------------\n";
        cout << "Look up\n";
        cout << "1) Given airport code, display details\n";
        cout << "2) Airports in a state (list & count)\n";
        cout << "3) Flights leaving from source (list & count)\n";
        cout << "4) Flights arriving at destination (list & count)\n";
        cout << "5) Flights from source to destination (list & count)\n";
        cout << "0) Quit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                findAirport();
                break;
            case 2:
                listAirportsInState();
                break;
            case 3:
                listFlightsFromSource();
                break;
            case 4:
                listFlightsToDestination();
                break;
            case 5:
                listFlightsBetween();
                break;
            case 0:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    return 0;
}
