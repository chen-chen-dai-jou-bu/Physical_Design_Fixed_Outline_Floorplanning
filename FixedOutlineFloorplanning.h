#pragma once
#ifndef FIXED_OUTLINE_FLOORPLANNING
#define FIXED_OUTLINE_FLOORPLANNING

#include <unordered_map>

#include "SoftModule.h"
#include "HardModule.h"
#include "Net.h"
#include "BStarTree.h"

class FixedOutlineFloorplanning {

	private:
		unsigned long maximunWidth;
		unsigned long maximunHeight;
		unsigned long numSoftModules;
		unsigned long numFixedModules;
		unsigned long numNets;

		std::unordered_map<std::string, Module*> modules;
		std::unordered_map<std::string, SoftModule*> softModules;
		std::unordered_map<std::string, HardModule*> hardModules;
		std::vector<Net*> nets;

		// 1. Hard modules are sorted in the non-decreasing order x-coordinates
		// 2. Hard modules with the same x-coordinate are ordered in the non - decreasing order of their y-coordinates.
		std::vector<HardModule*> sortedHardModules;

		// For placing modules
		Contour* contour;

		// For Simulated Annealing
		unsigned long areaNorm;
		unsigned long wirelengthNorm;

		std::shared_ptr<ProbabilityModel> probabilityModel = std::make_shared<ProbabilityModel>();
	public:
		FixedOutlineFloorplanning() {
			this->maximunWidth = 0;
			this->maximunHeight = 0;
			this->numSoftModules = 0;
			this->numFixedModules = 0;
			this->numNets = 0;

			this->areaNorm = 0;
			this->wirelengthNorm = 0;
		}

		FixedOutlineFloorplanning(FixedOutlineFloorplanning* floorplan) {

			this->maximunWidth = floorplan->getMaximunWidth();
			this->maximunHeight = floorplan->getMaximunHeight();
			this->numSoftModules = floorplan->getNumSoftModules();
			this->numFixedModules = floorplan->getNumFixedModules();
			this->numNets = floorplan->getNumNets();

			this->areaNorm = floorplan->getAreaNorm();
			this->wirelengthNorm = floorplan->getWirelengthNorm();

			std::unordered_map<std::string, Module*> modules = {};
			std::unordered_map<std::string, SoftModule*> softModules = {};
			std::unordered_map<std::string, HardModule*> hardModules = {};

			for (auto& i : floorplan->getSoftModules()) {
				auto* softModule = i.second;
				auto* copy = new SoftModule(softModule);
				softModules[i.first] = copy;
				modules[i.first] = copy;
			}
			this->softModules = softModules;

			for (auto& i : floorplan->getHardModules()) {
				auto* hardModule = i.second;
				auto* copy = new HardModule(hardModule);
				hardModules[i.first] = copy;
				modules[i.first] = copy;
			}
			this->hardModules = hardModules;
			this->modules = modules;

			std::vector<Net*> nets = {};
			for (auto& i : floorplan->getNets()) {
				nets.push_back(new Net(i));
			}
			this->nets = nets;

			std::vector<HardModule*> orderedHardModules = {};
			for (auto& i : this->hardModules) {
				orderedHardModules.push_back(i.second);
			}

			std::sort(orderedHardModules.begin(), orderedHardModules.end(), [](HardModule* a, HardModule* b) {
				if (a->getCoordinateX() < b->getCoordinateX()) {
					return true;
				}
				else if (a->getCoordinateX() == b->getCoordinateX()) {
					return a->getCoordinateY() < b->getCoordinateY();
				}
				return false;
				});
			this->setSortedHardModules(orderedHardModules);

			this->initializeContour();
		}

		void setMaximunWidth(unsigned long _maximunWidth) {
			this->maximunWidth = _maximunWidth;
		}

		unsigned long getMaximunWidth() {
			return this->maximunWidth;
		}

		void setMaximunHeight(unsigned long _maximunHeight) {
			this->maximunHeight = _maximunHeight;
		}

		unsigned long getMaximunHeight() {
			return this->maximunHeight;
		}

		void setNumSoftModules(unsigned long _numSoftModules) {
			this->numSoftModules = _numSoftModules;
		}

		unsigned long getNumSoftModules() {
			return this->numSoftModules;
		}

		void setNumFixedModules(unsigned long _numFixedModules) {
			this->numFixedModules = _numFixedModules;
		}

		unsigned long getNumFixedModules() {
			return this->numFixedModules;
		}

		void setNumNets(unsigned long _numNets) {
			this->numNets = _numNets;
		}

		unsigned long getNumNets() {
			return this->numNets;
		}

		void setModules(std::unordered_map<std::string, Module*> _modules) {
			this->modules = _modules;
		}

		auto getModules() {
			return this->modules;
		}

		void setSoftModules(std::unordered_map<std::string, SoftModule*> _softModules) {
			this->softModules = _softModules;

			for (auto& i : _softModules) {
				auto retPair = this->modules.insert(std::make_pair(i.first, i.second));
				if (retPair.second != true) {
					throw std::runtime_error("Inserting a module to the module map fail.");
				}
			}
		}

		std::unordered_map<std::string, SoftModule*> getSoftModules() {
			return this->softModules;
		}

		void setHardModules(std::unordered_map<std::string, HardModule*> _hardModules) {
			this->hardModules = _hardModules;

			for (auto& i : _hardModules) {
				auto retPair = this->modules.insert(std::make_pair(i.first, i.second));
				if (retPair.second != true) {
					throw std::runtime_error("Inserting a module to the module map fail.");
				}
			}
		}

		std::unordered_map<std::string, HardModule*> getHardModules() {
			return this->hardModules;
		}

		void setNets(std::vector<Net*> _nets) {
			this->nets = _nets;
		}

		std::vector<Net*> getNets() {
			return this->nets;
		}

		void setSortedHardModules(std::vector<HardModule*>& _sortedHardModules) {
			this->sortedHardModules = _sortedHardModules;
		}

		std::vector<HardModule*> getSortedHardModules() {
			return this->sortedHardModules;
		}

		Contour* getContour() {
			return this->contour;
		}

		void setAreaNorm(unsigned long _areaNorm) {
			this->areaNorm = _areaNorm;
		}

		unsigned long getAreaNorm() {
			return this->areaNorm;
		}

		void setWirelengthNorm(unsigned long _wirelengthNorm) {
			this->wirelengthNorm = _wirelengthNorm;
		}

		unsigned long getWirelengthNorm() {
			return this->wirelengthNorm;
		}

		void initializeContour() {
			this->contour = new Contour(this->maximunWidth);
		}

		unsigned long calculateWirelength(std::shared_ptr<BStarTree> tree) {

			std::unordered_map <std::string, Node*> nodes = tree->getNodes();
			std::unordered_map <std::string, Module*> modules = {};
			for (auto&i : nodes) {
				Module* module = i.second->getModule();
				modules[module->getName()] = module;
			}
			for (auto& i : this->getHardModules()) {
				Module* module = i.second;
				modules[module->getName()] = module;
			}

			unsigned long totalWireLength = 0;
			for (auto& i : this->getNets()) {

				unsigned long wireLength = 0;

				std::vector<std::string> names = i->getModules();
				Module* moduleA = modules[names[0]];
				Module* moduleB = modules[names[1]];

				/*
				if (moduleA == NULL || moduleB == NULL) {
					throw std::runtime_error("calculateHPWL Error: can't find the modules");
				}
				*/

				// Note that the x- or y- coordinate of the center of each module
				unsigned long x1 = moduleA->getCoordinateX() + moduleA->getWidth() / 2;
				unsigned long y1 = moduleA->getCoordinateY() + moduleA->getHeight() / 2;
				unsigned long x2 = moduleB->getCoordinateX() + moduleB->getWidth() / 2;
				unsigned long y2 = moduleB->getCoordinateY() + moduleB->getHeight() / 2;

				unsigned long maxCoordinateX = std::max(x1, x2);
				unsigned long maxCoordinateY = std::max(y1, y2);
				unsigned long minCoordinateX = std::min(x1, x2);
				unsigned long minCoordinateY = std::min(y1, y2);

				wireLength = (maxCoordinateX - minCoordinateX) * i->getWeight() + (maxCoordinateY - minCoordinateY) * i->getWeight();
				totalWireLength += wireLength;
			}

			return totalWireLength;
		}

		unsigned long getWidth() {
			return this->contour->getMaximunCoordinateX();

		}

		unsigned long getHeight() {
			return this->contour->getMaximunCoordinateY();

		}

		unsigned long calculateArea(std::shared_ptr<BStarTree> tree) {
			return this->contour->getMaximunCoordinateX() * this->contour->getMaximunCoordinateY();
		}


		// Returns true if two rectangles (l1, r1) and (l2, r2) overlap
		bool isOverlapping(Point l1, Point r1, Point l2, Point r2) {
			// if rectangle has area 0, no overlap
			if (l1.x == r1.x || l1.y == r1.y || r2.x == l2.x || l2.y == r2.y) {
				return false;
			}

			// If one rectangle is on left side of other
			if (l1.x > r2.x || l2.x > r1.x) {
				return false;
			}
			// If one rectangle is above other
			if (r1.y > l2.y || r2.y > l1.y) {
				return false;
			}
			/*
			// adjacent rectangles
			if (r1.x == l2.x) {
				return false;
			}
			if (l1.x == r2.x) {
				return false;
			}
			if (r1.y == l2.y) {
					return false;
			}
			if (r2.y == l1.y) {
				return false;
			}
			*/

			return true;
		}

		// Avoid overlapping fixed modules
		std::pair<unsigned long, unsigned long> getLegalCoordinate(Module* module) {

			unsigned long width = module->getWidth();
			unsigned long height = module->getHeight();

			unsigned long leftBoundary = module->getCoordinateX();
			unsigned long bottomBoundary = module->getCoordinateY();
			unsigned long rightBoundary = leftBoundary + width;
			unsigned long topBoundary = bottomBoundary + height;

			for (const auto& i : sortedHardModules) {

				Point l1 = { leftBoundary, topBoundary }, r1 = { rightBoundary, bottomBoundary };
				Point l2 = { i->getLeftBoundary(), i->getTopBoundary() }, r2 = { i->getRightBoundary(), i->getBottomBoundary() };
				if (isOverlapping(l1, r1, l2, r2)) {

					// Randomly right shift or up shift
					if (probabilityModel->getProbability() > 0.5) {
						// Shift Right
						leftBoundary = i->getCoordinateX() + i->getWidth();
						rightBoundary = leftBoundary + width;
					}
					else {
						// Shift Up
						bottomBoundary = i->getCoordinateY() + i->getHeight();
						topBoundary = bottomBoundary + height;
					}
				}
			} // end for-loop

			return std::make_pair(leftBoundary, bottomBoundary);
		}

		// Create a floorplan based on the B*-tree structure.
		void toFloorplan(std::shared_ptr <BStarTree> tree) {

			this->contour->initialize();

			Module* module = tree->getRoot()->getModule();
			module->setCoordinateX(0);
			module->setCoordinateY(0);

			// Avoid overlapping with fixed modules
			std::pair<unsigned long, unsigned long> point = this->getLegalCoordinate(module);
			module->setCoordinateX(point.first);
			module->setCoordinateY(point.second);

			this->contour->updateHorizontalContour(module->getCoordinateX(), module->getWidth(), module->getCoordinateY() + module->getHeight());

			if (tree->getRoot()->getLeft() != NULL) {
				placeModule(tree->getRoot()->getLeft(), true);
			}
			if (tree->getRoot()->getRight() != NULL) {
				placeModule(tree->getRoot()->getRight(), false);
			}
		}

		// Place a module according to parent node
		void placeModule(Node* node, bool isLeftChild) {

			Node* parent = node->getParent();
			Module* parentModule = parent->getModule();
			Module* module = node->getModule();

			if (isLeftChild) {
				module->setCoordinateX(parentModule->getCoordinateX() + parentModule->getWidth());
			}
			else {
				module->setCoordinateX(parentModule->getCoordinateX());
			}
			unsigned long coordinateY = contour->findMaximunCoordinateY(module->getCoordinateX(), module->getWidth());
			module->setCoordinateY(coordinateY);

			// Get a new coordinate if module overlaps with fixed modules
			std::pair<unsigned long, unsigned long> point = this->getLegalCoordinate(module);
			module->setCoordinateX(point.first);
			module->setCoordinateY(point.second);

			this->contour->updateHorizontalContour(module->getCoordinateX(), module->getWidth(), module->getCoordinateY() + module->getHeight());

			if (node->getLeft() != NULL) {
				/*----------------------------------------testing----------------------------------------
				if (node->getLeft()->getParent()->getModule()->getName() != node->getModule()->getName()) {
					throw std::runtime_error("Parent pointer is not correct.");
				}
				  ----------------------------------------testing----------------------------------------*/
				placeModule(node->getLeft(), true);
			}
			if (node->getRight() != NULL) {
				/*----------------------------------------testing----------------------------------------
				if (node->getRight()->getParent()->getModule()->getName() != node->getModule()->getName()) {
					throw std::runtime_error("Parent pointer is not correct.");
				}
				  ----------------------------------------testing----------------------------------------*/
				placeModule(node->getRight(), false);
			}
		}

		bool isFeasibleFloorplan() {

			if (this->contour->getMaximunCoordinateX() > maximunWidth) {
				return false;
			}
			if (this->contour->getMaximunCoordinateY() > maximunHeight) {
				return false;
			}
			return true;
		}
};

#endif /* FIXED_OUTLINE_FLOORPLANNING */