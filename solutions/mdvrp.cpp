// Multi-Depot Vehicle Routing Problem (MDVRP)
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace std;

const int MAX_VEHICLES = 20;
const int MAX_CUSTOMERS = 50;
const double MAX_DISTANCE = 1000.0;

struct Customer {
    int demand;
    double x, y;
};

struct Vehicle {
    int capacity;
    int current_load;
    int depot;
    vector<int> route;
};

struct Solution {
    vector<Vehicle> vehicles;
    double cost;
};

double distance(const Customer& c1, const Customer& c2) {
    return sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));
}

Solution generateInitialSolution(const vector<Customer>& customers, int num_depots) {
    Solution initial_solution;
    initial_solution.cost = 0.0;

    for (int i = 0; i < MAX_VEHICLES; ++i) {
        Vehicle v;
        v.capacity = 100;
        v.current_load = 0;
        v.depot = i % num_depots;
        initial_solution.vehicles.push_back(v);
    }

    vector<int> customer_indices(customers.size());
    for (int i = 0; i < customers.size(); ++i) {
        customer_indices[i] = i;
    }
    random_shuffle(customer_indices.begin(), customer_indices.end());

    int vehicle_idx = 0;
    for (int i = 0; i < customers.size(); ++i) {
        int cust_idx = customer_indices[i];
        if (initial_solution.vehicles[vehicle_idx].current_load + customers[cust_idx].demand > initial_solution.vehicles[vehicle_idx].capacity) {
            vehicle_idx = (vehicle_idx + 1) % MAX_VEHICLES;
        }
        initial_solution.vehicles[vehicle_idx].route.push_back(cust_idx);
        initial_solution.vehicles[vehicle_idx].current_load += customers[cust_idx].demand;
    }

    for (int i = 0; i < initial_solution.vehicles.size(); ++i) {
        Vehicle& v = initial_solution.vehicles[i];
        if (!v.route.empty()) {
            int last_cust_idx = v.route.back();
            initial_solution.cost += distance(customers[last_cust_idx], customers[v.depot]);
            for (int j = 0; j < v.route.size() - 1; ++j) {
                int cust_idx1 = v.route[j];
                int cust_idx2 = v.route[j + 1];
                initial_solution.cost += distance(customers[cust_idx1], customers[cust_idx2]);
            }
        }
    }

    return initial_solution;
}

Solution simulatedAnnealing(const vector<Customer>& customers, int num_depots) {
    srand(time(nullptr));

    double initial_temperature = 1000.0;
    double alpha = 0.95;
    int max_iterations = 1000;

    Solution current_solution = generateInitialSolution(customers, num_depots);
    Solution best_solution = current_solution;

    double current_temperature = initial_temperature;

    for (int iter = 0; iter < max_iterations; ++iter) {
        Solution new_solution = current_solution;

        int vehicle_idx = rand() % MAX_VEHICLES;
        if (!new_solution.vehicles[vehicle_idx].route.empty()) {
            int customer_idx = rand() % new_solution.vehicles[vehicle_idx].route.size();
            int selected_customer = new_solution.vehicles[vehicle_idx].route[customer_idx];
            
            int new_vehicle_idx = rand() % MAX_VEHICLES;
            while (new_vehicle_idx == vehicle_idx || new_solution.vehicles[new_vehicle_idx].current_load + customers[selected_customer].demand > new_solution.vehicles[new_vehicle_idx].capacity) {
                new_vehicle_idx = rand() % MAX_VEHICLES;
            }

            new_solution.vehicles[vehicle_idx].route.erase(new_solution.vehicles[vehicle_idx].route.begin() + customer_idx);
            new_solution.vehicles[new_vehicle_idx].route.push_back(selected_customer);
            new_solution.vehicles[new_vehicle_idx].current_load += customers[selected_customer].demand;

            new_solution.cost = 0.0;
            for (int i = 0; i < new_solution.vehicles.size(); ++i) {
                Vehicle& v = new_solution.vehicles[i];
                if (!v.route.empty()) {
                    int last_cust_idx = v.route.back();
                    new_solution.cost += distance(customers[last_cust_idx], customers[v.depot]);
                    for (int j = 0; j < v.route.size() - 1; ++j) {
                        int cust_idx1 = v.route[j];
                        int cust_idx2 = v.route[j + 1];
                        new_solution.cost += distance(customers[cust_idx1], customers[cust_idx2]);
                    }
                }
            }

            double rand_num = (rand() % 100) / 100.0;
            double delta_cost = new_solution.cost - current_solution.cost;
            if (delta_cost < 0 || exp(-delta_cost / current_temperature) > rand_num) {
                current_solution = new_solution;
            }

            if (current_solution.cost < best_solution.cost) {
                best_solution = current_solution;
            }
        }

        current_temperature *= alpha;
    }

    return best_solution;
}

void printSolution(const Solution& solution, const vector<Customer>& customers) {
    cout << "Total Cost: " << solution.cost << endl;
    for (int i = 0; i < solution.vehicles.size(); ++i) {
        const Vehicle& v = solution.vehicles[i];
        cout << "Vehicle " << i << " (Depot " << v.depot << ") Route: ";
        for (int j = 0; j < v.route.size(); ++j) {
            cout << v.route[j] << " ";
        }
        cout << endl;
    }
}

int main() {
    srand(time(nullptr));

    vector<Customer> customers(MAX_CUSTOMERS);
    for (int i = 0; i < MAX_CUSTOMERS; ++i) {
        customers[i].demand = rand() % 10 + 1;
        customers[i].x = (rand() / (double)RAND_MAX) * MAX_DISTANCE;
        customers[i].y = (rand() / (double)RAND_MAX) * MAX_DISTANCE;
    }

    int num_depots = 3;

    Solution best_solution = simulatedAnnealing(customers, num_depots);

    cout << "Best Solution:" << endl;
    printSolution(best_solution, customers);

    return 0;
}
