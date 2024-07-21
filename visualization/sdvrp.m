% Split Delivery Vehicle Routing Problem (SDVRP) 
NUM_CUSTOMERS = 50;
NUM_VEHICLES = 10;
VEHICLE_CAPACITY = 100;
COOLING_RATE = 0.99;
INITIAL_TEMPERATURE = 1000.0;
MAX_ITERATIONS = 10000;
rng('default');
customers = struct('demand', num2cell(randi([1 10], 1, NUM_CUSTOMERS)), ...
    'x', num2cell(randi([0 100], 1, NUM_CUSTOMERS)), ...
    'y', num2cell(randi([0 100], 1, NUM_CUSTOMERS)));
routes = cell(1, NUM_VEHICLES);
vehicle_capacity = repmat(VEHICLE_CAPACITY, 1, NUM_VEHICLES);
unvisited_customers = 1:NUM_CUSTOMERS;
[~, sorted_indices] = sort([customers(unvisited_customers).demand], 'descend');
unvisited_customers = unvisited_customers(sorted_indices);
for i = 1:NUM_CUSTOMERS
    customer = unvisited_customers(i);
    min_route_index = -1;
    min_increase_cost = inf;
    for v = 1:NUM_VEHICLES
        if vehicle_capacity(v) >= customers(customer).demand
            if isempty(routes{v})
                increase_cost = distance(1, customer, customers) + distance(customer, 1, customers); % Adjusted index from 0 to 1
            else
                increase_cost = distance(routes{v}(end), customer, customers) + distance(customer, 1, customers); % Adjusted index from 0 to 1
            end
            if increase_cost < min_increase_cost
                min_increase_cost = increase_cost;
                min_route_index = v;
            end
        end
    end
    if min_route_index ~= -1
        routes{min_route_index} = [routes{min_route_index} customer];
        vehicle_capacity(min_route_index) = vehicle_capacity(min_route_index) - customers(customer).demand;
    end
end
temperature = INITIAL_TEMPERATURE;
current_distance = evaluate_solution(routes, customers);
best_distance = current_distance;
best_solution = routes;
for iter = 1:MAX_ITERATIONS
    v1 = randi(NUM_VEHICLES);
    v2 = randi(NUM_VEHICLES);
    if v1 ~= v2 && ~isempty(routes{v1}) && ~isempty(routes{v2})
        idx1 = randi(length(routes{v1}));
        idx2 = randi(length(routes{v2}));
        temp_customer = routes{v1}(idx1);
        routes{v1}(idx1) = routes{v2}(idx2);
        routes{v2}(idx2) = temp_customer;
        new_distance = evaluate_solution(routes, customers);
        if new_distance < current_distance || exp((current_distance - new_distance) / temperature) > rand()
            current_distance = new_distance;
            if current_distance < best_distance
                best_distance = current_distance;
                best_solution = routes;
            end
        else
            temp_customer = routes{v1}(idx1);
            routes{v1}(idx1) = routes{v2}(idx2);
            routes{v2}(idx2) = temp_customer;
        end
    end
    temperature = temperature * COOLING_RATE;
end
figure;
hold on;
title('SDVRP');
scatter([customers.x], [customers.y], 'filled', 'MarkerEdgeColor', 'k', 'MarkerFaceColor', 'b');
text([customers.x], [customers.y], arrayfun(@(x) num2str(x), (1:NUM_CUSTOMERS)', 'UniformOutput', false), 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
colors = hsv(NUM_VEHICLES);
for v = 1:NUM_VEHICLES
    route = best_solution{v};
    route_x = [0];
    route_y = [0];
    for j = 1:length(route)
        customer_idx = route(j);
        route_x = [route_x customers(customer_idx).x];
        route_y = [route_y customers(customer_idx).y];
    end
    route_x = [route_x 0];
    route_y = [route_y 0];
    plot(route_x, route_y, '-o', 'Color', colors(v,:), 'MarkerFaceColor', colors(v,:), 'LineWidth', 1.5);
end
scatter(0, 0, 100, 'p', 'MarkerEdgeColor', 'k', 'MarkerFaceColor', 'g');
xlabel('X Coordinate');
ylabel('Y Coordinate');
xlim([-10 110]);
ylim([-10 110]);
grid on;
box on;
axis equal;
legend('Customers', 'Vehicle 1', 'Vehicle 2', 'Vehicle 3', 'Vehicle 4', ...
    'Vehicle 5', 'Vehicle 6', 'Vehicle 7', 'Vehicle 8', 'Vehicle 9', 'Vehicle 10', ...
    'Depot', 'Location', 'Best');
hold off;
function d = distance(i, j, customers)
    d = sqrt((customers(i).x - customers(j).x)^2 + (customers(i).y - customers(j).y)^2);
end
function total_distance = evaluate_solution(routes, customers)
    NUM_VEHICLES = length(routes);
    total_distance = 0;
    for v = 1:NUM_VEHICLES
        route = routes{v};
        if ~isempty(route)
            total_distance = total_distance + distance(1, route(1), customers); % Adjusted index from 0 to 1
            for i = 1:length(route) - 1
                total_distance = total_distance + distance(route(i), route(i+1), customers);
            end
            total_distance = total_distance + distance(route(end), 1, customers); % Adjusted index from 0 to 1
        end
    end
end
