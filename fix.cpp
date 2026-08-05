#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class airport {
public:
    string FAA;
    string name;
    string city;
    string state;
};

vector<airport> group;

void printstoredobjects() {
    cout << "Stored objects:" << endl;
    int count = 1;
    for (const auto &files : group) {
        cout << count << ". " << files.FAA << " " << files.name << " " << files.city << " " << files.state << endl;
        count++;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <airports file> <flights file>\n";
        return 1;
    }

    ifstream userfile(argv[1]);
    if (!userfile) {
        cout << "Error opening data file!\n";
        return 1;
    }

    string line;
    while (getline(userfile, line)) {
        airport tmp;
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

        group.push_back(tmp);
    }

    printstoredobjects();

    int x;
    cout << "Input 1 to find an airport, 0 to quit: ";
    cin >> x;

    while (x != 0) {
        cout << "Input the 3-letter FAA code: ";
        string ans;
        cin >> ans;

        bool found = false;

        for (const auto &airport : group) {
            if (ans == airport.FAA) {
                cout << airport.FAA << " " << airport.name << " " << airport.city << " " << airport.state << endl;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Not found." << endl;
        }

        cout << "Input 1 to find another airport, 0 to quit: ";
        cin >> x;
    }

    return 0;
}
