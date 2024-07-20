// Stochastic Vehicle Routing Problem (SVRP) 
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <limits>

using namespace std;

const int NUM_VEHICLES = 3;
const int DEPOT_INDEX = 0;
const double INF = numeric_limits<double>::infinity();

struct Customer {
    int x;
    int y;
};

struct Solution {
    vector<vector<int>> routes;
    double cost;
};

double euclideanDistance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

vector<Customer> readCustomersFromFile(const string& filename) {
    vector<Customer> customers;
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    int numCustomers;
    file >> numCustomers;

    customers.resize(numCustomers);

    for (int i = 0; i < numCustomers; ++i) {
        file >> customers[i].x >> customers[i].y;
    }

    file.close();
    return customers;
}

Solution generateInitialSolution(const vector<Customer>& customers) {
    Solution initialSolution;
    initialSolution.routes.resize(NUM_VEHICLES);

    for (int i = 1; i < customers.size(); ++i) {
        int vehicle = rand() % NUM_VEHICLES;
        initialSolution.routes[vehicle].push_back(i);
    }

    initialSolution.cost = 0.0;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        if (!initialSolution.routes[v].empty()) {
            int prevNode = DEPOT_INDEX;
            for (int i = 0; i < initialSolution.routes[v].size(); ++i) {
                int customer = initialSolution.routes[v][i];
                initialSolution.cost += euclideanDistance(customers[prevNode].x, customers[prevNode].y,
                                                          customers[customer].x, customers[customer].y);
                prevNode = customer;
            }
            initialSolution.cost += euclideanDistance(customers[prevNode].x, customers[prevNode].y,
                                                      customers[DEPOT_INDEX].x, customers[DEPOT_INDEX].y);
        }
    }

    return initialSolution;
}

Solution generateNeighborSolution(const Solution& currentSolution) {
    Solution neighborSolution = currentSolution;

    int vehicle1 = rand() % NUM_VEHICLES;
    int vehicle2 = rand() % NUM_VEHICLES;
    while (vehicle1 == vehicle2) {
        vehicle2 = rand() % NUM_VEHICLES;
    }

    if (!neighborSolution.routes[vehicle1].empty() && !neighborSolution.routes[vehicle2].empty()) {
        int customer1 = rand() % neighborSolution.routes[vehicle1].size();
        int customer2 = rand() % neighborSolution.routes[vehicle2].size();

        int temp = neighborSolution.routes[vehicle1][customer1];
        neighborSolution.routes[vehicle1][customer1] = neighborSolution.routes[vehicle2][customer2];
        neighborSolution.routes[vehicle2][customer2] = temp;

        neighborSolution.cost = 0.0;
        for (int v = 0; v < NUM_VEHICLES; ++v) {
            if (!neighborSolution.routes[v].empty()) {
                int prevNode = DEPOT_INDEX;
                for (int i = 0; i < neighborSolution.routes[v].size(); ++i) {
                    int customer = neighborSolution.routes[v][i];
                    neighborSolution.cost += euclideanDistance(customers[prevNode].x, customers[prevNode].y,
                                                               customers[customer].x, customers[customer].y);
                    prevNode = customer;
                }
                neighborSolution.cost += euclideanDistance(customers[prevNode].x, customers[prevNode].y,
                                                           customers[DEPOT_INDEX].x, customers[DEPOT_INDEX].y);
            }
        }
    }

    return neighborSolution;
}

Solution simulatedAnnealing(const vector<Customer>& customers, double initialTemperature, double coolingRate, int iterations) {
    Solution currentSolution = generateInitialSolution(customers);
    Solution bestSolution = currentSolution;

    double temperature = initialTemperature;

    for (int i = 0; i < iterations; ++i) {
        Solution neighborSolution = generateNeighborSolution(currentSolution);
        double deltaCost = neighborSolution.cost - currentSolution.cost;

        if (deltaCost < 0 || exp(-deltaCost / temperature) > ((double) rand() / RAND_MAX)) {
            currentSolution = neighborSolution;
        }

        if (currentSolution.cost < bestSolution.cost) {
            bestSolution = currentSolution;
        }

        temperature *= coolingRate;
    }

    return bestSolution;
}

void outputSolution(const Solution& bestSolution, const vector<Customer>& customers) {
    cout << "Best Solution (Cost = " << bestSolution.cost << "):" << endl;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        cout << "Vehicle " << v + 1 << ": ";
        for (int i = 0; i < bestSolution.routes[v].size(); ++i) {
            int customerIndex = bestSolution.routes[v][i];
            cout << customerIndex << " ";
        }
        cout << endl;
    }

    for (int v = 0; v < NUM_VEHICLES; ++v) {
        cout << "Route for Vehicle " << v + 1 << ": ";
        if (!bestSolution.routes[v].empty()) {
            int prevNode = DEPOT_INDEX;
            for (int i = 0; i < bestSolution.routes[v].size(); ++i) {
                int customerIndex = bestSolution.routes[v][i];
                cout << prevNode << " -> " << customerIndex << " ";
                prevNode = customerIndex;
            }
            cout << "-> " << DEPOT_INDEX;
        }
        cout << endl;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    string filename = "customers.txt";
    vector<Customer> customers = readCustomersFromFile(filename);

    double initialTemperature = 1000;
    double coolingRate = 0.95;
    int iterations = 10000;

    Solution bestSolution = simulatedAnnealing(customers, initialTemperature, coolingRate, iterations);

    outputSolution(bestSolution, customers);

    return 0;
}
