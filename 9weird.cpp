#include <unordered_set>
#include <climits>
#include <queue>
#include <stack>
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
	string start;
	string end;
    int starttime;
    int landingtime;
    int time;
    int cost;
    int miles;
    string airline;
    string flightID;
	int weight = 1;
};

struct FlightPath {
    int currentAirport;
    vector<Content> flights;
    int totalTime;
    int totalMiles;
};

struct BFSState {
    int airportIndex;
    int currentTime;
    int flightsTaken;
	int totalFare;

    BFSState(int airportIndex, int currentTime, int flightsTaken, int totalFare)
        : airportIndex(airportIndex), currentTime(currentTime), flightsTaken(flightsTaken), totalFare(totalFare) {}
};


struct Timer {
    int airportIndex;
    int currentTime;
	int totalFlightTime;

    Timer(int airportIndex, int currentTime, int fTime)
        : airportIndex(airportIndex), currentTime(currentTime), totalFlightTime(fTime) {}
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
//        string src, dest;
        size_t pos;

        pos = line.find(' ');
        flight.start = line.substr(0, pos);
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.end = line.substr(0, pos);
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
        flight.cost = stoi(line.substr(0, pos));
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.miles = stoi(line.substr(0, pos));
        line = line.substr(pos + 1);

        pos = line.find(' ');
        flight.flightID = line.substr(0, pos);
        line = line.substr(pos + 1);

        flight.airline = line;

        if (airportIndex.find(flight.start) == airportIndex.end() || airportIndex.find(flight.end) == airportIndex.end()) {
            cout << "Warning: Airport not found for flight (" << flight.start << " -> " << flight.end << ")" << endl;
            continue;
        }

        int srcIndex = airportIndex[flight.start];
        int destIndex = airportIndex[flight.end];

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
                cout << "From: " << flight.start << " -> To: " << flight.end << endl;
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
            cout << "Miles: " << flight.miles  << " miles." << endl;
            cout << "Time taken: " << flight.time / 100 << ":00" << endl;
			cout << "Cost : $" <<flight.cost << ".00" << endl << endl;
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

void dijkstra(string start, string dest, int type) {
    int nodes = airports.size();
    int begin = airportIndex[start];
    int end = airportIndex[dest];

    vector<int> dist(nodes, 9999);
    vector<bool> lockedin(nodes, false);
    vector<int> previous(nodes, -1);

    dist[begin] = 0;

    for (int i = 0; i < nodes; i++) {
        int minDistance = 9999;
        int currentNode = -1;

        for (int j = 0; j < nodes; j++) {
            if (!lockedin[j] && dist[j] < minDistance) {
                minDistance = dist[j];
                currentNode = j;
            }
        }

        if (currentNode == -1) break;

        lockedin[currentNode] = true;

        for (int neighbor = 0; neighbor < nodes; neighbor++) {
            if (!flightGraph[currentNode][neighbor].empty() && !lockedin[neighbor]) {
                int minFlightMetric = 9999;
                
                // Depending on 'type', choose the appropriate metric (cost, miles, or weight)
                for (const auto& flight : flightGraph[currentNode][neighbor]) {
                    if (type == 1) { // For cost
                        if (flight.cost < minFlightMetric) {
                            minFlightMetric = flight.cost;
                        }
                    } else if (type == 2) { // For miles
                        if (flight.miles < minFlightMetric) {
                            minFlightMetric = flight.miles;
                        }
                    } else if (type == 3) { // For weight
                        if (flight.weight < minFlightMetric) {
                            minFlightMetric = flight.weight;
                        }
                    }
                }

                if (dist[currentNode] + minFlightMetric < dist[neighbor]) {
                    dist[neighbor] = dist[currentNode] + minFlightMetric;
                    previous[neighbor] = currentNode;
                }
            }
        }
    }

    if (dist[end] == 9999) {
        cout << "No path from " << start << " to " << dest << endl;
    } else {
        // Depending on 'type', print the appropriate result
        if (type == 1) {
            cout << "Minimum cost from " << start << " to " << dest << ": $" << dist[end] << endl;
        } else if (type == 2) {
            cout << "Minimum miles from " << start << " to " << dest << ": " << dist[end] << " miles." << endl;
        } else if (type == 3) {
            cout << "Lowest hops from " << start << " to " << dest << ": " << dist[end] - 1 << " hops." << endl;
        }
    }

    if (dist[end] != 9999) {
        stack<int> path;
        int at = end;

        while (at != -1) {
            path.push(at);
            at = previous[at];
        }

        cout << "Path taken: " << endl;
        int count = 1;
        while (!path.empty()) {
            int index = path.top();
            path.pop();
            cout << count << "). " << airports[index].name << ", " << airports[index].FAA << endl;
            count++;
        }

        cout << endl;
    }
}


void reachableDestinations(string sourceFAA, int arrivalTime, int maxValue, int type) {
    if (airportIndex.find(sourceFAA) == airportIndex.end()) {
        cout << "Source airport not found!" << endl;
        return;
    }

    int srcIndex = airportIndex[sourceFAA];

    // Queue holds: current airport, current time, number of flights, total fare, total time
    queue<BFSState> q;
    vector<bool> reachable(airports.size(), false);  // Track reachable airports

    // Start BFS with the source airport, arrival time, 0 flights, 0 fare, 0 time
    q.push(BFSState(srcIndex, arrivalTime, 0, 0));

    while (!q.empty()) {
        BFSState state = q.front();
        q.pop();

        // Apply the constraint based on the type
        if (type == 1 && state.flightsTaken > maxValue) continue; // Type 1: max flights
        if (type == 2 && state.totalFare > maxValue) continue;    // Type 2: max fare

        // Mark as reachable if it's not the source airport
        if (state.airportIndex != srcIndex) {
            reachable[state.airportIndex] = true;
        }

        // Explore all outgoing flights from current airport
        for (int next = 0; next < airports.size(); ++next) {
            for (const auto& flight : flightGraph[state.airportIndex][next]) {
                // Check if the flight can be taken after the current time
                if (flight.starttime >= state.currentTime) {
                    // Update totalFare and totalTime based on flight data
                    int newFare = state.totalFare + flight.cost;

                    // Push new state to the queue depending on the selected type
//					if (newTime >= 0)
                        q.push(BFSState(next, flight.landingtime, state.flightsTaken + 1, newFare));
                }
            }
        }
    }

    // Output the reachable destinations
    bool foundReachable = false;
    for (int i = 0; i < airports.size(); ++i) {
        if (reachable[i] && i != srcIndex) {  // Skip the source airport
            cout << "- " << airports[i].FAA << ": " << airports[i].name << " in " << airports[i].city << ", " << airports[i].state << endl;
            foundReachable = true;
        }
    }

    if (!foundReachable) {
        cout << "No destinations reachable from " << sourceFAA << " within " << maxValue << " (based on selected type)." << endl;
    }
}



void reachableDestinationsHours(string sourceFAA, int arrivalTime, int maxValue) {
    if (airportIndex.find(sourceFAA) == airportIndex.end()) {
        cout << "Source airport not found!" << endl;
        return;
    }

    int srcIndex = airportIndex[sourceFAA];

    queue<Timer> q;
    vector<bool> reachable(airports.size(), false);
    vector<int> minFlightTime(airports.size(), INT_MAX); // Optional optimization

    q.push(Timer(srcIndex, arrivalTime, 0));
    minFlightTime[srcIndex] = 0;

    while (!q.empty()) {
        Timer state = q.front();
        q.pop();

        if (state.airportIndex != srcIndex && state.totalFlightTime <= maxValue) {
            reachable[state.airportIndex] = true;
        }

        for (int next = 0; next < airports.size(); ++next) {
            for (const Content& flight : flightGraph[state.airportIndex][next]) {
                if (flight.starttime >= state.currentTime) {
					int waitTime = (flight.starttime - state.currentTime)/100;
                    int newTotalFlightTime = state.totalFlightTime + flight.time + waitTime;
					cout <<" After landing at " <<flight.end << " , new time is " << newTotalFlightTime << endl;

                    if (newTotalFlightTime <= maxValue && newTotalFlightTime < minFlightTime[next]) {
                        minFlightTime[next] = newTotalFlightTime;
                        q.push(Timer(next, flight.landingtime, newTotalFlightTime));
                    }
                }
            }
        }
    }

    cout << "Reachable airports within " << maxValue << " flight hours:" << endl;
    for (int i = 0; i < airports.size(); ++i) {
        if (i != srcIndex && minFlightTime[i] <= maxValue) {
            cout << airports[i].FAA << " - " << airports[i].name << endl;
        }
    }
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
		cout << "7) Find minimum cost from source to destination\n";
		cout << "8) Find minimum miles from source to destination\n";
		cout << "9) Find fewest hops from source to destination\n";
		cout << "10) From source, arriving at time T, destinations reachable in F flights\n";
		cout << "11) From source, arriving at time T, destinations reachable under M total fare\n";
		cout << "12) From source, arriving at time T, destinations reachable under H hours\n";
        cout << "0) Quit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) findAirport();
        else if (choice == 2) listAirportsInState();
        else if (choice == 3) listFlightsFromSource();
        else if (choice == 4) listFlightsToDestination();
        else if (choice == 5) listFlightsBetween();
        else if (choice == 6) {
            string src, dest;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the 3-letter FAA code of the destination airport: ";
            cin >> dest;
            findFlightPath(src, dest);  // Call the findFlightPath function
        }
		else if (choice == 7 )
		{
			string src, dest;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the 3-letter FAA code of the destination airport: ";
            cin >> dest;
            dijkstra(src, dest,1);  // Call the findFlightPath function

		}
		else if (choice == 8 )
		{
			string src, dest;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the 3-letter FAA code of the destination airport: ";
            cin >> dest;
            dijkstra(src, dest,2);  // Call the findFlightPath function

		}
		else if (choice == 9 )
		{
			string src, dest;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the 3-letter FAA code of the destination airport: ";
            cin >> dest;
            dijkstra(src, dest,3);  // Call the findFlightPath function

		}

		else if (choice == 10 )
		{
			string src;
			int time, flights;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the starting time: ";
            cin >> time;
            cout << "Enter the max amount of flights: ";
            cin >> flights;
            reachableDestinations(src, time, flights, 1);  // Call the findFlightPath function

		}

		else if (choice == 11 )
		{
			string src;
			int time, cost;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the starting time: ";
            cin >> time;
            cout << "Enter the max amount in cost: ";
            cin >> cost;
            reachableDestinations(src, time, cost, 2);  // Call the findFlightPath function

		}

		else if (choice == 12 )
		{
			string src;
			int time, hours;
            cout << "Enter the 3-letter FAA code of the source airport: ";
            cin >> src;
            cout << "Enter the starting time: ";
            cin >> time;
            cout << "Enter the max amount of hours: ";
            cin >> hours;
            reachableDestinationsHours(src, time, hours);  // Call the findFlightPath function

		}

        else if (choice == 0) cout << "Exiting program. Goodbye!" << endl;
        else cout << "Invalid choice. Please try again." << endl;
    }

    return 0;
}
