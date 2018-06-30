# one-seeker
A simple genetic algorithm over a randomly generated population of binary strings, in which the fitness of a member is measured by the number of "1"s in its string

<h3>Use:</h3>

The user of this program can configure and observe the evolution of the population by setting constants in config.h.

* PRINT, INFO, and DEBUG control what kind of output is printed to the terminal. PRINT prints basic information about each generation, INFO prints more detailed information about the population, including a printout of the state of every single member, and DEBUG prints esoteric information about probability table generation, etc.

* CROSSOVER_RATE is the chance that any two parents selected for reproduction swap bits from a randomly selected point. If parents are selected for crossover, an index from their bit array is selected, and the bits including and to the right of that index are swapped between parents.

The other variables should be self-explanatory.
