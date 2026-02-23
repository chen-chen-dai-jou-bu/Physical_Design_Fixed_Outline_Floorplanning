#pragma once
#ifndef SOFT_MODULE
#define SOFT_MODULE

#include <string>
#include <random>

#include "SettingParameters.h"
#include "Module.h"

class SoftModule : public Module{

	private:
		unsigned long minimumArea;
		double minimumAspectRatio;
		double maximumAspectRatio;

	public:

		SoftModule(std::string _name, unsigned long _minimumArea, double _minimumAspectRatio, double _maximumAspectRatio) : Module(_name) {
			this->minimumArea = _minimumArea;
			this->minimumAspectRatio = _minimumAspectRatio;
			this->maximumAspectRatio = _maximumAspectRatio;
		}

		SoftModule(SoftModule* _module) : Module(_module) {
			this->minimumArea = _module->getMinimumArea();
			this->minimumAspectRatio = _module->getMinimumAspectRatio();
			this->maximumAspectRatio = _module->getMaximumAspectRatio();
		}

		void setMinimumArea(unsigned long _minimumArea) {
			this->minimumArea = _minimumArea;
		}

		unsigned long getMinimumArea() {
			return this->minimumArea;
		}

		double getMinimumAspectRatio() {
			return this->minimumAspectRatio;
		}

		double getMaximumAspectRatio() {
			return this->maximumAspectRatio;
		}

		void setRandomAsepectRatio() {

			// Get a random aspect ratio and setup width and height
			double aspectRatio = randomAspectRatio();
			unsigned long height = (unsigned long)std::ceil(std::sqrt((double)this->getMinimumArea() * aspectRatio));
			unsigned long width = (unsigned long)std::ceil(std::sqrt((double)this->getMinimumArea() / aspectRatio));

			this->setWidth(width);
			this->setHeight(height);

			// --------------------------------------Testing---------------------------------------
			/*
			unsigned long area = height * width;
			if (area < this->getMinimumArea()) {
				throw std::runtime_error("Soft Module Aspect Ratio is not correct.");
			}
			*/
			// std::cout << "soft module's area >= minimum area is " <<  (area >= this->getMinimumArea()) << std::endl;
			// --------------------------------------Testing---------------------------------------
		}

		// Getting a random double value
		static double randomAspectRatio() {

			// std::uniform_real_distribution<double> uniformRealDistribution(INITAL_MINIMUM_ASPECT_RATIO, INITAL_MAXIMUM_ASPECT_RATIO);
			// std::default_random_engine randomEngine(time(NULL));

			// return uniformRealDistribution(randomEngine);
			double diff = INITAL_MAXIMUM_ASPECT_RATIO - INITAL_MINIMUM_ASPECT_RATIO;
			return (((double) rand() / RAND_MAX) * diff) + INITAL_MINIMUM_ASPECT_RATIO;
		}
};

#endif /* SOFT_MODULE */
