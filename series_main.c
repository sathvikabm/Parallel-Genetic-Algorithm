int main() {
    //cudaSetDevice(0);

    //cudaError_t err = cudaSuccess;

    int max_val = 250;
    clock_t begin = clock();

    //initialising 1D array of cost of ith city to jth city called citymap
    float citymap[num_cities*num_cities];

    int* population = (int*)calloc(ISLANDS*num_cities, sizeof(int));
    float* population_fitness = (float*)calloc(ISLANDS, sizeof(float));
    float* population_cost = (float*)calloc(ISLANDS, sizeof(float));

    printf("Num islands: %d\n", ISLANDS);
    printf("Population size: %d\n", ISLANDS*num_cities);

    //building cost table (citymap)
    for(int i=0; i<num_cities; i++) {
        for(int j=0; j<num_cities; j++) {
            if(i!=j) {
                citymap[i*num_cities+j] = L2distance(city_x[i], city_y[i], city_x[j], city_y[j]);
            } else {
                citymap[i*num_cities+j] = max_val * max_val;
            }
        }
    }

    initalizeRandomPopulation(population, population_cost, population_fitness, citymap);

    int fittest = getFittestScore(population_fitness);
    printf("min distance: %f\n", population_cost[fittest]);

    // Device Variables
    int* population_d;
    float* population_fitness_d;
    float* population_cost_d;
    int* parent_cities_d;
    float* citymap_d;
    //curandState *states_d;

    /*float milliseconds;
    cudaEvent_t start, stop;
    cudaEventCreate (&start);
    cudaEventCreate (&stop);
    cudaEventRecord (start);*/

    //cudaMalloc((void **)&population_d, ISLANDS*num_cities*sizeof(int));
    //cudaMalloc((void **)&population_cost_d, ISLANDS*sizeof(float));
    //cudaMalloc((void **)&population_fitness_d, ISLANDS*sizeof(float));
    //cudaMalloc((void **)&parent_cities_d, 2*ISLANDS*num_cities*sizeof(int));
    //cudaMalloc((void **)&citymap_d, num_cities*num_cities*sizeof(float));
    //cudaMalloc((void **)&states_d, ISLANDS*sizeof(curandState));

    malloc((void **)&population_d, ISLANDS*num_cities*sizeof(int));
     malloc((void **)&population_cost_d, ISLANDS*sizeof(float));
     malloc((void **)&population_fitness_d, ISLANDS*sizeof(float));
     malloc((void **)&parent_cities_d, 2*ISLANDS*num_cities*sizeof(int));
     malloc((void **)&citymap_d, num_cities*num_cities*sizeof(float));
    //malloc((void **)&states_d, ISLANDS*sizeof(curandState));

    //cudaMemcpy(population_d, population, ISLANDS*num_cities*sizeof(int), cudaMemcpyHostToDevice);
    //cudaMemcpy(population_cost_d, population_cost, ISLANDS*sizeof(float), cudaMemcpyHostToDevice);
    //cudaMemcpy(population_fitness_d, population_fitness, ISLANDS*sizeof(float), cudaMemcpyHostToDevice);
    //cudaMemcpy(citymap_d, citymap, num_cities*num_cities*sizeof(float), cudaMemcpyHostToDevice);

    init/*<<<num_blocks, num_threads>>>*/(states_d);
      /*   err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Init Kernel: %s\n", cudaGetErrorString(err));
        exit(0);
    } */

    // Get initial fitness of population
    getPopulationFitness/*<<<num_blocks, num_threads>>>*/(
            population_d, population_cost_d, population_fitness_d, citymap_d);
    /*err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Fitness Kernel: %s\n", cudaGetErrorString(err));
        exit(0);
    }*/


    for(int i = 0; i < num_generations; i++ ) {

        selection/*<<<num_blocks, num_threads>>>*/(
                population_d, population_cost_d, population_fitness_d, parent_cities_d, states_d);
        //cudaDeviceSynchronize();
       /* err = cudaGetLastError();
        if (err != cudaSuccess) {
            fprintf(stderr, "Selection Kernel: %s\n", cudaGetErrorString(err));
            exit(0);
        }*/

        for (int j = 1; j < num_cities; j++){
            crossover/*<<<num_blocks, num_threads>>>*/(population_d, population_cost_d, population_fitness_d, parent_cities_d, states_d, citymap_d, j); 
            //printf("%d", j);
            //cudaDeviceSynchronize();
            /*err = cudaGetLastError();
            if (err != cudaSuccess) {
                fprintf(stderr, "Crossover Kernel: %s\n", cudaGetErrorString(err));
                exit(0);
            }*/

        }

        mutation/*<<<num_blocks, num_threads>>>*/(
                population_d, population_cost_d, population_fitness_d, states_d);
        //cudaDeviceSynchronize();
        err = cudaGetLastError();
        /*if (err != cudaSuccess) {
            fprintf(stderr, "Mutation Kernel: %s\n", cudaGetErrorString(err));
            exit(0);
        }*/

        getPopulationFitness/*<<<num_blocks, num_threads>>>*/(
                population_d, population_cost_d, population_fitness_d, citymap_d);
        //cudaDeviceSynchronize();
        /*err = cudaGetLastError();
        if (err != cudaSuccess) {
            fprintf(stderr, "Mutation Kernel: %s\n", cudaGetErrorString(err));
            exit(0);
        }*/

        // Print things for sanity check
        if(i > 0 && i % print_interval == 0) {
            /*cudaMemcpy(population_fitness, population_fitness_d,  ISLANDS*sizeof(float), cudaMemcpyDeviceToHost);
            cudaMemcpy(population_cost, population_cost_d,  ISLANDS*sizeof(float), cudaMemcpyDeviceToHost);
            fittest = getFittestScore(population_fitness);*/
            printf("Iteration:%d, min distance: %f\n", i, population_cost[fittest]);
        }
    }

    /*cudaEventRecord (stop);
    cudaEventSynchronize (stop);
    cudaEventElapsedTime (&milliseconds, start, stop);

    cudaMemcpy(population, population_d,  ISLANDS*num_cities*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(population_fitness, population_fitness_d,  ISLANDS*sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(population_cost, population_cost_d,  ISLANDS*sizeof(float), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();*/

    clock_t end = clock();
double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    fittest = getFittestScore(population_fitness);
    printf("time: %f s,  min distance: %f\n",time_spent, population_cost[fittest]);

    /*cudaFree(population_d);
    cudaFree(population_fitness_d);
    cudaFree(population_cost_d);
    cudaFree(parent_cities_d);
    cudaFree(citymap_d);
    cudaFree(states_d);*/

    return 0;
}