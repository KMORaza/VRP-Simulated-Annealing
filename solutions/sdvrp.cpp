// Split Delivery Vehicle Routing Problem (SDVRP) 
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

const int NUM_CUSTOMERS = 50;
const int NUM_VEHICLES = 10;
const int VEHICLE_CAPACITY = 100;
const double COOLING_RATE = 0.99;
const double INITIAL_TEMPERATURE = 1000.0;
const int MAX_ITERATIONS = 10000;

struct Customer {
    int demand;
    int x, y;
};

vector<Customer> customers(NUM_CUSTOMERS);
vector<vector<int>> routes(NUM_VEHICLES);
vector<int> vehicle_capacity(NUM_VEHICLES, VEHICLE_CAPACITY);

double distance(int i, int j) {
    return sqrt(pow(customers[i].x - customers[j].x, 2) + pow(customers[i].y - customers[j].y, 2));
}

void generate_initial_solution() {
    vector<int> unvisited_customers(NUM_CUSTOMERS);
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        unvisited_customers[i] = i;
    }

    sort(unvisited_customers.begin(), unvisited_customers.end(), [](int a, int b) {
        return customers[a].demand > customers[b].demand;
    });

    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        int customer = unvisited_customers[i];
        int min_route_index = -1;
        double min_increase_cost = numeric_limits<double>::max();

        for (int v = 0; v < NUM_VEHICLES; ++v) {
            if (vehicle_capacity[v] >= customers[customer].demand) {
                double increase_cost = 0.0;
                if (!routes[v].empty()) {
                    increase_cost += distance(routes[v].back(), customer);
                    increase_cost += distance(customer, 0);
                } else {
                    increase_cost += distance(0, customer);
                    increase_cost += distance(customer, 0);
                }

                if (increase_cost < min_increase_cost) {
                    min_increase_cost = increase_cost;
                    min_route_index = v;
                }
            }
        }

        if (min_route_index != -1) {
            routes[min_route_index].push_back(customer);
            vehicle_capacity[min_route_index] -= customers[customer].demand;
        }
    }
}

double evaluate_solution() {
    double total_distance = 0.0;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        if (!routes[v].empty()) {
            total_distance += distance(0, routes[v][0]);
            for (int i = 0; i < routes[v].size() - 1; ++i) {
                total_distance += distance(routes[v][i], routes[v][i + 1]);
            }
            total_distance += distance(routes[v].back(), 0);
        }
    }
    return total_distance;
}

void simulated_annealing() {
    double temperature = INITIAL_TEMPERATURE;
    double current_distance = evaluate_solution();
    double best_distance = current_distance;
    vector<vector<int>> best_solution = routes;

    for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
        int v1 = rand() % NUM_VEHICLES;
        int v2 = rand() % NUM_VEHICLES;

        if (v1 != v2 && !routes[v1].empty() && !routes[v2].empty()) {
            int idx1 = rand() % routes[v1].size();
            int idx2 = rand() % routes[v2].size();

            int temp_customer = routes[v1][idx1];
            routes[v1][idx1] = routes[v2][idx2];
            routes[v2][idx2] = temp_customer;

            double new_distance = evaluate_solution();

            if (new_distance < current_distance || exp((current_distance - new_distance) / temperature) > ((double)rand() / RAND_MAX)) {
                current_distance = new_distance;
                if (current_distance < best_distance) {
                    best_distance = current_distance;
                    best_solution = routes;
                }
            } else {
                temp_customer = routes[v1][idx1];
                routes[v1][idx1] = routes[v2][idx2];
                routes[v2][idx2] = temp_customer;
            }
        }

        temperature *= COOLING_RATE;
    }

    cout << "Best distance found: " << best_distance << endl;
    cout << "Best solution: " << endl;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        cout << "Route " << v << ": ";
        for (int i = 0; i < best_solution[v].size(); ++i) {
            cout << best_solution[v][i] << " ";
        }
        cout << endl;
    }
}

int main() {
    srand(time(NULL));
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customers[i].demand = rand() % 10 + 1;
        customers[i].x = rand() % 100;
        customers[i].y = rand() % 100;
    }

    generate_initial_solution();

    simulated_annealing();

    return 0;
}
