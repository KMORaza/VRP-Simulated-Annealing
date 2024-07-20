%% Period Vehicle Routing Problem (PVRP)
NUM_CUSTOMERS = 20;
NUM_VEHICLES = 4;
VEHICLE_CAPACITY = 10;
MAX_ITER = 10000;
INITIAL_TEMP = 100.0;
COOLING_RATE = 0.003;
PERIOD_LENGTH = 7;
customers = struct('id', [], 'demand', []);
for i = 1:NUM_CUSTOMERS
    customers(i).id = i;
    customers(i).demand = randi([1, 5]); 
end
calculate_distance = @(c1, c2) abs(c1.id - c2.id);
function route_distance = calculate_route_distance(route, customers, calculate_distance)
    route_distance = 0;
    for i = 1:length(route)-1
        route_distance = route_distance + calculate_distance(customers(route(i)), customers(route(i+1)));
    end
    route_distance = route_distance + calculate_distance(customers(route(end)), customers(route(1)));
end
function initial_solution = generate_initial_solution(customers, NUM_VEHICLES, VEHICLE_CAPACITY, calculate_distance)
    initial_solution = cell(1, NUM_VEHICLES);
    visited = false(1, length(customers));
    visited(1) = true; 
    current_vehicle = 1;
    current_capacity = 0;
    current_customer = 1; 
    while true
        nearest_customer = -1;
        min_distance = inf;
        for i = 2:length(customers)
            if ~visited(i) && customers(i).demand + current_capacity <= VEHICLE_CAPACITY
                dist = calculate_distance(customers(current_customer), customers(i));
                if dist < min_distance
                    min_distance = dist;
                    nearest_customer = i;
                end
            end
        end
        if nearest_customer == -1
            current_vehicle = current_vehicle + 1;
            if current_vehicle > NUM_VEHICLES
                break;
            end
            current_capacity = 0;
            current_customer = 1; 
        else
            initial_solution{current_vehicle} = [initial_solution{current_vehicle}, nearest_customer];
            visited(nearest_customer) = true;
            current_capacity = current_capacity + customers(nearest_customer).demand;
            current_customer = nearest_customer;
        end
    end
end
function best_solution = simulated_annealing(customers, NUM_VEHICLES, MAX_ITER, INITIAL_TEMP, COOLING_RATE, VEHICLE_CAPACITY, calculate_distance, calculate_route_distance)
    current_solution = generate_initial_solution(customers, NUM_VEHICLES, VEHICLE_CAPACITY, calculate_distance);
    best_solution = current_solution;
    temperature = INITIAL_TEMP;
    for iter = 1:MAX_ITER
        new_solution = current_solution;
        vehicle1 = randi(NUM_VEHICLES);
        vehicle2 = randi(NUM_VEHICLES);
        if vehicle1 ~= vehicle2 && ~isempty(new_solution{vehicle1}) && ~isempty(new_solution{vehicle2})
            rand_index1 = randi(length(new_solution{vehicle1}));
            rand_index2 = randi(length(new_solution{vehicle2}));
            temp = new_solution{vehicle1}(rand_index1);
            new_solution{vehicle1}(rand_index1) = new_solution{vehicle2}(rand_index2);
            new_solution{vehicle2}(rand_index2) = temp;
        end
        current_cost = 0;
        new_cost = 0;
        for v = 1:NUM_VEHICLES
            current_cost = current_cost + calculate_route_distance(current_solution{v}, customers, calculate_distance);
            new_cost = new_cost + calculate_route_distance(new_solution{v}, customers, calculate_distance);
        end
        if new_cost < current_cost
            current_solution = new_solution;
        else
            acceptance_probability = exp((current_cost - new_cost) / temperature);
            if rand < acceptance_probability
                current_solution = new_solution;
            end
        end
        if calculate_route_distance(current_solution{1}, customers, calculate_distance) < calculate_route_distance(best_solution{1}, customers, calculate_distance)
            best_solution = current_solution;
        end
        temperature = temperature * (1 - COOLING_RATE);
    end
end
function plot_routes(customers, solution, NUM_VEHICLES)
    figure;
    hold on;
    title('PVRP');
    xlabel('X Coordinate');
    ylabel('Y Coordinate');
    plot(customers(1).id, 0, 'ko', 'MarkerSize', 10, 'LineWidth', 2);
    text(customers(1).id, 0, 'Depot', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right');
    for i = 2:length(customers)
        plot(customers(i).id, 0, 'bo', 'MarkerSize', 8, 'LineWidth', 1);
    end
    colors = {'r', 'g', 'b', 'm', 'c'};
    for v = 1:NUM_VEHICLES
        route = solution{v};
        if ~isempty(route)
            x_coords = [customers(1).id, [customers(route).id]];
            y_coords = zeros(1, length(x_coords));
            plot(x_coords, y_coords, '-o', 'Color', colors{v}, 'LineWidth', 1.5, 'MarkerSize', 8);
        end
    end
    legend_entries = cell(1, NUM_VEHICLES);
    for v = 1:NUM_VEHICLES
        legend_entries{v} = sprintf('Vehicle %d', v);
    end
    legend(['Depot', 'Customers', legend_entries], 'Location', 'best');
    hold off;
end
best_solutions = cell(PERIOD_LENGTH, 1);
execution_times = zeros(PERIOD_LENGTH, 1);
total_distances = zeros(PERIOD_LENGTH, 1);
for period = 1:PERIOD_LENGTH
    tic;
    best_solutions{period} = simulated_annealing(customers, NUM_VEHICLES, MAX_ITER, INITIAL_TEMP, COOLING_RATE, VEHICLE_CAPACITY, calculate_distance, @calculate_route_distance);
    execution_times(period) = toc;
    fprintf('Best solution found for Day %d:\n', period);
    for v = 1:NUM_VEHICLES
        fprintf('Vehicle %d: ', v);
        fprintf('%d ', best_solutions{period}{v});
        fprintf('\n');
    end
    total_distance = 0;
    for v = 1:NUM_VEHICLES
        total_distance = total_distance + calculate_route_distance(best_solutions{period}{v}, customers, calculate_distance);
    end
    total_distances(period) = total_distance;
    fprintf('Total Distance for Day %d: %f\n', period, total_distance);
    fprintf('Execution Time for Day %d: %f seconds\n', period, execution_times(period));
    plot_routes(customers, best_solutions{period}, NUM_VEHICLES);
end
