%% Vehicle Routing Problem with Time Windows (VRPTW)
num_customers = 20; 
num_vehicles = 3;    
capacity = 100;    
depot = [0, 0];      
customers = rand(num_customers, 2)*100;
distances = zeros(num_customers+1, num_customers+1);
for i = 1:num_customers
    for j = 1:num_customers
        distances(i,j) = norm(customers(i,:) - customers(j,:));
    end
    distances(i, num_customers+1) = norm(customers(i,:) - depot);
    distances(num_customers+1, i) = norm(customers(i,:) - depot);
end
for i = 1:num_customers
    distances(i, num_customers+1) = norm(customers(i,:) - depot);
    distances(num_customers+1, i) = norm(customers(i,:) - depot);
end
time_windows = randi([0, 50], num_customers, 2);  
initial_temperature = 100;
cooling_rate = 0.99;
iterations_per_temperature = 100;
initial_solution = cell(num_vehicles, 1);
for v = 1:num_vehicles
    initial_solution{v} = [1; randperm(num_customers)'; 1]; 
end
calculate_route_cost = @(route) sum(distances(sub2ind(size(distances), route(1:end-1), route(2:end))));
current_solution = initial_solution;
best_solution = initial_solution;
current_temperature = initial_temperature;
best_costs = Inf;
while current_temperature > 1
    for iter = 1:iterations_per_temperature
        neighbor_solution = current_solution;
        vehicle_idx = randi(num_vehicles);
        if numel(neighbor_solution{vehicle_idx}) > 3  
            swap_indices = randsample(2:numel(neighbor_solution{vehicle_idx})-1, 2);
            neighbor_solution{vehicle_idx}([swap_indices(1), swap_indices(2)]) = neighbor_solution{vehicle_idx}([swap_indices(2), swap_indices(1)]);
        end
        current_cost = sum(cellfun(calculate_route_cost, current_solution));
        neighbor_cost = sum(cellfun(calculate_route_cost, neighbor_solution));
        if neighbor_cost < current_cost || exp((current_cost - neighbor_cost) / current_temperature) > rand()
            current_solution = neighbor_solution;
            if neighbor_cost < best_costs
                best_solution = neighbor_solution;
                best_costs = neighbor_cost;
            end
        end
    end
    current_temperature = current_temperature * cooling_rate;
end
figure;
hold on;
colors = ['r', 'g', 'b', 'm', 'c', 'y'];
for v = 1:num_vehicles
    route = best_solution{v};
    plot(customers(route, 1), customers(route, 2), ['o-', colors(v)]);
end
plot(depot(1), depot(2), 'ks', 'MarkerSize', 10, 'MarkerFaceColor', 'k');
text(depot(1), depot(2), ' Depot');
title('VRPTW');
xlabel('X');
ylabel('Y');
grid on;
hold off;
disp('Best Solution:');
for v = 1:num_vehicles
    disp(['Vehicle ' num2str(v) ' route:']);
    disp(best_solution{v}');
    disp(['Cost: ' num2str(calculate_route_cost(best_solution{v}))]);
end
