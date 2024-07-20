// Time Dependent Vehicle Routing Problem with Time Windows (TDVRPTW) 
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

const int MAX_ITER = 10000;
const double INITIAL_TEMPERATURE = 100.0;
const double FINAL_TEMPERATURE = 0.1;
const double COOLING_RATE = 0.95;

struct Node {
    int id;
    double x;
    double y;
    double demand;
    double ready_time;
    double due_time;
    double service_time;
};

struct TimeMatrix {
    vector<vector<double>> travel_time;
};

struct Solution {
    vector<vector<int>> routes;
    vector<double> arrival_times;
    double total_cost;
};

double distance(const Node& n1, const Node& n2) {
    return sqrt(pow(n1.x - n2.x, 2) + pow(n1.y - n2.y, 2));
}

double travelTime(const Node& n1, const Node& n2, const TimeMatrix& time_matrix) {
    return time_matrix.travel_time[n1.id][n2.id];
}

double calculateRouteTravelTime(const vector<int>& route, const vector<Node>& nodes, const TimeMatrix& time_matrix) {
    double total_time = 0.0;
    if (route.size() == 0)
        return total_time;
    
    total_time += travelTime(nodes[0], nodes[route[0]], time_matrix);

    for (size_t i = 0; i < route.size() - 1; ++i) {
        total_time += travelTime(nodes[route[i]], nodes[route[i + 1]], time_matrix);
    }

    total_time += travelTime(nodes[route.back()], nodes[0], time_matrix);

    return total_time;
}

double calculateTotalCost(const Solution& solution, const vector<Node>& nodes, const TimeMatrix& time_matrix) {
    double total_cost = 0.0;
    for (const auto& route : solution.routes) {
        total_cost += calculateRouteTravelTime(route, nodes, time_matrix);
    }
    return total_cost;
}

Solution generateInitialSolution(const vector<Node>& nodes, int num_vehicles, const TimeMatrix& time_matrix) {
    Solution initial_solution;
    initial_solution.routes.resize(num_vehicles);
    vector<int> unassigned_nodes(nodes.size() - 1);
    for (size_t i = 1; i < nodes.size(); ++i) {
        unassigned_nodes[i - 1] = nodes[i].id;
    }
    random_shuffle(unassigned_nodes.begin(), unassigned_nodes.end());

    int vehicle_idx = 0;
    for (size_t i = 0; i < unassigned_nodes.size(); ++i) {
        initial_solution.routes[vehicle_idx].push_back(unassigned_nodes[i]);
        vehicle_idx = (vehicle_idx + 1) % num_vehicles;
    }

    initial_solution.arrival_times.resize(nodes.size(), 0.0);
    for (int v = 0; v < num_vehicles; ++v) {
        double current_time = 0.0;
        for (size_t i = 0; i < initial_solution.routes[v].size(); ++i) {
            int node_id = initial_solution.routes[v][i];
            current_time += travelTime(nodes[0], nodes[node_id], time_matrix);
            initial_solution.arrival_times[node_id] = current_time;
            current_time = max(current_time, nodes[node_id].ready_time) + nodes[node_id].service_time;
        }
        current_time += travelTime(nodes[initial_solution.routes[v].back()], nodes[0], time_matrix);
        initial_solution.arrival_times[0] = current_time;
    }

    initial_solution.total_cost = calculateTotalCost(initial_solution, nodes, time_matrix);

    return initial_solution;
}

Solution generateNeighborSolution(const Solution& current_solution, const vector<Node>& nodes, const TimeMatrix& time_matrix) {
    Solution neighbor_solution = current_solution;

    int route1 = rand() % neighbor_solution.routes.size();
    int route2 = rand() % neighbor_solution.routes.size();
    while (route1 == route2) {
        route2 = rand() % neighbor_solution.routes.size();
    }

    if (neighbor_solution.routes[route1].size() > 0 && neighbor_solution.routes[route2].size() > 0) {
        int node1 = rand() % neighbor_solution.routes[route1].size();
        int node2 = rand() % neighbor_solution.routes[route2].size();

        int temp = neighbor_solution.routes[route1][node1];
        neighbor_solution.routes[route1][node1] = neighbor_solution.routes[route2][node2];
        neighbor_solution.routes[route2][node2] = temp;

        neighbor_solution.arrival_times.resize(nodes.size(), 0.0);
        for (int v = 0; v < neighbor_solution.routes.size(); ++v) {
            double current_time = 0.0;
            for (size_t i = 0; i < neighbor_solution.routes[v].size(); ++i) {
                int node_id = neighbor_solution.routes[v][i];
                current_time += travelTime(nodes[0], nodes[node_id], time_matrix);
                neighbor_solution.arrival_times[node_id] = current_time;
                current_time = max(current_time, nodes[node_id].ready_time) + nodes[node_id].service_time;
            }
            current_time += travelTime(nodes[neighbor_solution.routes[v].back()], nodes[0], time_matrix);
            neighbor_solution.arrival_times[0] = current_time;
        }

        neighbor_solution.total_cost = calculateTotalCost(neighbor_solution, nodes, time_matrix);
    }

    return neighbor_solution;
}

Solution simulatedAnnealing(const vector<Node>& nodes, int num_vehicles, const TimeMatrix& time_matrix) {
    Solution current_solution = generateInitialSolution(nodes, num_vehicles, time_matrix);
    Solution best_solution = current_solution;

    double temperature = INITIAL_TEMPERATURE;

    int iteration = 0;
    while (temperature > FINAL_TEMPERATURE && iteration < MAX_ITER) {
        Solution neighbor_solution = generateNeighborSolution(current_solution, nodes, time_matrix);

        double current_cost = current_solution.total_cost;
        double neighbor_cost = neighbor_solution.total_cost;
        double cost_difference = neighbor_cost - current_cost;

        if (cost_difference < 0 || exp(-cost_difference / temperature) > (rand() / (RAND_MAX + 1.0))) {
            current_solution = neighbor_solution;
            if (current_solution.total_cost < best_solution.total_cost) {
                best_solution = current_solution;
            }
        }

        temperature *= COOLING_RATE;

        iteration++;
    }

    return best_solution;
}

TimeMatrix initializeTimeMatrix(const vector<Node>& nodes) {
    TimeMatrix time_matrix;
    int n = nodes.size();
    time_matrix.travel_time.resize(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                time_matrix.travel_time[i][j] = distance(nodes[i], nodes[j]);
            }
        }
    }

    return time_matrix;
}

int main() {
    srand(time(0));

    vector<Node> nodes = {
        {0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {1, 5.0, 0.0, 1.0, 0.0, 10.0, 1.0},
        {2, 10.0, 0.0, 2.0, 0.0, 10.0, 1.0},
        {3, 0.0, 5.0, 1.0, 0.0, 10.0, 1.0},
        {4, 5.0, 5.0, 2.0, 0.0, 10.0, 1.0},
        {5, 10.0, 5.0, 1.0, 0.0, 10.0, 1.0}
    };

    int num_vehicles = 2;

    TimeMatrix time_matrix = initializeTimeMatrix(nodes);

    Solution best_solution = simulatedAnnealing(nodes, num_vehicles, time_matrix);

    cout << "Best solution:" << endl;
    cout << "Total cost: " << best_solution.total_cost << endl;
    cout << "Routes:" << endl;
    for (size_t i = 0; i < best_solution.routes.size(); ++i) {
        cout << "Route " << i + 1 << ": ";
        for (size_t j = 0; j < best_solution.routes[i].size(); ++j) {
            cout << best_solution.routes[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
