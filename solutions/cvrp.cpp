// Capacited Vehicle Routing Problem (CVRP) 
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <limits>

using namespace std;

const int NUM_CUSTOMERS = 20;
const int NUM_VEHICLES = 5;
const int CAPACITY = 100;
const int MAX_ITERATIONS = 10000;
const double INIT_TEMPERATURE = 1000.0;
const double COOLING_RATE = 0.003;

struct Customer {
    int demand;
    int x, y;
};

double distance(const Customer& cust1, const Customer& cust2) {
    return sqrt(pow(cust1.x - cust2.x, 2) + pow(cust1.y - cust2.y, 2));
}

void generateProblem(vector<Customer>& customers, int& depot_x, int& depot_y) {
    srand(time(NULL));
    
    depot_x = rand() % 100;
    depot_y = rand() % 100;
    
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        Customer cust;
        cust.demand = rand() % 10 + 1;
        cust.x = rand() % 100;
        cust.y = rand() % 100;
        customers.push_back(cust);
    }
}

double calculateTotalDistance(const vector<vector<int>>& routes, const vector<Customer>& customers, const Customer& depot) {
    double total_distance = 0.0;
    
    for (const auto& route : routes) {
        if (route.empty()) continue;
        
        total_distance += distance(depot, customers[route[0]]);
        
        for (int i = 0; i < route.size() - 1; ++i) {
            total_distance += distance(customers[route[i]], customers[route[i + 1]]);
        }
        
        total_distance += distance(customers[route.back()], depot);
    }
    
    return total_distance;
}

vector<vector<int>> generateInitialSolution(const vector<Customer>& customers) {
    vector<vector<int>> solution(NUM_VEHICLES);
    vector<int> customer_indices(NUM_CUSTOMERS);
    
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customer_indices[i] = i;
    }
    
    random_shuffle(customer_indices.begin(), customer_indices.end());
    
    int vehicle_index = 0;
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        if (i % (NUM_CUSTOMERS / NUM_VEHICLES) == 0 && vehicle_index < NUM_VEHICLES) {
            ++vehicle_index;
        }
        solution[vehicle_index - 1].push_back(customer_indices[i]);
    }
    
    return solution;
}

vector<vector<int>> generateNeighborSolution(const vector<vector<int>>& current_solution) {
    vector<vector<int>> neighbor_solution = current_solution;
    
    int route1 = rand() % NUM_VEHICLES;
    int route2 = rand() % NUM_VEHICLES;
    
    if (neighbor_solution[route1].empty() || neighbor_solution[route2].empty()) {
        return neighbor_solution;
    }
    
    int cust_index1 = rand() % neighbor_solution[route1].size();
    int cust_index2 = rand() % neighbor_solution[route2].size();
    
    int temp = neighbor_solution[route1][cust_index1];
    neighbor_solution[route1][cust_index1] = neighbor_solution[route2][cust_index2];
    neighbor_solution[route2][cust_index2] = temp;
    
    return neighbor_solution;
}

double evaluateSolution(const vector<vector<int>>& solution, const vector<Customer>& customers, const Customer& depot) {
    double total_distance = calculateTotalDistance(solution, customers, depot);
    return total_distance;
}

bool acceptNeighbor(double current_cost, double new_cost, double temperature) {
    if (new_cost < current_cost) {
        return true;
    }
    double acceptance_prob = exp((current_cost - new_cost) / temperature);
    double random_prob = static_cast<double>(rand()) / RAND_MAX;
    return random_prob < acceptance_prob;
}

void updateTemperature(double& temperature) {
    temperature *= (1 - COOLING_RATE);
}

int main() {
    vector<Customer> customers;
    int depot_x, depot_y;
    generateProblem(customers, depot_x, depot_y);
    
    double temperature = INIT_TEMPERATURE;
    vector<vector<int>> current_solution = generateInitialSolution(customers);
    double current_cost = evaluateSolution(current_solution, customers, {0, depot_x, depot_y});
    
    vector<vector<int>> best_solution = current_solution;
    double best_cost = current_cost;
    
    for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
        vector<vector<int>> neighbor_solution = generateNeighborSolution(current_solution);
        double neighbor_cost = evaluateSolution(neighbor_solution, customers, {0, depot_x, depot_y});
        
        if (acceptNeighbor(current_cost, neighbor_cost, temperature)) {
            current_solution = neighbor_solution;
            current_cost = neighbor_cost;
        }
        
        if (current_cost < best_cost) {
            best_solution = current_solution;
            best_cost = current_cost;
        }
        
        updateTemperature(temperature);
    }
    
    cout << "Best solution found:" << endl;
    for (int i = 0; i < best_solution.size(); ++i) {
        cout << "Route " << i + 1 << ": ";
        for (int j = 0; j < best_solution[i].size(); ++j) {
            cout << best_solution[i][j] << " ";
        }
        cout << endl;
    }
    
    double total_distance = calculateTotalDistance(best_solution, customers, {0, depot_x, depot_y});
    cout << "Total distance traveled: " << total_distance << endl;
    
    return 0;
}
