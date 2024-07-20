%% Multi-Depot Vehicle Routing Problem (MDVRP)
MAX_VEHICLES = 20;
MAX_CUSTOMERS = 50;
MAX_DISTANCE = 1000.0;
customers = struct('demand', zeros(1, MAX_CUSTOMERS), ...
                   'x', zeros(1, MAX_CUSTOMERS), ...
                   'y', zeros(1, MAX_CUSTOMERS));
vehicles = struct('capacity', 100, ...
                  'current_load', 0, ...
                  'depot', 0, ...
                  'route', {});
solution = struct('vehicles', repmat(vehicles, 1, MAX_VEHICLES), ...
                  'cost', 0.0);
for i = 1:MAX_CUSTOMERS
    customers(i).demand = randi([1, 10]);
    customers(i).x = rand() * MAX_DISTANCE;
    customers(i).y = rand() * MAX_DISTANCE;
end
num_depots = 3;
initial_temperature = 1000.0;
alpha = 0.95;
max_iterations = 1000;
solution = generateInitialSolution(customers, num_depots, solution, MAX_VEHICLES);
current_solution = solution;
best_solution = current_solution;
current_temperature = initial_temperature;
for iter = 1:max_iterations
    new_solution = current_solution;
    vehicle_idx = randi(MAX_VEHICLES);
    if ~isempty(new_solution.vehicles(vehicle_idx).route)
        customer_idx = randi(length(new_solution.vehicles(vehicle_idx).route));
        selected_customer = new_solution.vehicles(vehicle_idx).route(customer_idx);
        new_vehicle_idx = randi(MAX_VEHICLES);
        while new_vehicle_idx == vehicle_idx || ...
              new_solution.vehicles(new_vehicle_idx).current_load + customers(selected_customer).demand > new_solution.vehicles(new_vehicle_idx).capacity
            new_vehicle_idx = randi(MAX_VEHICLES);
        end
        new_solution.vehicles(vehicle_idx).route(customer_idx) = [];
        new_solution.vehicles(new_vehicle_idx).route = [new_solution.vehicles(new_vehicle_idx).route, selected_customer];
        new_solution.vehicles(new_vehicle_idx).current_load = new_solution.vehicles(new_vehicle_idx).current_load + customers(selected_customer).demand;
        new_solution = calculateCost(new_solution, customers, MAX_VEHICLES);
        rand_num = rand();
        delta_cost = new_solution.cost - current_solution.cost;
        if delta_cost < 0 || exp(-delta_cost / current_temperature) > rand_num
            current_solution = new_solution;
        end
        if current_solution.cost < best_solution.cost
            best_solution = current_solution;
        end
    end
    current_temperature = current_temperature * alpha;
end
fprintf('Best Solution:\n');
printSolution(best_solution, customers);
plotSolution(best_solution, customers, num_depots);
function solution = generateInitialSolution(customers, num_depots, solution, MAX_VEHICLES)
    for i = 1:MAX_VEHICLES
        solution.vehicles(i).capacity = 100;
        solution.vehicles(i).current_load = 0;
        solution.vehicles(i).depot = mod(i-1, num_depots) + 1;
        solution.vehicles(i).route = [];
    end
    customer_indices = randperm(length(customers));
    vehicle_idx = 1;
    for i = 1:length(customers)
        cust_idx = customer_indices(i);
        if solution.vehicles(vehicle_idx).current_load + customers(cust_idx).demand > solution.vehicles(vehicle_idx).capacity
            vehicle_idx = mod(vehicle_idx, MAX_VEHICLES) + 1; % Ensure cyclic assignment of vehicles
        end
        solution.vehicles(vehicle_idx).route = [solution.vehicles(vehicle_idx).route, cust_idx];
        solution.vehicles(vehicle_idx).current_load = solution.vehicles(vehicle_idx).current_load + customers(cust_idx).demand;
    end
    solution = calculateCost(solution, customers, MAX_VEHICLES);
end
function solution = calculateCost(solution, customers, MAX_VEHICLES)
    solution.cost = 0.0;
    for i = 1:MAX_VEHICLES
        if ~isempty(solution.vehicles(i).route)
            route = solution.vehicles(i).route;
            last_cust_idx = route(end);
            solution.cost = solution.cost + distance(customers(last_cust_idx), customers(solution.vehicles(i).depot));
            for j = 1:length(route)-1
                cust_idx1 = route(j);
                cust_idx2 = route(j+1);
                solution.cost = solution.cost + distance(customers(cust_idx1), customers(cust_idx2));
            end
        end
    end
end
function dist = distance(c1, c2)
    dist = sqrt((c1.x - c2.x)^2 + (c1.y - c2.y)^2);
end
function printSolution(solution, customers)
    fprintf('Total Cost: %.2f\n', solution.cost);
    for i = 1:length(solution.vehicles)
        fprintf('Vehicle %d (Depot %d) Route: ', i, solution.vehicles(i).depot);
        route = solution.vehicles(i).route;
        fprintf('%d ', route);
        fprintf('\n');
    end
end
function plotSolution(best_solution, customers, num_depots)
    figure;
    hold on;
    axis equal;
    title('Vehicle Routes for MDVRP');
    xlabel('X-coordinate');
    ylabel('Y-coordinate');
    depot_colors = {'r', 'g', 'b'};
    for i = 1:num_depots
        depot_customers = find([best_solution.vehicles.depot] == (i-1));
        for j = 1:length(depot_customers)
            depot_idx = depot_customers(j);
            route = best_solution.vehicles(depot_idx).route;
            if ~isempty(route)
                x = [customers(route).x];
                y = [customers(route).y];
                plot([x, x(1)], [y, y(1)], strcat(depot_colors{i}, '-o'));
            end
        end
    end
    for i = 1:length(customers)
        plot(customers(i).x, customers(i).y, 'ko', 'MarkerFaceColor', 'k');
    end
    legend('Depot 1', 'Depot 2', 'Depot 3', 'Customers', 'Location', 'Best');
    hold off;
end
