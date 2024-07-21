%% Stochastic Vehicle Routing Problem (SVRP)
NUM_VEHICLES = 3;
initialTemperature = 1000;
coolingRate = 0.95;
iterations = 10000;
coordinates = [
    10, 20;
    15, 25;
    30, 10;
    25, 15;
    40, 30;
    35, 35;
    20, 5;
    5, 15;
    10, 40
];
x = coordinates(:, 1);
y = coordinates(:, 2);
NUM_CUSTOMERS = size(coordinates, 1);
figure;
hold on;
scatter(x, y, 'filled', 'DisplayName', 'Customers');
scatter(x(1), y(1), 'filled', 'MarkerEdgeColor', 'r', 'MarkerFaceColor', 'r', 'DisplayName', 'Depot');
legend('show');
title('SVRP');
rng('shuffle');  
bestSolution = simulatedAnnealing(initialTemperature, coolingRate, iterations, NUM_CUSTOMERS, NUM_VEHICLES, x, y);
for v = 1:NUM_VEHICLES
    route = bestSolution.routes{v};
    if ~isempty(route)
        route = [1; route(:); 1]; 
        plot(x(route), y(route), '-o', 'LineWidth', 1.5, 'MarkerSize', 8, ...
            'DisplayName', sprintf('Vehicle %d', v));
    end
end
legend('show');
hold off;
function bestSolution = simulatedAnnealing(initialTemperature, coolingRate, iterations, NUM_CUSTOMERS, NUM_VEHICLES, x, y)
    DEPOT_INDEX = 1; 
    bestSolution.routes = cell(NUM_VEHICLES, 1);
    bestSolution.cost = inf;
    currentSolution = generateInitialSolution(NUM_CUSTOMERS, NUM_VEHICLES, x, y);
    temperature = initialTemperature;
    for i = 1:iterations
        neighborSolution = generateNeighborSolution(currentSolution, NUM_VEHICLES, x, y);
        deltaCost = neighborSolution.cost - currentSolution.cost;
        if deltaCost < 0 || exp(-deltaCost / temperature) > rand()
            currentSolution = neighborSolution;
        end
        if currentSolution.cost < bestSolution.cost
            bestSolution = currentSolution;
        end
        temperature = temperature * coolingRate;
    end
end
function initialSolution = generateInitialSolution(NUM_CUSTOMERS, NUM_VEHICLES, x, y)
    DEPOT_INDEX = 1; 
    initialSolution.routes = cell(NUM_VEHICLES, 1);
    initialSolution.cost = 0;
    initialSolution.routes{1} = [2, 5, 7];
    initialSolution.routes{2} = [3, 4];
    initialSolution.routes{3} = [6, 8, 9];
    for v = 1:NUM_VEHICLES
        route = initialSolution.routes{v};
        if ~isempty(route)
            prevNode = DEPOT_INDEX;
            for i = 1:length(route)
                customer = route(i);
                initialSolution.cost = initialSolution.cost + ...
                    euclideanDistance(x(prevNode), y(prevNode), x(customer), y(customer));
                prevNode = customer;
            end
            initialSolution.cost = initialSolution.cost + ...
                euclideanDistance(x(prevNode), y(prevNode), x(DEPOT_INDEX), y(DEPOT_INDEX));
        end
    end
end
function neighborSolution = generateNeighborSolution(currentSolution, NUM_VEHICLES, x, y)
    DEPOT_INDEX = 1; 
    neighborSolution = currentSolution;
    vehicle1 = randi(NUM_VEHICLES);
    vehicle2 = randi(NUM_VEHICLES);
    while vehicle1 == vehicle2
        vehicle2 = randi(NUM_VEHICLES);
    end
    route1 = neighborSolution.routes{vehicle1};
    route2 = neighborSolution.routes{vehicle2};
    if ~isempty(route1) && ~isempty(route2)
        customer1 = randi(length(route1));
        customer2 = randi(length(route2));
        temp = route1(customer1);
        route1(customer1) = route2(customer2);
        route2(customer2) = temp;
        neighborSolution.routes{vehicle1} = route1;
        neighborSolution.routes{vehicle2} = route2;
        neighborSolution.cost = 0;
        for v = 1:NUM_VEHICLES
            route = neighborSolution.routes{v};
            if ~isempty(route)
                prevNode = DEPOT_INDEX;
                for i = 1:length(route)
                    customer = route(i);
                    neighborSolution.cost = neighborSolution.cost + ...
                        euclideanDistance(x(prevNode), y(prevNode), x(customer), y(customer));
                    prevNode = customer;
                end
                neighborSolution.cost = neighborSolution.cost + ...
                    euclideanDistance(x(prevNode), y(prevNode), x(DEPOT_INDEX), y(DEPOT_INDEX));
            end
        end
    end
end
function distance = euclideanDistance(x1, y1, x2, y2)
    distance = sqrt((x1 - x2)^2 + (y1 - y2)^2);
end
