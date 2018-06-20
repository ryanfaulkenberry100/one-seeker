#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ga1.h"

void printFloatArray(float* array, int len) {

	int i;
	fprintf(stdout, "[");
	for (i=0; i < len; i++) {
		fprintf(stdout, "%f,", array[i]);
	}
	fprintf(stdout, "]\n");
}

/**
 * Prints the population members to stdout
 */
void printPopulation(chromosome* pop) {

	int i,j;
	for (i=0; i < POPULATION_SIZE; i++) {
		fprintf(stdout, "[");
		for (j=0; j < CHROM_SIZE; j++) {
			fprintf(stdout, "%d", pop[i].alleleSet[j]);
		}
		fprintf(stdout, "], ");
	}
	fprintf(stdout, "\n");
}

/**
 * Helper function to copy the allele set of chromosome src to chromosome dest.
 */
void copyAlleleSet(chromosome* src, chromosome* dest) {

	int i;
	for (i=0; i < CHROM_SIZE; i++) {
		dest->alleleSet[i] = src->alleleSet[i];
	}
}

/**
 * Returns the fitness value of a single chromosome
 * fitness(chrom) = the number of 1's in the provided chromosome
 */
int getFitness(chromosome chrom) {

	int ret = 0;
	int i;
	for (i=0; i < CHROM_SIZE; i++) {
		ret += chrom.alleleSet[i];
	}
	if (STOP_ON_ALL_ONES) {
		// If we found an individual with perfect fitness, terminate
		if (ret == CHROM_SIZE) {
			printf("PERFECT SPECIMEN CREATED\n");
			exit(0);
		}
	}
	return ret;
}

/**
 * Sets the fitness field of all chromosomes in the provided population
 */
void getPopulationFitness(chromosome* population) {

	int i;
	for (i=0; i < POPULATION_SIZE; i++) {
		population[i].fitness = getFitness(population[i]);
	}
}

/**
 * Randomly generate popsize chromosomes of size CHROM_SIZE and put them in
 * the provided array
 */
void resetPopulation(chromosome* population) {

	int i, j;
	for (i=0; i < POPULATION_SIZE; i++) {
		for (j=0; j < CHROM_SIZE; j++) {
			population[i].alleleSet[j] = rand() % 2;
		}
	}
}

/**
 * Mutate the provided chromosome. Each allele (bit) has a MUTATION_RATE
 * chance of being mutated (flipped)
 */
void mutate(chromosome* chrom) {

	int i;
	float r;
	for (i=0; i < CHROM_SIZE; i++) {
		r = drand48();
		// If mutation hits, flip the allele
		if (r < MUTATION_RATE) {
			if (chrom->alleleSet[i] == 1) {
				chrom->alleleSet[i] = 0;
			} else {
				chrom->alleleSet[i] = 1;
			}
		}
	}

}

/**
 * Swap all alleles occurring on and after the provided locus between
 * the 2 chromosomes
 */
void crossover(chromosome* chrom1, chromosome* chrom2, int locus) {

	int temp, i;
	for (i=locus; i < CHROM_SIZE; i++) {
		temp = chrom1->alleleSet[i];
		chrom1->alleleSet[i] = chrom2->alleleSet[i];
		chrom2->alleleSet[i] = temp;
	}
}

/**
 * Given probability and alias tables, selects a member of the given
 * population using the alias method, and returns the index at which
 * that member can be found in the population array.
 */
int aliasSelect(float* probTable, int* aliasTable) {

	int wholePart;
	float fracPart;
	float randomProb;

	randomProb = drand48();
	wholePart = randomProb * POPULATION_SIZE; // Implicit truncation
	fracPart = randomProb * (float)POPULATION_SIZE - (float)wholePart; // Uniformly distributed over [0,1)

	if (fracPart < probTable[wholePart]) {
		return wholePart;
	} else {
		return aliasTable[wholePart];
	}
}

/**
 * Helper method for getTables responsible for alias
 * table generation as described in:
 * https://en.wikipedia.org/wiki/Alias_method#Table_generation
 *
 * params in: 		under, over, undercount, overcount
 * params in/out:	probTable, aliasTable
 */
void buildAliasTable(float* probTable, int* aliasTable, int* under, int* over, int undercount, int overcount) {


	// Go through all probTable entries which do not equal 1 and "alias" them
	while (undercount != 0 && overcount != 0) {
		// "Pop" 2 arbitrary indices from each container
		int u = under[undercount - 1];
		int o = over[overcount - 1];

		// "Allocate" unused space in the under value to the over value by aliasing it
		aliasTable[u] = o;
		// "Deallocate" extra space in the over value by decreasing its selection probability
		probTable[o] += probTable[u] - 1;

		// Since u has been aliased, we're done with that index
		undercount--;

		// Since propTable[o] changed, remove it from overcount and reassign it
		overcount--;
		if (probTable[o] < 1) {
			under[undercount++] = o;
		} else if (probTable[o] > 1) {
			over[overcount++] = o;
		}
	}

	// Floating point roundoff error might have caused one container to empty before the other
	// Stragglers can be set to 1 with minimal error
	while (undercount != 0) {
		probTable[under[--undercount]] = 1;
	}
	while (overcount != 0) {
		probTable[over[--overcount]] = 1;
	}
}

/**
 * Build the probability and alias tables from the fitness value of each chromosome.
 * See https://en.wikipedia.org/wiki/Alias_method
 *
 * params in:		population, popsize
 * params in/out:	probTable, aliasTable
 */
void getTables(float* probTable, int* aliasTable, chromosome* population) {

	int under[POPULATION_SIZE];
	int over[POPULATION_SIZE];
	int undercount = 0;
	int overcount = 0;

	int totalFitness = 0;

	int i;
	for (i=0; i < POPULATION_SIZE; i++) {
		totalFitness += population[i].fitness;
	}

	// Generate first pass probability table and fill under/over containers for alias assignment
	// After this loop terminates the average of all probTable entries will be ~1
	for (i=0; i < POPULATION_SIZE; i++) {
		probTable[i] = (float)(population[i].fitness * POPULATION_SIZE) / (float)totalFitness;

		if (DEBUG) {
			printf("probTable[%d]: %f\n", i, probTable[i]);
		}

		// Track which indices of the prob table are <1, ==1, >1
		if (probTable[i] < 1) {
			under[undercount++] = i;
		} else if (probTable[i] > 1) {
			over[overcount++] = i;
		}
	}

	if (DEBUG) {
		fprintf(stdout, "Entering buildAliasTable with args:\nprobTable: ");
		printFloatArray(probTable, POPULATION_SIZE);
		fprintf(stdout, "undercount: %d\novercount: %d\n", undercount, overcount);
	}

	buildAliasTable(probTable, aliasTable, under, over, undercount, overcount);
}

/**
 * Produce the next generation, using the Walker alias method for weighted selection,
 * such that the chromosome with greatest fitness is the most likely to reproduce.
 * Good explanation of alias method here: http://www.keithschwarz.com/darts-dice-coins/
 */
void generateOffspring(chromosome* population, chromosome* offspring) {

	chromosome child1, child2;
	int locus;
	float r;

	int aliasTable[POPULATION_SIZE];
	float probTable[POPULATION_SIZE];

	 // Generate alias tables
	getTables(probTable, aliasTable, population);

	// Generate offspring for next generation
	int i;
	for (i=0; i < POPULATION_SIZE / 2; i++) {

		// Select two parents to produce offspring (with replacement)
		copyAlleleSet(&population[aliasSelect(probTable, aliasTable)], &child1);
		copyAlleleSet(&population[aliasSelect(probTable, aliasTable)], &child2);

		// If "crossover" hits, swap all the parent's alleles on and after some random locus
		r = drand48();
		if (r < CROSSOVER_RATE) {

			// Randomly select a locus
			locus = rand() % CHROM_SIZE;

			if (INFO) {
				printf("Mating: [");

				for (int i=0; i < CHROM_SIZE; i++) {
					printf("%d", child1.alleleSet[i]);
				}
				printf("], [");
				for (int i=0; i < CHROM_SIZE; i++) {
					printf("%d", child2.alleleSet[i]);
				}
				printf("] from locus %d\n", locus);
			}

			// Swap the alleles of the parents
			crossover(&child1, &child2, locus);

			if (INFO) {
				printf("Children: [");

				for (int i=0; i < CHROM_SIZE; i++) {
					printf("%d", child1.alleleSet[i]);
				}
				printf("], [");
				for (int i=0; i < CHROM_SIZE; i++) {
					printf("%d", child2.alleleSet[i]);
				}
				printf("]\n");
			}
		}
		// MUTATE THE CHILDREN!!!
		mutate(&child1);
		mutate(&child2);

		// Add the new children to the offspring group
		offspring[i] = child1;
		offspring[i + POPULATION_SIZE / 2] = child2;
	}
}

int main() {

	srand(time(NULL));

	chromosome population[POPULATION_SIZE];
	chromosome offspring[POPULATION_SIZE];

	// Generate population randomly
	resetPopulation(population);
	if (PRINT) {
		fprintf(stdout, "Generation 0:\n");
		printPopulation(population);
	}

	// Iterate through generations
	int gen;
	for (gen=0; gen < GENERATIONS; gen++) {
		// Compute the fitness of all chromosomes
		getPopulationFitness(population);

		// Generate offspring
		generateOffspring(population, offspring);

		if (PRINT) {
			fprintf(stdout, "Generation %d:\n", gen+1);
			printPopulation(offspring);
		}

		// Make offspring the new population (kill off the old people)
		copyAlleleSet(offspring, population);
	}
}
