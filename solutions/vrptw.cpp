// Vehicle Routing Problem with Time Windows (VRPTW) 
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using namespace std;
const int MAX_ITER = 10000;
const double INITIAL_TEMPERATURE = 1000.0;
const double COOLING_RATE = 0.99;
struct Point {
    double x, y;
};
struct Customer {
    Point location;
    int demand;
    double ready_time;
    double due_time;
    double service_time;
};
struct Vehicle {
    int capacity;
};
struct Solution {
    vector<vector<int>> routes;
    double cost;
};
vector<Customer> customers;
Vehicle vehicle;
int num_vehicles;
double euclideanDistance(Point a, Point b);
double calculateTotalCost(const vector<vector<int>>& routes);
Solution generateInitialSolution();
Solution neighborSolution(const Solution& current_solution);
bool acceptNeighbor(double current_cost, double neighbor_cost, double temperature);
double anneal(Solution& initial_solution);
double euclideanDistance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
double calculateTotalCost(const vector<vector<int>>& routes) {
    double total_cost = 0.0;
    for (const auto& route : routes) {
        if (route.size() > 0) {
            Point prev_location = {0.0, 0.0};
            for (int i = 0; i < route.size(); ++i) {
                int customer_index = route[i];
                Point current_location = customers[customer_index].location;
                total_cost += euclideanDistance(prev_location, current_location);
                prev_location = current_location;
            }
            total_cost += euclideanDistance(prev_location, {0.0, 0.0});
        }
    }
    return total_cost;
}
Solution generateInitialSolution() {
    Solution initial_solution;
    initial_solution.routes.resize(num_vehicles);
    vector<int> customer_indices(customers.size());
    for (int i = 0; i < customers.size(); ++i) {
        customer_indices[i] = i;
    }
    random_shuffle(customer_indices.begin(), customer_indices.end());
    int vehicle_index = 0;
    for (int i = 0; i < customer_indices.size(); ++i) {
        int customer_index = customer_indices[i];
        initial_solution.routes[vehicle_index].push_back(customer_index);
        vehicle_index = (vehicle_index + 1) % num_vehicles;
    }
    initial_solution.cost = calculateTotalCost(initial_solution.routes);

    return initial_solution;
}
Solution neighborSolution(const Solution& current_solution) {
    Solution neighbor_solution = current_solution;
    int route1 = rand() % num_vehicles;
    int route2 = rand() % num_vehicles;
    while (route1 == route2 || neighbor_solution.routes[route1].empty() || neighbor_solution.routes[route2].empty()) {
        route1 = rand() % num_vehicles;
        route2 = rand() % num_vehicles;
    }
    int index1 = rand() % neighbor_solution.routes[route1].size();
    int index2 = rand() % neighbor_solution.routes[route2].size();
    int customer1 = neighbor_solution.routes[route1][index1];
    int customer2 = neighbor_solution.routes[route2][index2];
    neighbor_solution.routes[route1][index1] = customer2;
    neighbor_solution.routes[route2][index2] = customer1;
    neighbor_solution.cost = calculateTotalCost(neighbor_solution.routes);
    return neighbor_solution;
}
bool acceptNeighbor(double current_cost, double neighbor_cost, double temperature) {
    if (neighbor_cost < current_cost) {
        return true;
    } else {
        double acceptance_probability = exp((current_cost - neighbor_cost) / temperature);
        double rand_num = ((double) rand() / (RAND_MAX));
        return rand_num < acceptance_probability;
    }
}
double anneal(Solution& initial_solution) {
    Solution current_solution = initial_solution;
    double current_cost = initial_solution.cost;
    double temperature = INITIAL_TEMPERATURE;
    int iteration = 0;
    while (temperature > 1.0 && iteration < MAX_ITER) {
        Solution neighbor = neighborSolution(current_solution);
        double neighbor_cost = neighbor.cost;
        if (acceptNeighbor(current_cost, neighbor_cost, temperature)) {
            current_solution = neighbor;
            current_cost = neighbor_cost;
        }
        temperature *= COOLING_RATE;
        iteration++;
    }
    return current_cost;
}
int main() {
    srand(time(NULL)); 
    num_vehicles = 3;
    vehicle.capacity = 100;
    customers = {
        {{10.0, 10.0}, 5, 0.0, 100.0, 0.0},    
        {{20.0, 20.0}, 10, 0.0, 100.0, 0.0},
        {{30.0, 30.0}, 7, 0.0, 100.0, 0.0},
        {{15.0, 15.0}, 12, 0.0, 100.0, 0.0},
        {{25.0, 25.0}, 8, 0.0, 100.0, 0.0}
    };
    Solution initial_solution = generateInitialSolution();
    double best_cost = anneal(initial_solution);
    cout << "Best cost found: " << best_cost << endl;
    return 0;
}
