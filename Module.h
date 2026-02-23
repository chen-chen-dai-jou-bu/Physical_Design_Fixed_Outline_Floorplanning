#pragma once
#ifndef MODULE
#define MODULE

#include <string>

class Module {

	private:
		std::string name;
		unsigned long coordinateX = 0;
		unsigned long coordinateY = 0;
		unsigned long width = 0;
		unsigned long height = 0;

	public:
		Module(std::string _name) {
			this->name = _name;
		}

		Module(Module* _module) {
			this->name = _module->getName();
			this->coordinateX = _module->getCoordinateX();
			this->coordinateY = _module->getCoordinateY();
			this->width = _module->getWidth();
			this->height = _module->getHeight();
		}

		void setName(std::string _name) {
			this->name = _name;
		}

		std::string getName() {
			return this->name;
		}

		void setCoordinateX(unsigned long _coordinateX) {
			this->coordinateX = _coordinateX;
		}

		unsigned long getCoordinateX() {
			return this->coordinateX;
		}

		void setCoordinateY(unsigned long _coordinateY) {
			this->coordinateY = _coordinateY;
		}

		unsigned long getCoordinateY() {
			return this->coordinateY;
		}

		void setWidth(unsigned long _width) {
			this->width = _width;
		}

		unsigned long getWidth() {
			return this->width;
		}

		void setHeight(unsigned long _height) {
			this->height = _height;
		}

		unsigned long getHeight() {
			return this->height;
		}
};

#endif /* MODULE */