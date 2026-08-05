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

class content
{
public:

	int time;
	int cost;
	int miles;
	string airline;
	string flightID;


};



    ifstream userfile(argv[2]);
    if (!userfile) {
        cout << "Error opening data file!\n";
        return 1;
    }

    string line;
    while (getline(userfile, line)) {
        content tmp;
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


int printstoredobjects() {
    cout << "Stored objects:" << endl;
    int count = 1;
    for (const auto &files : group) {
        cout << count << ". " << files.FAA << " " << files.name << " " << files.city << " " << files.state << endl;
        count++;
        cout << endl;
    }
    return count - 1;
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

    int count = printstoredobjects();

	content arr[count][count];

    int x;
    cout << "Input 1 to find an airport, 0 to quit: ";
    cin >> x;

    while (x != 0) {
        cout << "Input the 3-letter FAA code: ";
        string ans;
        cin >> ans;

        bool found = false;

        for (int k = 0; k < count; k++) {
            if (ans == group[k].FAA) {
				cout <<" -------AIRPORT LIST------- "<< endl;
				cout <<" Airport with FAA "<< ans << " has been found."<< endl << " Details below:"<< endl;
                cout << endl <<"FAA   - " << group[k].FAA << endl << "Name  - " << group[k].name << endl << "City  - " << group[k].city << endl << "State - " << group[k].state << endl << endl;
				cout <<" ------- THANK YOU! ------- "<<endl;
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
