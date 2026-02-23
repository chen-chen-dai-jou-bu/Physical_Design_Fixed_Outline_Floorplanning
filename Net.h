#pragma once
#ifndef NET
#define NET

#include <string>
#include <vector>

#include "Module.h"

class Net {
	private:
		unsigned long weight;
		std::vector<std::string> modules;

	public:
		Net(unsigned long _weight, std::vector<std::string> _modules) {
			this->weight = _weight;
			this->modules = _modules;
		}

		Net(Net* net) {

			this->weight = net->getWeight();
			std::vector<std::string> modules = {};
			for (std::string module : net->getModules()) {
				modules.push_back(module);
			}
			this->modules = modules;
		}

		void setWeight(unsigned long _weight) {
			this->weight = _weight;
		}

		unsigned long getWeight() {
			return this->weight;
		}

		void setModules(std::vector<std::string> _modules) {
			this->modules = _modules;
		}

		std::vector<std::string> getModules() {
			return this->modules;
		}
};

#endif /* NET */