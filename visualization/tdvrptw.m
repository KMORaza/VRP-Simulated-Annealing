%% Time Dependent Vehicle Routing Problem with Time Windows (TDVRPTW)nodes = [
num_customers = 10;
num_vehicles = 3;  
max_capacity = 100;  
depot = [0, 0];      
rng(1); 
customer_locations = 10 * rand(num_customers, 2);  
customer_demands = randi([1, 10], num_customers, 1); 
time_windows = randi([0, 8], num_customers, 2);  
vehicle_routes = cell(num_vehicles, 1);
for i = 1:num_vehicles
    route_length = randi([2, num_customers]);  
    vehicle_routes{i} = [1, randperm(num_customers, route_length - 1), 1]; 
end
figure;
hold on;
title('TDVRPTW');
plot(depot(1), depot(2), 'ko', 'MarkerSize', 10, 'MarkerFaceColor', 'k');
text(depot(1), depot(2), ' Depot', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
plot(customer_locations(:, 1), customer_locations(:, 2), 'bo', 'MarkerSize', 8, 'MarkerFaceColor', 'b');
for i = 1:num_customers
    text(customer_locations(i, 1), customer_locations(i, 2), sprintf(' %d', i), 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
end
colors = {'r', 'g', 'm', 'c', 'y'};
for i = 1:num_vehicles
    route = vehicle_routes{i};
    route_locations = [depot; customer_locations(route, :); depot];
    plot(route_locations(:, 1), route_locations(:, 2), '-o', 'Color', colors{i}, 'LineWidth', 1.5);
    for j = 1:length(route)
        text(route_locations(j, 1), route_locations(j, 2), sprintf(' %d', route(j)), 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'left');
    end
end
for i = 1:num_customers
    start_time = time_windows(i, 1);
    end_time = time_windows(i, 2);
    plot(customer_locations(i, 1), customer_locations(i, 2), 'ks', 'MarkerSize', 8, 'MarkerFaceColor', 'w');
    text(customer_locations(i, 1), customer_locations(i, 2), sprintf(' TW: [%d, %d]', start_time, end_time), 'VerticalAlignment', 'top', 'HorizontalAlignment', 'left');
end
legend_strings = [{'Depot'}, arrayfun(@(x) sprintf('Customer %d', x), 1:num_customers, 'UniformOutput', false), arrayfun(@(x) sprintf('Vehicle %d', x), 1:num_vehicles, 'UniformOutput', false)];
legend(legend_strings, 'Location', 'Best');
hold off;
axis equal;
xlabel('X');
ylabel('Y');
