%% Time Dependent Vehicle Routing Problem with Time Windows (TDVRPTW)
nNodes = 10; 
nodes = rand(nNodes, 2) * 100; 
demands = randi([1, 10], nNodes, 1); 
time_windows = randi([0, 100], nNodes, 2); 
travel_times = rand(nNodes, nNodes) * 50; 
MaxIter = 1000; 
InitialTemperature = 100; 
CoolingSchedule = 0.95;
best_solution = [];
best_cost = Inf;
initial_solution = cell(1, nNodes - 1);
for i = 1:length(initial_solution)
    initial_solution{i} = [1, randperm(nNodes-1) + 1, 1]; 
end
current_solution = initial_solution;
current_cost = computeTotalCost(current_solution, travel_times);
iter = 0;
temperature = InitialTemperature;
while iter < MaxIter && temperature > eps
    new_solution = perturbSolution(current_solution);
    new_cost = computeTotalCost(new_solution, travel_times);
    if new_cost < current_cost || rand < exp(-(new_cost - current_cost) / temperature)
        current_solution = new_solution;
        current_cost = new_cost;
        if current_cost < best_cost
            best_solution = current_solution;
            best_cost = current_cost;
        end
    end
    temperature = CoolingSchedule * temperature;
    iter = iter + 1;
end
function total_cost = computeTotalCost(solution, travel_times)
    total_cost = 0;
    for i = 1:length(solution)
        route = solution{i};
        for j = 1:length(route) - 1
            total_cost = total_cost + travel_times(route(j), route(j+1));
        end
    end
end
function new_solution = perturbSolution(solution)
    new_solution = solution;
    numRoutes = length(new_solution);
    if numRoutes > 1
        r1 = randi(numRoutes);
        r2 = randi(numRoutes);
        if r1 ~= r2
            idx1 = randi(length(new_solution{r1}) - 2) + 1;
            idx2 = randi(length(new_solution{r2}) - 2) + 1;
            temp = new_solution{r1}(idx1);
            new_solution{r1}(idx1) = new_solution{r2}(idx2);
            new_solution{r2}(idx2) = temp;
        end
    end
end
figure;
hold on;
scatter(nodes(:, 1), nodes(:, 2), 100, 'filled', 'MarkerFaceColor', 'k');
numRoutes = length(best_solution);
colors = hsv(numRoutes);
for r = 1:numRoutes
    route = best_solution{r};
    route_nodes = nodes(route, :);
    plot(route_nodes(:, 1), route_nodes(:, 2), 'Color', colors(r, :), 'LineWidth', 2);
    for j = 2:length(route)-1
        tw = time_windows(route(j), :);
        text(route_nodes(j, 1), route_nodes(j, 2), sprintf('TW: [%d, %d]', tw(1), tw(2)), ...
            'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'left', 'FontSize', 8);
    end
end
plot(nodes(1, 1), nodes(1, 2), 's', 'MarkerSize', 12, 'MarkerEdgeColor', 'k', 'MarkerFaceColor', 'g');
title('TDVRPTW');
xlabel('X');
ylabel('Y');
grid on;
axis equal;
legend('Nodes', 'Routes', 'Depot', 'Location', 'Best');
hold off;
