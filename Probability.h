#pragma once
#ifndef PROBABILITY_MODEL
#define PROBABILITY_MODEL

#include <random>

class ProbabilityModel {

	public:
		double getProbability() {

			// std::uniform_real_distribution<double> uniformRealDistribution(0, 1);
			// std::default_random_engine randomEngine(time(NULL));
			// return uniformRealDistribution(randomEngine);

			// Getting a random double value
			return ((double) rand() / (RAND_MAX));
		}
};

#endif /* PROBABILITY_MODEL */