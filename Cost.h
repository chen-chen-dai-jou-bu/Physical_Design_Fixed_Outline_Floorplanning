#pragma once
#ifndef COST_H
#define COST_H

class Cost {

    private:
        unsigned long width;
        unsigned long height;
        unsigned long area;
        unsigned long wirelength;
        double aspectRatio;
        double cost = 0;
        bool feasible = false;

    public:
        Cost(unsigned long _area, unsigned long _width, unsigned long _height, unsigned long _wirelength, double _aspectRatio) {
            this->width = _width;
            this->height = _height;
            this->area = _area;
            this->wirelength = _wirelength;
            this->aspectRatio = _aspectRatio;
        }

        Cost(std::shared_ptr<Cost> cost) {
            this->width = cost->getWidth();
            this->height = cost->getHeight();
            this->area = cost->getArea();
            this->wirelength = cost->getWirelength();
            this->aspectRatio = cost->getAspectRatio();
            this->cost = cost->getCost();
            this->feasible = cost->getFeasible();
        }

        void setWidth(unsigned long _width) {
            this->width = _width;
        }

        void setHeight(unsigned long _height) {
            this->height = _height;
        }

        void setArea(unsigned long _area) {
            this->area = _area;
        }

        void setWirelength(unsigned long _wirelength) {
            this->wirelength = _wirelength;
        }

        void setAspectRatio(double _aspectRatio) {
            this->aspectRatio = _aspectRatio;
        }

        void setCost(double _cost) {
            this->cost = _cost;
        }

        void setFeasible(bool _feasible) {
            this->feasible = _feasible;
        }

        unsigned long getWidth() {
            return this->width;
        }

        unsigned long getHeight() {
            return this->height;
        }

        unsigned long getArea() {
            return this->area;
        }

        unsigned long getWirelength() {
            return this->wirelength;
        }

        double getAspectRatio() {
            return this->aspectRatio;
        }

        double getCost() {
            return this->cost;
        }

        bool getFeasible() {
            return this->feasible;
        }

};

#endif /* COST_H */