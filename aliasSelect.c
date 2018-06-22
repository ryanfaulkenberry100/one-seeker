/**
 * This file contains experimental code that implements the alias method for
 * efficient weighted selection. These functions are currently unused.
 */

#include "inc/config.h"

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
