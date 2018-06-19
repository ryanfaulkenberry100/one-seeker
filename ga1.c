#include <stdio.h>
#include <stdlib.h>

#define POPULATION_SIZE 20
#define CHROM_SIZE 20
#define GENERATIONS 20

typedef struct chromosome_ {
	int alleleSet[CHROM_SIZE];
	int fitness;
} chromosome;

/**
 * Returns the fitness value of a single chromosome
 * fitness(chrom) = the number of 1's in the provided chromosome
 */
int getFitness(chromosome* chrom) {
	int ret = 0;
	int i;
	for (i=0; i < CHROM_SIZE; i++) {
		ret += chrom[i];
	}
	return ret;
}

/**
 * Sets the fitness field of all chromosomes in the provided population
 */
void getPopulationFitness(chromosome* population, int popsize) {

	int i;
	for (i=0; i < popsize; i++) {
		population[i].fitness = getFitness(population[i].alleleSet);
	}
}

/**
 * Randomly generate popsize chromosomes of size CHROM_SIZE and put them in
 * the provided array
 */
void resetPopulation(chromosome* population, int popsize) {

	int i, j;
	for (i=0; i < popsize; i++) {
		for (j=0; j < CHROM_SIZE; j++) {
			population[i].alleleSet[j] = rand() % 2;
		}
	}
}

/**
 * Helper method for getAliasTables responsible for a single step of alias
 * table generation as described in
 * https://en.wikipedia.org/wiki/Alias_method#Table_generation
 */
void buildAlias(float* probTable, int* aliasTable, int* under, int* over, int* undercount, int* overcount) {

	// "Pop" 2 arbitrary indices from each container
	int u = under[undercount];
	int o = over[overcount];

	// "Allocate" unused space in the under value to the over value aliasing
	aliasTable[u] = o;
	// "Deallocate" extra space in the over value by decreasing its probability
	probTable[o] += probTable[u] - 1;

	// Since u has been aliased, we're done with that index
	undercount--;

	// Since propTable[o] changed, remove it from overcount and reassign it
	overcount--;
	if (probTable[o] < 1) {
		under[++undercount] = o;
	} else if (probTable[o] > 1) {
		over[++overcount] = o;
	}

	// Floating point roundoff error might have caused one container to empty before the other
	// Stragglers can be set to 1 with minimal error
	while (undercount != 0) {
		probTable[under[undercount--]] = 1;
	}
	while (overcount != 0) {
		probTable[over[overcount--]] = 1;
	}
}

/**
 * Build the probability and alias tables from the fitness value of each chromosome.
 * See https://en.wikipedia.org/wiki/Alias_method
 */
void getAliasTables(float* probTable, int* aliasTable, chromosome* population, int popsize) {

	int under[popsize];
	int over[popsize];
	int undercount = 0;
	int overcount = 0;

	int totalFitness = 0;

	int i;
	for (i=0; i < popsize; i++) {
		totalFitness += population[i].fitness;
	}

	// Generate first pass probability table and fill containers for alias assignment
	for (i=0; i < popsize; i++) {
		probTable[i] = population[i].fitness * popsize / totalFitness;

		// Track which indices of the prob table are <1, ==1, >1
		if (probTable[i] < 1) {
			under[undercount++] = i;
		} else if (probTable[i] > 1) {
			over[overcount++] = i;
		}
	}

	// Assign aliases and update probability table
	undercount--;
	overcount--;
	while (undercount != 0 && overcount != 0) {



	}

}

/**
 * Produce the next generation, using the Walker alias method for weighted selection,
 * such that the chromosome with greatest fitness is the most likely to reproduce.
 * Good explanation of alias method here: http://www.keithschwarz.com/darts-dice-coins/
 */
void generateOffspring(chromosome* population, chromosome* offspring, int popsize) {

	int selection1, selection2;
	int wholePart;
	float fracPart;
	float r;

	int aliasTable[popsize];
	float probabilityTable[popsize];

	 // Generate alias tables
	getAliasTables(probabilityTable, aliasTable, population, popsize);

	// Generate offspring
	int i;
	for (i=0; i < popsize; i++) {

		r = rand();
		wholePart = r * popsize;
		fracPart = r * popsize + 1 - i;

		if (fracPart < probabilityTable[wholePart]) {
			selection1 = wholePart;
		} else {
			selection1 = aliasTable[wholePart];
		}
	}

}

int main(int argc, char** argv) {

	chromosome population[POPULATION_SIZE];
	chromosome offspring[POPULATION_SIZE];

	// Generate population randomly
	resetPopulation(population, POPULATION_SIZE);

	// Iterate through generations
	int gen;
	for (gen=0; gen < GENERATIONS; gen++) {
		// Compute the fitness of all chromosomes
		getPopulationFitness(population, POPULATION_SIZE);

		// Generate offspring
		generateOffspring(population, offspring, POPULATION_SIZE);


	}
}
