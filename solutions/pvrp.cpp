// Period Vehicle Routing Problem (PVRP)
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

const int NUM_CUSTOMERS = 20;   
const int NUM_VEHICLES = 4;       
const int VEHICLE_CAPACITY = 10;  
const int MAX_ITER = 10000;       
const double INITIAL_TEMP = 100.0;
const double COOLING_RATE = 0.003;
const int PERIOD_LENGTH = 7;      
struct Customer {
    int id;
    int demand;
};
struct Depot {
    int id;
};
double distance(const Customer& c1, const Customer& c2) {
    return sqrt(pow(c1.id - c2.id, 2));
}

double calculate_route_distance(const vector<int>& route, const vector<Customer>& customers) {
    double total_distance = 0.0;
    for (int i = 0; i < route.size() - 1; ++i) {
        total_distance += distance(customers[route[i]], customers[route[i + 1]]);
    }
    total_distance += distance(customers[route.back()], customers[0]);
    return total_distance;
}

vector<vector<int>> generate_initial_solution(const vector<Customer>& customers) {
    vector<vector<int>> initial_solution(NUM_VEHICLES);

    int current_capacity = 0;
    int current_vehicle = 0;
    vector<bool> visited(customers.size(), false);
    visited[0] = true;
    int current_customer = 0; 
    while (true) {
        int nearest_customer = -1;
        double min_distance = numeric_limits<double>::max();
        for (int i = 1; i < customers.size(); ++i) {
            if (!visited[i] && customers[i].demand + current_capacity <= VEHICLE_CAPACITY) {
                double dist = distance(customers[current_customer], customers[i]);
                if (dist < min_distance) {
                    min_distance = dist;
                    nearest_customer = i;
                }
            }
        }
        if (nearest_customer == -1) {
            current_vehicle++;
            if (current_vehicle >= NUM_VEHICLES) {
                break; 
            }
            current_capacity = 0;
            current_customer = 0;
        } else {
           initial_solution[current_vehicle].push_back(nearest_customer);
            visited[nearest_customer] = true;
            current_capacity += customers[nearest_customer].demand;
            current_customer = nearest_customer;
        }
    }

    return initial_solution;
}
vector<vector<int>> simulated_annealing(const vector<Customer>& customers) {
    vector<vector<int>> current_solution = generate_initial_solution(customers);
    vector<vector<int>> best_solution = current_solution;
    double temperature = INITIAL_TEMP;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        vector<vector<int>> new_solution = current_solution;
        int vehicle1 = rand() % NUM_VEHICLES;
        int vehicle2 = rand() % NUM_VEHICLES;
        if (vehicle1 != vehicle2 && !new_solution[vehicle1].empty() && !new_solution[vehicle2].empty()) {
            int rand_index1 = rand() % new_solution[vehicle1].size();
            int rand_index2 = rand() % new_solution[vehicle2].size();
            swap(new_solution[vehicle1][rand_index1], new_solution[vehicle2][rand_index2]);
        }
        double current_cost = 0.0;
        double new_cost = 0.0;
        for (int v = 0; v < NUM_VEHICLES; ++v) {
            current_cost += calculate_route_distance(current_solution[v], customers);
            new_cost += calculate_route_distance(new_solution[v], customers);
        }
        if (new_cost < current_cost) {
            current_solution = new_solution;
        } else {
            double acceptance_probability = exp((current_cost - new_cost) / temperature);
            double random_value = dis(gen);
            if (random_value < acceptance_probability) {
                current_solution = new_solution;
            }
        }
        if (calculate_route_distance(current_solution[0], customers) < calculate_route_distance(best_solution[0], customers)) {
            best_solution = current_solution;
        }
        temperature *= (1 - COOLING_RATE);
    }
    return best_solution;
}

int main() {
    vector<Customer> customers(NUM_CUSTOMERS + 1);  
    for (int i = 1; i <= NUM_CUSTOMERS; ++i) {
        customers[i].id = i;
        customers[i].demand = rand() % 5 + 1;  
    }
    for (int period = 0; period < PERIOD_LENGTH; ++period) {
        auto start_time = chrono::high_resolution_clock::now();
        vector<vector<int>> solution = simulated_annealing(customers);
        auto end_time = chrono::high_resolution_clock::now();
        cout << "Best solution found for Day " << period + 1 << ":" << endl;
        for (int v = 0; v < NUM_VEHICLES; ++v) {
            cout << "Vehicle " << v + 1 << ": ";
            for (int i = 0; i < solution[v].size(); ++i) {
                cout << solution[v][i] << " ";
            }
            cout << endl;
        }
        double total_distance = 0.0;
        for (int v = 0; v < NUM_VEHICLES; ++v) {
            total_distance += calculate_route_distance(solution[v], customers);
        }
        cout << "Total Distance for Day " << period + 1 << ": " << total_distance << endl;
        chrono::duration<double> elapsed_time = end_time - start_time;
        cout << "Execution Time for Day " << period + 1 << ": " << elapsed_time.count() << " seconds" << endl;
    }
    return 0;
}
