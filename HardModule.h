#pragma once
#ifndef HARD_MODULE
#define HARD_MODULE

#include <string>

#include "Module.h"

class HardModule : public Module {
	private:
		unsigned long leftBoundary;
		unsigned long rightBoundary;
		unsigned long bottomBoundary;
		unsigned long topBoundary;

	public:
		HardModule(std::string _name, unsigned long _width, unsigned long _height, unsigned long _coordinateX, unsigned long _coordinateY) : Module(_name){
			this->setWidth(_width);
			this->setHeight(_height);
			this->setCoordinateX(_coordinateX);
			this->setCoordinateY(_coordinateY);

			this->leftBoundary = _coordinateX;
			this->rightBoundary = _coordinateX + _width;
			this->bottomBoundary = _coordinateY;
			this->topBoundary = _coordinateY + _height;
		}

		HardModule(HardModule* _module) : Module(_module) {

			this->setWidth(_module->getWidth());
			this->setHeight(_module->getHeight());
			this->setCoordinateX(_module->getCoordinateX());
			this->setCoordinateY(_module->getCoordinateY());

			this->leftBoundary = _module->getCoordinateX();
			this->rightBoundary = _module->getCoordinateX() + _module->getWidth();
			this->bottomBoundary = _module->getCoordinateY();
			this->topBoundary = _module->getCoordinateY() + _module->getHeight();
		}

		unsigned long getTopBoundary() {
			return this->topBoundary;
		}

		unsigned long getRightBoundary() {
			return this->rightBoundary;
		}

		unsigned long getLeftBoundary() {
			return this->leftBoundary;
		}

		unsigned long getBottomBoundary() {
			return this->bottomBoundary;
		}
};

#endif /* HARD_MODULE */