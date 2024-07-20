// Vehicle Routing Problem with Pick-up and Delivery (VRPPD)
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

// Define constants for the problem
const int NUM_CUSTOMERS = 10;   // Number of customers
const int NUM_VEHICLES = 3;     // Number of vehicles

// Define structures for points, customers, vehicles, and routes
struct Point {
    int x, y;
};

struct TimeWindow {
    int start_time;
    int end_time;
};

struct Customer {
    Point pickup;
    Point delivery;
    TimeWindow time_window; // Time window for pickup
};

struct Vehicle {
    Point depot;
    vector<int> route;  // Sequence of customer indices
};

struct Solution {
    vector<Vehicle> vehicles;
    double cost;
};

// Global variables
vector<Customer> customers(NUM_CUSTOMERS);
vector<Point> depots(NUM_VEHICLES); // Each vehicle has a starting depot
vector<TimeWindow> customer_time_windows(NUM_CUSTOMERS);
Solution best_solution;
double best_cost = numeric_limits<double>::max();
vector<vector<double>> distance_matrix; // Distance matrix for all points

// Helper functions
double distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void calculate_distance_matrix() {
    int num_points = NUM_CUSTOMERS + NUM_VEHICLES;
    distance_matrix.assign(num_points, vector<double>(num_points, 0.0));

    for (int i = 0; i < NUM_VEHICLES; ++i) {
        for (int j = 0; j < NUM_VEHICLES; ++j) {
            if (i != j) {
                distance_matrix[i][j] = distance(depots[i], depots[j]);
            }
        }
    }

    for (int i = 0; i < NUM_VEHICLES; ++i) {
        for (int j = 0; j < NUM_CUSTOMERS; ++j) {
            distance_matrix[i][NUM_VEHICLES + j] = distance(depots[i], customers[j].pickup);
            distance_matrix[NUM_VEHICLES + j][i] = distance(customers[j].delivery, depots[i]);
        }
    }

    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        for (int j = 0; j < NUM_CUSTOMERS; ++j) {
            if (i != j) {
                distance_matrix[NUM_VEHICLES + i][NUM_VEHICLES + j] = distance(customers[i].delivery, customers[j].pickup);
            }
        }
    }
}

double calculate_route_cost(const Vehicle& vehicle) {
    double total_distance = 0.0;
    Point current_location = vehicle.depot;
    int current_time = 0;

    for (int customer_idx : vehicle.route) {
        total_distance += distance_matrix[current_location.x][current_location.y];
        current_time += distance_matrix[current_location.x][current_location.y];

        if (current_time < customers[customer_idx].time_window.start_time) {
            current_time = customers[customer_idx].time_window.start_time;
        }
        else if (current_time > customers[customer_idx].time_window.end_time) {
            total_distance += (current_time - customers[customer_idx].time_window.end_time);
            current_time = customers[customer_idx].time_window.start_time;
        }

        total_distance += distance_matrix[customers[customer_idx].pickup.x][customers[customer_idx].pickup.y];
        current_time += distance_matrix[customers[customer_idx].pickup.x][customers[customer_idx].pickup.y];
        total_distance += distance_matrix[customers[customer_idx].delivery.x][customers[customer_idx].delivery.y];
        current_location = customers[customer_idx].delivery;
    }

    total_distance += distance_matrix[current_location.x][current_location.y];
    return total_distance;
}

double calculate_solution_cost(const Solution& solution) {
    double total_cost = 0.0;
    for (const Vehicle& vehicle : solution.vehicles) {
        total_cost += calculate_route_cost(vehicle);
    }
    return total_cost;
}

void generate_initial_solution() {
    Solution initial_solution;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        Vehicle vehicle;
        vehicle.depot = depots[v];
        vehicle.route.push_back(rand() % NUM_CUSTOMERS);
        vehicle.route.push_back(rand() % NUM_CUSTOMERS);
        initial_solution.vehicles.push_back(vehicle);
    }
    initial_solution.cost = calculate_solution_cost(initial_solution);
    best_solution = initial_solution;
    best_cost = initial_solution.cost;
}

vector<Solution> generate_neighborhood(const Solution& current_solution) {
    vector<Solution> neighborhood;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        Vehicle current_vehicle = current_solution.vehicles[v];
        int num_customers = current_vehicle.route.size();

        for (int i = 1; i < num_customers - 1; ++i) {
            for (int j = i + 1; j < num_customers; ++j) {
                Vehicle neighbor_vehicle = current_vehicle;
                reverse(neighbor_vehicle.route.begin() + i, neighbor_vehicle.route.begin() + j);
                Solution neighbor_solution = current_solution;
                neighbor_solution.vehicles[v] = neighbor_vehicle;
                neighbor_solution.cost = calculate_solution_cost(neighbor_solution);
                neighborhood.push_back(neighbor_solution);
            }
        }
    }
    return neighborhood;
}

void simulated_annealing(double initial_temperature, double cooling_rate, int iterations) {
    srand(time(nullptr));
    double temperature = initial_temperature;
    Solution current_solution;
    generate_initial_solution();

    while (temperature > 1e-6) {
        for (int i = 0; i < iterations; ++i) {
            int vehicle_index = rand() % NUM_VEHICLES;
            current_solution = best_solution;
            vector<Solution> neighborhood = generate_neighborhood(current_solution);
            Solution neighbor_solution = neighborhood[rand() % neighborhood.size()];
            double delta_cost = neighbor_solution.cost - current_solution.cost;
            if (delta_cost < 0 || exp(-delta_cost / temperature) > (double)rand() / RAND_MAX) {
                best_solution = neighbor_solution;
                best_cost += delta_cost;
            }
        }
        temperature *= cooling_rate;
    }
}

int main() {
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customers[i].pickup.x = rand() % 100;
        customers[i].pickup.y = rand() % 100;
        customers[i].delivery.x = rand() % 100;
        customers[i].delivery.y = rand() % 100;
        customers[i].time_window.start_time = rand() % 100;
        customers[i].time_window.end_time = customers[i].time_window.start_time + rand() % 100;
    }

    for (int i = 0; i < NUM_VEHICLES; ++i) {
        depots[i].x = rand() % 100;
        depots[i].y = rand() % 100;
    }

    calculate_distance_matrix();

    simulated_annealing(100.0, 0.99, 1000);

    cout << "Best solution cost: " << best_cost << endl;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        cout << "Vehicle " << v << " route: ";
        for (int customer_idx : best_solution.vehicles[v].route) {
            cout << customer_idx << " ";
        }
        cout << endl;
    }

    return 0;
}
