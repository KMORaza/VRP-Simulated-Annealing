// Time-dependent Vehicle Routing Problem with Time Windows and Multiple Service Workers (VRPTWMS)
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
using namespace std;
const int NUM_VEHICLES = 2; 
const int NUM_CUSTOMERS = 5; 
const int MAX_TIME = 1000; 
const int COOLING_STEPS = 1000; 
struct Customer {
    int id;
    int demand;
    int start_time;
    int end_time;
    vector<int> service_times; 
};
struct Solution {
    vector<vector<int>> routes;
    vector<int> route_times; 
    int total_time; 
};
vector<Customer> customers(NUM_CUSTOMERS);
vector<vector<int>> travel_time(NUM_CUSTOMERS + 1, vector<int>(NUM_CUSTOMERS + 1)); 
void initialize_problem() {
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customers[i].id = i + 1;
        customers[i].demand = 1; 
        customers[i].start_time = 0; 
        customers[i].end_time = MAX_TIME; 
        customers[i].service_times = {10, 15};
    }
    for (int i = 0; i <= NUM_CUSTOMERS; ++i) {
        for (int j = 0; j <= NUM_CUSTOMERS; ++j) {
            if (i != j) {
                travel_time[i][j] = rand() % 50 + 10; 
            } else {
                travel_time[i][j] = 0; 
            }
        }
    }
}
Solution generate_initial_solution() {
    Solution initial_solution;
    initial_solution.routes.resize(NUM_VEHICLES);
    initial_solution.route_times.resize(NUM_VEHICLES);
    initial_solution.total_time = numeric_limits<int>::max();
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        int vehicle_id = rand() % NUM_VEHICLES;
        initial_solution.routes[vehicle_id].push_back(customers[i].id);
    }
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        initial_solution.route_times[v] = 0;
        int current_location = 0;
        for (int i = 0; i < initial_solution.routes[v].size(); ++i) {
            int next_location = initial_solution.routes[v][i];
            initial_solution.route_times[v] += travel_time[current_location][next_location];
            current_location = next_location;
        }
        initial_solution.route_times[v] += travel_time[current_location][0]; 
        if (initial_solution.route_times[v] < initial_solution.total_time) {
            initial_solution.total_time = initial_solution.route_times[v];
        }
    }
    return initial_solution;
}
int evaluate_solution(const Solution& solution) {
    return solution.total_time;
}
Solution generate_neighbor(const Solution& current_solution) {
    Solution neighbor_solution = current_solution;
    int vehicle_id = rand() % NUM_VEHICLES;
    if (neighbor_solution.routes[vehicle_id].size() > 0) {
        int customer_index = rand() % neighbor_solution.routes[vehicle_id].size();
        int customer_id = neighbor_solution.routes[vehicle_id][customer_index];
        neighbor_solution.routes[vehicle_id].erase(neighbor_solution.routes[vehicle_id].begin() + customer_index);
        int new_vehicle_id = rand() % NUM_VEHICLES;
        neighbor_solution.routes[new_vehicle_id].push_back(customer_id);
        vector<int> affected_vehicles = {vehicle_id, new_vehicle_id};
        for (int v : affected_vehicles) {
            neighbor_solution.route_times[v] = 0;
            int current_location = 0;
            for (int i = 0; i < neighbor_solution.routes[v].size(); ++i) {
                int next_location = neighbor_solution.routes[v][i];
                neighbor_solution.route_times[v] += travel_time[current_location][next_location];
                current_location = next_location;
            }
            neighbor_solution.route_times[v] += travel_time[current_location][0];
        }
        neighbor_solution.total_time = *max_element(neighbor_solution.route_times.begin(), neighbor_solution.route_times.end());
    }
    return neighbor_solution;
}
bool accept_neighbor(int current_time, int new_time, double temperature) {
    if (new_time <= current_time) {
        return true;
    } else {
        double probability = exp((current_time - new_time) / temperature);
        double rand_num = static_cast<double>(rand()) / RAND_MAX;
        return rand_num < probability;
    }
}
Solution simulated_annealing() {
    srand(static_cast<unsigned>(time(nullptr)));
    Solution current_solution = generate_initial_solution();
    int current_time = evaluate_solution(current_solution);
    double initial_temperature = 1000.0;
    double cooling_rate = 0.99;
    double temperature = initial_temperature;
    for (int step = 0; step < COOLING_STEPS; ++step) {
        Solution neighbor_solution = generate_neighbor(current_solution);
        int neighbor_time = evaluate_solution(neighbor_solution);
        if (accept_neighbor(current_time, neighbor_time, temperature)) {
            current_solution = neighbor_solution;
            current_time = neighbor_time;
        }
        temperature *= cooling_rate;
    }
    return current_solution;
}
void print_solution(const Solution& solution) {
    cout << "Total time: " << solution.total_time << endl;
    for (int v = 0; v < NUM_VEHICLES; ++v) {
        cout << "Route " << v + 1 << ": ";
        for (int i = 0; i < solution.routes[v].size(); ++i) {
            cout << solution.routes[v][i] << " ";
        }
        cout << "- Total time: " << solution.route_times[v] << endl;
    }
}
int main() {
    initialize_problem();
    Solution best_solution = simulated_annealing();
    cout << "Best solution found:" << endl;
    print_solution(best_solution);
    return 0;
}
