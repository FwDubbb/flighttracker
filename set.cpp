#include <cfloat>
#include <unordered_set>
#include <climits>
#include <queue>
#include <stack>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>


using namespace std;

unordered_map<string, int> flights;

class Set {
private:
    vector<int> elements;

public:
    Set() {}



    Set(const vector<int>& x)
        {
        for (int item : x) {
            if (find(elements.begin(), elements.end(), item) == elements.end())
                elements.push_back(item);
        }
    }



    Set& operator += (int value)
        {
        if (!( *this ^ value))
            elements.push_back(value);
        return *this;
    }



    bool operator ^ (int value) const
        {
        return find(elements.begin(), elements.end(), value) != elements.end();
    }

    Set operator * (const Set& other) const
        {
        Set result;
        for (int item : elements)
                {
            if (other ^ item)
                        {
                result += item;
            }
        }
        return result;
    }

    Set operator + (const Set& other) const
        {
        Set result = *this;
        for (int item : other.elements)
                {
            result += item;
        }
        return result;
    }

    Set operator - (const Set& other) const
        {
        Set result;
        for (int item : elements)
                {
            if (!(other ^ item))
                        {
                result += item;
            }
        }
        return result;
    }

    bool operator < (const Set& other) const
        {
        bool isSubset = true;
        for (int item : this->elements)
                {
                if (!(other ^ item))
                        {
                isSubset = false;
                break;
                }
        }

        bool isSmaller = false;
            if (this->elements.size() < other.elements.size())
                {
             isSmaller = true;
            }

            if (isSubset && isSmaller)
                {
                return true;
                }
                else
                {
        return false;
        }

}



    bool operator <= (const Set& other) const
        {
        for (int item : elements)
                {
            if (!(other ^ item))
                        {
                return false;
            }
        }
        return true;
    }

    bool operator ! () const
        {
        return elements.empty();
    }

    int size() const
        {
        return elements.size();
    }

    friend ostream& operator<<(ostream& out, const Set& s)
        {
        out << "List: ";
        for (int item : s.elements)
                {
            out << item << " ";
        }
        return out;
    }
};



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



//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


void buildMSTFromAirports(vector<int>& subsetAirports) {
    int N = subsetAirports.size();
    if (N == 0) return;

        vector<vector<int>> adjMatrix(N);

        for (int i = 0; i < N; i++)
        {
        for (int j = 0; j < N; j++)
        {
                adjMatrix[i].push_back(1000000);
        }
        }

        //make adj.list using minimum cost of flights and thn use for prim
    for (int i = 0; i < N; ++i) {
        int u = subsetAirports[i];
        for (int j = 0; j < N; ++j) {
            if (i == j) continue;
            int v = subsetAirports[j];

            int minCost = 1000000;
            for (const Content& f : flightGraph[u][v]) {
                minCost = min(minCost, f.cost);
            }

            adjMatrix[i][j] = minCost;
        }
    }

    vector<int> key(N, 1000000);
    vector<int> parent(N, -1);
    key[0] = 0;
    int totalCost = 0;

        //to check if an edge is in the minimum spanning tree
    Set mstSet;
    cout << "\n=== Minimum Spanning Tree ===\n";

    for (int count = 0; count < N; ++count) {
        int minKey = 1000000, u = -1;

        for (int i = 0; i < N; ++i) {
            if (!(mstSet ^ i) && key[i] < minKey) {
                minKey = key[i];
                u = i;
            }
        }

        if (u == -1) break;

        mstSet += u;

        if (parent[u] != -1) {
            int from = subsetAirports[parent[u]];
            int to = subsetAirports[u];

            cout << airports[from].city << " (" << airports[from].FAA << ")"
                 << " -- $" << key[u] << " --> "
                 << airports[to].city << " (" << airports[to].FAA << ")\n";

            totalCost += key[u];
        }

        for (int v = 0; v < N; ++v) {
            if (!(mstSet ^ v) && adjMatrix[u][v] < key[v]) {
                key[v] = adjMatrix[u][v];
                parent[v] = u;
            }
        }
    }

    cout << "Total Cost of the Minimum Spanning Tree: $" << totalCost << endl;
}




//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------









int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <airports file> <flights file>\n";
        return 1;
    }

    loadAirports(argv[1]);
    loadFlights(argv[2]);

    vector<int> flights;
    for (int i = 0; i < airports.size(); ++i) {
        flights.push_back(i);
    }

    buildMSTFromAirports(flights);

    return 0;
}
