/**
 * Program configurations
 */
// This value must be even
#define POPULATION_SIZE 100
// Number of alleles in a chromosome
#define CHROM_SIZE 20
#define GENERATIONS 40
#define CROSSOVER_RATE 0.7
#define MUTATION_RATE 0.001
// Terminates the program if an individual with perfect fitness is born
#define STOP_ON_ALL_ONES 1

/**
 * Print configurations
 */
// Print every member of every generation
#define PRINT 1
// Print information about the population of each generation
#define INFO  0
// Print debug info
#define DEBUG 0


typedef struct chromosome_ {
	int alleleSet[CHROM_SIZE];
	int fitness;
} chromosome;
