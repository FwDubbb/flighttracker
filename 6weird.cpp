#include <queue>
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
    int starttime;
    int landingtime;
    int time;
    int cost;
    int miles;
    string airline;
    string flightID;
};

struct FlightPath {
    int currentAirport;
    vector<Content> flights;
    int totalTime;
    int totalMiles;
};

vector<Airport> airports;
unordered_map<string, int> airportIndex;
vector<vector<vector<Content>>> flightGraph;

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
        tmp.state = line.substr(0, pos);
        line = line.substr(pos + 1);

        tmp.city = line;

        airports.push_back(tmp);
        airportIndex[tmp.FAA] = index++;
    }

    flightGraph.resize(airports.size(), vector<vector<Content>>(airports.size()));
}

void loadFlights(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error opening flights file!" << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        Content flight;
        string src, dest;
        size_t pos;

        pos = line.find(' ');
        src = line.substr(0, pos);
        line = line.substr(pos + 1);

        pos = line.find(' ');
        dest = line.substr(0, pos);
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.starttime = stoi(line.substr(0, pos));
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.landingtime = stoi(line.substr(0, pos));
        line = line.substr(pos + 1);

        if (flight.landingtime >= flight.starttime)
            flight.time = flight.landingtime - flight.starttime;
        else
            flight.time = 2400 - flight.starttime + flight.landingtime;

        pos = line.find(' ');
        flight.miles = stoi(line.substr(0, pos));
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.cost = stoi(line.substr(0, pos));
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.flightID = line.substr(0, pos);
        line = line.substr(pos + 1);

        flight.airline = line;

        if (airportIndex.find(src) == airportIndex.end() || airportIndex.find(dest) == airportIndex.end()) {
            cout << "Warning: Airport not found for flight (" << src << " -> " << dest << ")" << endl;
            continue;
        }

        int srcIndex = airportIndex[src];
        int destIndex = airportIndex[dest];

        flightGraph[srcIndex][destIndex].push_back(flight);
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

bool findFlightPath(string src, string dest) {
    if (airportIndex.find(src) == airportIndex.end() || airportIndex.find(dest) == airportIndex.end()) {
        cout << "One or both airports not found!" << endl;
        return false;
    }

    int startIndex = airportIndex[src];
    int destIndex = airportIndex[dest];

    queue<FlightPath> q;
    vector<bool> visited(airports.size(), false);
	//content has starttime, landing time, time, cost, miles, airline, flightID
	//flightpath has int currentairport, vector <content> flights, int total time, int total miles.
    q.push({startIndex, {}, 0, 0});

    while (!q.empty()) {
        FlightPath fp = q.front();
        q.pop();

        if (fp.currentAirport == destIndex) {
            cout << "Flight path found from " << src << " to " << dest << ":\n";
            int totalTime = 0, totalMiles = 0;

            for (const auto& flight : fp.flights) {
                cout << "Flight ID: " << flight.flightID << ", Airline: " << flight.airline << endl;
                cout << "From: " << src << " -> To: " << dest << endl;
                cout << "Takeoff: " << flight.starttime / 100 << ":00" << endl;
                cout << "Landing: " << flight.landingtime / 100 << ":00" << endl;
                cout << "Duration: " << flight.time / 100 << ":00" << endl;
                cout << "Cost: $" << flight.cost << ", Distance: " << flight.miles << " miles\n";
                cout << "------------------------------------\n";
                totalTime += flight.time;
                totalMiles += flight.miles;
            }

            cout << "Total Travel Time: " << totalTime / 100 << ":00\n";
            cout << "Total Miles Traveled: " << totalMiles << " miles\n";
            return true;
        }

        visited[fp.currentAirport] = true;

        for (int nextAirport = 0; nextAirport < airports.size(); ++nextAirport) {
            if (!flightGraph[fp.currentAirport][nextAirport].empty() && !visited[nextAirport]) {
                for (auto& flight : flightGraph[fp.currentAirport][nextAirport]) {
                    FlightPath newPath = fp;
                    newPath.flights.push_back(flight);
                    newPath.currentAirport = nextAirport;
                    newPath.totalTime += flight.time;
                    newPath.totalMiles += flight.miles;
                    q.push(newPath);
                }
            }
        }
    }

    cout << "No flight path found from " << src << " to " << dest << endl;
    return false;
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
        for (const auto& flight : flightGraph[srcIndex][i]) {
            cout << "Flight to " << airports[i].FAA << " (ID: " << flight.flightID << "), Airline: " << flight.airline << endl;
            cout << "Takeoff: " << flight.starttime / 100 << ":00" << endl;
            cout << "Landing: " << flight.landingtime / 100 << ":00" << endl;
            cout << "Time taken: " << flight.time / 100 << ":00" << endl << endl;
            count++;
        }
    }
    cout << "Total flights departing from " << src << ": " << count << endl;
}
void listFlightsToDestination()
{
    string dest;
    cout << "Enter the 3-letter FAA code of the destination airport: ";
    cin >> dest;

    if ( airportIndex.find(dest) == airportIndex.end() )
	{
        cout << "Airport not found." << endl;
        return;
    }

    int destIndex = airportIndex[dest];

	//int index = 0;
    int count = 0;

    for (int i = 0; i < airports.size(); ++i) {
        for (auto & flight : flightGraph[i][destIndex])
		{
            cout << "Flight from " << airports[i].FAA << " (ID: " << flight.flightID << "), Airline: " << flight.airline << endl;
            cout << "Takeoff: " << flight.starttime / 100 << ":00" << endl;
            cout << "Landing: " << flight.landingtime / 100 << ":00" << endl;
            cout << "Duration: " << flight.time / 100 << ":00" << endl;
            cout << "Cost: $" << flight.cost << ", Distance: " << flight.miles << " miles\n";
            cout << "------------------------------------\n";
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

    // FIX: Iterate over all flights instead of showing only one
    vector<Content>& flights = flightGraph[srcIndex][destIndex];

    if (flights.empty()) {
        cout << "No flights found from " << src << " to " << dest << endl;
        return;
    }

    cout << "Flights from " << src << " to " << dest << ":\n";
    for (const auto& flight : flights) {
        cout << "Flight ID: " << flight.flightID << ", Airline: " << flight.airline << endl;
        cout << "Takeoff: " << flight.starttime / 100 << ":00" << endl;
        cout << "Landing: " << flight.landingtime / 100 << ":00" << endl;
        cout << "Duration: " << flight.time / 100 << ":00" << endl;
        cout << "Cost: $" << flight.cost << ", Distance: " << flight.miles << " miles\n";
        cout << "------------------------------------\n";
    }

    cout << "Total flights from " << src << " to " << dest << ": " << flights.size() << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <airports file> <flights file>\n";
        return 1;
    }

    loadAirports(argv[1]);
    loadFlights(argv[2]);

    int choice = -1;
    while (choice != 0) {
        cout << "\n--------------------------------------------------\n";
        cout << "1) Given airport code, display details\n";
        cout << "2) Airports in a state (list & count)\n";
        cout << "3) Flights leaving from source (list & count)\n";
        cout << "4) Flights arriving at destination (list & count)\n";
        cout << "5) Flights from source to destination (list & count)\n";
		cout << "6) Find a flight path (with stops)\n";
        cout << "0) Quit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) findAirport();
        else if (choice == 2) listAirportsInState();
        else if (choice == 3) listFlightsFromSource();
        else if (choice == 4) listFlightsToDestination();
        else if (choice == 6) {
            string src, dest;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the 3-letter FAA code of the destination airport: ";
            cin >> dest;
            findFlightPath(src, dest);  // Call the findFlightPath function
        }
        else if (choice == 5) listFlightsBetween();
        else if (choice == 0) cout << "Exiting program. Goodbye!" << endl;
        else cout << "Invalid choice. Please try again." << endl;
    }

    return 0;
}
