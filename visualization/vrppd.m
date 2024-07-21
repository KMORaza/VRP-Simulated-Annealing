%% Vehicle Routing Problem with Pick-up and Delivery (VRPPD) 
num_locations = 10;  
num_vehicles = 3;    
capacity = 10;       
locations = rand(num_locations, 2)*10;
distances = zeros(num_locations);
for i = 1:num_locations
    for j = 1:num_locations
        distances(i,j) = norm(locations(i,:) - locations(j,:));
    end
end
demands = [0 randi([1 5], 1, num_locations-1)];  
initial_temperature = 100;
cooling_rate = 0.95;
iterations_per_temperature = 1000;
current_solution = cell(num_vehicles, 1);
for v = 1:num_vehicles
    current_solution{v} = [1 randperm(num_locations-1) + 1]; 
end
calculate_cost = @(solution) calculate_total_distance(solution, distances);
temperature = initial_temperature;
best_solution = current_solution;
best_cost = calculate_cost(best_solution);
while temperature > 1
    for i = 1:iterations_per_temperature
        new_solution = generate_neighbor(current_solution, num_vehicles);
        new_cost = calculate_cost(new_solution);
        if new_cost < best_cost
            best_solution = new_solution;
            best_cost = new_cost;
        end
        delta_cost = new_cost - calculate_cost(current_solution);
        if delta_cost < 0 || exp(-delta_cost / temperature) > rand()
            current_solution = new_solution;
        end
    end
    temperature = temperature * cooling_rate;
end
function new_solution = generate_neighbor(solution, num_vehicles)
    new_solution = solution;
    vehicle1 = randi(num_vehicles);
    vehicle2 = randi(num_vehicles);
    if vehicle1 ~= vehicle2 && ~isempty(solution{vehicle1}) && ~isempty(solution{vehicle2})
        idx1 = randi(length(solution{vehicle1}));
        idx2 = randi(length(solution{vehicle2}));
        temp = new_solution{vehicle1}(idx1);
        new_solution{vehicle1}(idx1) = new_solution{vehicle2}(idx2);
        new_solution{vehicle2}(idx2) = temp;
    end
end
function total_distance = calculate_total_distance(solution, distances)
    total_distance = 0;
    for v = 1:length(solution)
        route = solution{v};
        if ~isempty(route)
            route_distance = 0;
            for j = 1:length(route)-1
                route_distance = route_distance + distances(route(j), route(j+1));
            end
            total_distance = total_distance + route_distance;
        end
    end
end
figure;
hold on;
title('VRPPD');
xlabel('X');
ylabel('Y');
plot(locations(1, 1), locations(1, 2), 'ko', 'MarkerSize', 10, 'MarkerFaceColor', 'k');
scatter(locations(2:end, 1), locations(2:end, 2), 'filled');
colors = hsv(num_vehicles); 
for v = 1:num_vehicles
    route = best_solution{v};
    if ~isempty(route)
        x = locations([1 route 1], 1);
        y = locations([1 route 1], 2);
        plot(x, y, '-o', 'Color', colors(v,:), 'MarkerFaceColor', colors(v,:), 'LineWidth', 1.5);
    end
end
legend('Depot', 'Locations', 'Routes');
axis equal;
hold off;
