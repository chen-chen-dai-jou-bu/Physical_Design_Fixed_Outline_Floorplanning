#pragma once
#ifndef CONTOUR
#define CONTOUR

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <omp.h>

#include "Module.h"
#include "HardModule.h"

class Contour {

	private:
		std::vector<unsigned long> horizontalContour;
		unsigned long boundary;
		unsigned long maximunCoordinateX = 0;
		unsigned long maximunCoordinateY = 0;

	public :
		Contour(unsigned long _width) {
			this->horizontalContour = std::vector<unsigned long>(_width * 5, 0);
			this->boundary = _width * 5;
		}

		Contour(Contour* contour) {
			this->horizontalContour = std::vector<unsigned long>(contour->getHorizontalContour().size(), 0);
			contour->getHorizontalContour().swap(this->horizontalContour);

			this->boundary = contour->getBoundary();
			this->maximunCoordinateX = contour->getMaximunCoordinateX();
			this->maximunCoordinateY = contour->getMaximunCoordinateY();
		}

		~Contour() {
			this->horizontalContour.clear();
			this->horizontalContour.shrink_to_fit();
		}

		void initialize() {
			std::fill(horizontalContour.begin(), horizontalContour.end(), 0);
			this->maximunCoordinateX = 0;
			this->maximunCoordinateY = 0;
		}

		std::vector<unsigned long> getHorizontalContour() {
			return this->horizontalContour;
		}

		unsigned long getBoundary() {
			return this->boundary;
		}

		unsigned long getMaximunCoordinateX() {
			return this->maximunCoordinateX;
		}

		unsigned long getMaximunCoordinateY() {
			return this->maximunCoordinateY;
		}

		unsigned long findMaximunCoordinateY(unsigned long x, unsigned long width) {

			unsigned long rightBoundary = x + width;

			// Out of boundary
			if (x > this->boundary || rightBoundary > this->boundary) {
				throw std::runtime_error("Out of boundary");
				// return ULONG_MAX;
			}

			// Method ONE
			/*
			unsigned long maximunCoordinateY = 0;
			for (unsigned long i = x; i < rightBoundary; i++) {
				maximunCoordinateY = std::max(horizontalContour[i], maximunCoordinateY);
			}
			return maximunCoordinateY;
			*/

			// Method TWO
			return *(std::max_element(horizontalContour.begin() + x, horizontalContour.begin() + rightBoundary));
		}

		void updateHorizontalContour(unsigned long x, unsigned long width, unsigned long coordinateY) {

			unsigned long rightBoundary = x + width;

			// Out of boundary
			if (x >= this->boundary || rightBoundary >= this->boundary) {
				throw std::runtime_error("Out of boundary");
			}

			this->maximunCoordinateX = std::max(rightBoundary, maximunCoordinateX);
			this->maximunCoordinateY = std::max(coordinateY, maximunCoordinateY);

			// Method ONE
			#pragma omp parallel for
			for (unsigned long i = x; i < rightBoundary; i++) {
				this->horizontalContour[i] = coordinateY;
			}

			// Method TWO
			// std::fill_n(this->horizontalContour.begin() + x, rightBoundary, coordinateY);
		}

		void printContour() {

			unsigned long previousCoordinateY = -1;

			std::cout << "<" << "(0,0)";
			for (unsigned long x = 0; x < this->horizontalContour.size(); x++) {
				if (this->horizontalContour[x] != previousCoordinateY) {
					if (x > 0) {
						std::cout << "(" << x << "," << this->horizontalContour[x - 1] << ")";
					}
					std::cout << "(" << x << "," << this->horizontalContour[x] << ")";
					previousCoordinateY = this->horizontalContour[x];
				}
			}
			std::cout << "(\u221E,0)" << ">" << std::endl;
		}

		/* Test
		void main() {
			HardModule* module1 = new HardModule("1", 9, 6);
			HardModule* module2 = new HardModule("2", 6, 8);
			HardModule* module3 = new HardModule("3", 3, 6);
			HardModule* module4 = new HardModule("4", 3, 7);
			HardModule* module5 = new HardModule("5", 6, 5);
			HardModule* module6 = new HardModule("6", 12, 2);
			HardModule* module7 = new HardModule("7", 3, 7);
			HardModule* module8 = new HardModule("8", 9, 7);

			Contour* contour = new Contour(floorplan->getMaximunWidth());

			unsigned long coordinateY = contour->addRectangleOnRoot(module1->getCoordinateX(), module1->getWidth(),module1->getHeight());
			module1->setCoordinateY(coordinateY);
			contour->printContour();

			module2->setCoordinateX(module1->getCoordinateX() + module1->getWidth());
			coordinateY = contour->addRectangleOnRightSide(module2->getCoordinateX(), module2->getWidth(), module2->getHeight());
			module2->setCoordinateY(coordinateY);
			contour->printContour();

			module3->setCoordinateX(module1->getCoordinateX());
			coordinateY = contour->addRectangleOnUpperSide(module3->getCoordinateX(), module3->getWidth(), module3->getHeight());
			module3->setCoordinateY(coordinateY);
			contour->printContour();


			module4->setCoordinateX(module3->getCoordinateX() + module3->getWidth());
			coordinateY = contour->addRectangleOnRightSide(module4->getCoordinateX(), module4->getWidth(), module4->getHeight());
			module4->setCoordinateY(coordinateY);
			contour->printContour();

			module5->setCoordinateX(module4->getCoordinateX() + module4->getWidth());
			coordinateY = contour->addRectangleOnRightSide(module5->getCoordinateX(), module5->getWidth(), module5->getHeight());
			module5->setCoordinateY(coordinateY);
			contour->printContour();

			module6->setCoordinateX(module3->getCoordinateX());
			coordinateY = contour->addRectangleOnUpperSide(module6->getCoordinateX(), module6->getWidth(), module6->getHeight());
			module6->setCoordinateY(coordinateY);
			contour->printContour();

			module7->setCoordinateX(module2->getCoordinateX());
			coordinateY = contour->addRectangleOnUpperSide(module7->getCoordinateX(), module7->getWidth(), module7->getHeight());
			module7->setCoordinateY(coordinateY);
			contour->printContour();
		}
		*/
};


#endif /* CONTOUR */