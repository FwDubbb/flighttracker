# flighttracker

## File Format and Usage

The main program is located in:

```text
11weird.cpp
```

The program requires two input files:

1. A file containing U.S. airport information.
2. A file containing flight information.

Flight data files should use the `.air2` extension. All other data files are treated as airport files.

---

## Airport File Format

Each line represents one U.S. airport using the following format:

```text
FAA_CODE, AIRPORT_NAME, CITY, STATE
```

### Fields

* `FAA_CODE` — Three-letter airport code
* `AIRPORT_NAME` — Full airport name
* `CITY` — City where the airport is located
* `STATE` — Two-letter state abbreviation

### Example

```text
ABQ, Albuquerque International, Albuquerque, NM
LAX, Los Angeles International, Los Angeles, CA
JFK, John F. Kennedy International, New York, NY
```

---

## Flight File Format

Each line represents one flight using the following format:

```text
SOURCE DESTINATION DEPARTURE ARRIVAL COST MILES AIRLINE FLIGHT_CODE
```

### Fields

* `SOURCE` — Three-letter departure airport code
* `DESTINATION` — Three-letter arrival airport code
* `DEPARTURE` — Departure time in 24-hour `HHMM` format
* `ARRIVAL` — Arrival time in 24-hour `HHMM` format
* `COST` — Ticket cost as an integer
* `MILES` — Flight distance as an integer
* `AIRLINE` — Airline name with no spaces
* `FLIGHT_CODE` — Airline flight identifier

### Example

```text
LAX JFK 0600 1400 300 2475 Delta DL1001
ABQ LAX 0900 1100 180 677 Southwest WN245
```

---

## Compiling the Program

Compile the program with C++11 support:

```bash
g++ -std=c++11 11weird.cpp -o flights
```

---

## Running the Program

Run the compiled program by providing the airport file first and the flight file second:

```bash
./flights <airports_file> <flights_file>
```

### Example

```bash
./flights us_airports.txt us_flights.air2
```

You may use the included data files or create your own files using the formats described above.
