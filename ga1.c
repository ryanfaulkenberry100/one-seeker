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
 * Generates a probability table for each member of the given population
 * such that individuals with greater fitness have a higher probability of
 * being chosen in the rouletteSelect.
 */
void generateRouletteTable(chromosome* population, float* probTable) {

	int totalFitness = 0;
	int i;
	// Get sum of every individual's fitness
	for (i=0; i < POPULATION_SIZE; i++) {
		totalFitness += getFitness(population[i]);
	}

	int previousProbability = 0;
	for (i=0; i < POPULATION_SIZE; i++) {

	}
}

/**
 * Selects a member of the given population such that probability of
 * selection is a function of fitness, using a provided probability
 * table. Returns the index at which the chosen member may be found
 * in the population array.
 */
int rouletteSelect(chromosome* population, float* probTable) {



}

/**
 * Produce the next generation, such that the chromosome with greatest fitness
 * is the most likely to reproduce.
 */
void generateOffspring(chromosome* population, chromosome* offspring) {

	chromosome child1, child2;
	int locus;
	float probTable[POPULATION_SIZE];
	float r;

	generateRouletteTable(population, probTable);

	// Generate offspring for next generation
	int i;
	for (i=0; i < POPULATION_SIZE / 2; i++) {

		// Select two parents to produce offspring (with replacement)
		copyAlleleSet(&population[rouletteSelect(population)], &child1);
		copyAlleleSet(&population[rouletteSelect(population)], &child2);

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
