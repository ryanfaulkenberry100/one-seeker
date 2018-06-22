/**
 * This file contains custom configurations for the user to modify certain
 * parameters of the algorithm
 */

/**
 * Number of chromosomes in the population. This value must be even.
 */
#define POPULATION_SIZE 100

/**
 * Number of alleles in a chromosome.
 */
#define CHROM_SIZE 20

/**
 * Number of times the population will reproduce
 */
#define GENERATIONS 40

/**
 * The rate at which mating pairs of chromosomes will swap a subset of their alleles.
 */
#define CROSSOVER_RATE 0.7

/**
 * When a child chromosome is produced, each of its alleles has this chance of being
 * flipped to its opposite value.
 */
#define MUTATION_RATE 0.001

/**
 * If true, terminates the program when a chromosome with perfect fitness is
 * produced and prints a notification.
 */
#define STOP_ON_ALL_ONES 1

/**
 * If true, prints simple information about each generation.
 */
#define PRINT 1

/**
 * If true, prints detailed information about each generation.
 */
#define INFO  0

/**
 * If true, prints debug information.
 */
#define DEBUG 0
