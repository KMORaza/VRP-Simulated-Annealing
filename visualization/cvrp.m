%% Capacited Vehicle Routing Problem (CVRP) 
function cvrp()
    NUM_CUSTOMERS = 20;
    NUM_VEHICLES = 5;
    CAPACITY = 100;
    MAX_ITERATIONS = 10000;
    INIT_TEMPERATURE = 1000.0;
    COOLING_RATE = 0.003;
    distance = @(cust1, cust2) sqrt((cust1.x - cust2.x)^2 + (cust1.y - cust2.y)^2);
    function [customers, depot_x, depot_y] = generateProblem()
        customers = struct('demand', zeros(NUM_CUSTOMERS, 1), 'x', zeros(NUM_CUSTOMERS, 1), 'y', zeros(NUM_CUSTOMERS, 1));
        depot_x = randi(100);
        depot_y = randi(100);
        for i = 1:NUM_CUSTOMERS
            customers(i).demand = randi(10); % Demand between 1 and 10
            customers(i).x = randi(100);
            customers(i).y = randi(100);
        end
    end
    function total_distance = calculateTotalDistance(routes, customers, depot)
        total_distance = 0;
        for r = 1:numel(routes)
            route = routes{r};
            if isempty(route)
                continue;
            end
            total_distance = total_distance + distance(depot, customers(route(1)));
            for i = 1:numel(route)-1
                total_distance = total_distance + distance(customers(route(i)), customers(route(i+1)));
            end
            total_distance = total_distance + distance(customers(route(end)), depot);
        end
    end
    function initial_solution = generateInitialSolution(customers)
        initial_solution = cell(1, NUM_VEHICLES);
        customer_indices = randperm(NUM_CUSTOMERS);
        vehicle_index = 1;
        for i = 1:NUM_CUSTOMERS
            if mod(i, NUM_CUSTOMERS / NUM_VEHICLES) == 1 && vehicle_index <= NUM_VEHICLES
                vehicle_index = vehicle_index + 1;
            end
            initial_solution{vehicle_index - 1} = [initial_solution{vehicle_index - 1}, customer_indices(i)];
        end
    end
    function neighbor_solution = generateNeighborSolution(current_solution)
        neighbor_solution = current_solution;
        route1 = randi(NUM_VEHICLES);
        route2 = randi(NUM_VEHICLES);
        if isempty(neighbor_solution{route1}) || isempty(neighbor_solution{route2})
            return;
        end
        cust_index1 = randi(numel(neighbor_solution{route1}));
        cust_index2 = randi(numel(neighbor_solution{route2}));
        temp = neighbor_solution{route1}(cust_index1);
        neighbor_solution{route1}(cust_index1) = neighbor_solution{route2}(cust_index2);
        neighbor_solution{route2}(cust_index2) = temp;
    end
    function cost = evaluateSolution(solution, customers, depot)
        cost = calculateTotalDistance(solution, customers, depot);
    end
    function accept = acceptNeighbor(current_cost, new_cost, temperature)
        if new_cost < current_cost
            accept = true;
        else
            acceptance_prob = exp((current_cost - new_cost) / temperature);
            random_prob = rand;
            accept = random_prob < acceptance_prob;
        end
    end
    function temperature = updateTemperature(temperature)
        temperature = temperature * (1 - COOLING_RATE);
    end
    function main()
        [customers, depot_x, depot_y] = generateProblem();
        temperature = INIT_TEMPERATURE;
        current_solution = generateInitialSolution(customers);
        current_cost = evaluateSolution(current_solution, customers, struct('x', depot_x, 'y', depot_y));
        best_solution = current_solution;
        best_cost = current_cost;
        for iter = 1:MAX_ITERATIONS
            neighbor_solution = generateNeighborSolution(current_solution);
            neighbor_cost = evaluateSolution(neighbor_solution, customers, struct('x', depot_x, 'y', depot_y));
            if acceptNeighbor(current_cost, neighbor_cost, temperature)
                current_solution = neighbor_solution;
                current_cost = neighbor_cost;
            end
            if current_cost < best_cost
                best_solution = current_solution;
                best_cost = current_cost;
            end
            temperature = updateTemperature(temperature);
        end
        disp('Best solution found:');
        for i = 1:numel(best_solution)
            fprintf('Route %d: ', i);
            fprintf('%d ', best_solution{i});
            fprintf('\n');
        end
        total_distance = calculateTotalDistance(best_solution, customers, struct('x', depot_x, 'y', depot_y));
        fprintf('Total distance traveled: %.2f\n', total_distance);
        figure;
        hold on;
        axis equal;
        xlim([0 100]);
        ylim([0 100]);
        scatter(depot_x, depot_y, 'filled', 'MarkerFaceColor', 'r'); 
        colors = ['b', 'g', 'c', 'm', 'y'];
        for i = 1:numel(best_solution)
            route = best_solution{i};
            if ~isempty(route)
                route = [route, route(1)]; 
                plot([customers(route).x], [customers(route).y], '-o', 'Color', colors(mod(i, numel(colors)) + 1));
            end
        end
        title('Capacited Vehicle Routing Problem (CVRP)');
        xlabel('X');
        ylabel('Y');
        legend('Depot', 'Route 1', 'Route 2', 'Route 3', 'Route 4', 'Route 5', 'Location', 'Best');
        hold off;
    end
    main();
end
