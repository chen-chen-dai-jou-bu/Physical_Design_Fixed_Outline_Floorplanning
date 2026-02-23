#pragma once
#ifndef FLOOR_PLAN_ALOGRITHM
#define FLOOR_PLAN_ALOGRITHM

#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include<chrono>

#include "FixedOutlineFloorplanning.h"
#include "SoftModule.h"
#include "FloorplanFile.h"
#include "Net.h"
#include "BStarTree.h"
#include "Contour.h"
#include "TreePrinter.h"
#include "Cost.h"

class FloorPlanAlogrithm {

private:
    // For randomly choose a move
    std::shared_ptr<ProbabilityModel> probabilityModel = std::make_shared<ProbabilityModel>();
    const int MOVE_RESIZE = 0;
    const int MOVE_ROTATE = 1;
    const int MOVE_DELETE_AND_INSERT = 2;
    const int MOVE_SWAP = 3;
    const int MOVE_SIZE = 4;

public:
    // Get all information of soft modules from file text
    // Include module name and minimum area
    static std::unordered_map<std::string, SoftModule*> initializeSoftModules(std::fstream& file, int numberOfLines) {

        std::unordered_map<std::string, SoftModule*> softModules = {};

        // Line Example : ¡uSoftModule M0 102400¡v
        const int INDEX_NAME = 1;
        const int INDEX_MINIMUM_AREA = 2;

        std::string line;
        int index = 0;
        while (index < numberOfLines) {

            // Read data from the file object and put it into a string.
            getline(file, line);

            // declaring vector to store the string after split
            std::vector<std::string> vector = FloorplanFile::splitString(line);;

            std::string name = std::string(vector.at(INDEX_NAME));
            unsigned long minimumArea = std::stoul(vector.at(INDEX_MINIMUM_AREA));

            // New a soft module
            SoftModule* softModule = new SoftModule(name, minimumArea, INITAL_MINIMUM_ASPECT_RATIO, INITAL_MAXIMUM_ASPECT_RATIO);

            auto retPair = softModules.insert(std::make_pair(name, softModule));
            if (retPair.second != true) {
                throw std::runtime_error("Inserting a softModule to the soft module map fail.");
            }

            index++;
        }
        return softModules;
    }

    // Get all information of fixed modules from file text
    // Include module name, x-coordinate, y-coordinate, width and height
    static std::unordered_map<std::string, HardModule*> initializeFixedModules(std::fstream& file, int numberOfLines) {

        std::unordered_map<std::string, HardModule*> hardModules = {};

        // Line Example : ¡uFixedModule CPU0 0 2020 1900 280¡v
        const int INDEX_NAME = 1;
        const int INDEX_COORDINATE_X = 2;
        const int INDEX_COORDINATE_Y = 3;
        const int INDEX_WIDTH = 4;
        const int INDEX_HEIGHT = 5;

        std::string line;
        int index = 0;
        while (index < numberOfLines) {

            // Read data from the file object and put it into a string.
            getline(file, line);

            // declaring vector to store the string after split
            std::vector<std::string> vector = FloorplanFile::splitString(line);;

            std::string name = std::string(vector.at(INDEX_NAME));
            unsigned long coordinateX = std::stoul(vector.at(INDEX_COORDINATE_X));
            unsigned long coordinateY = std::stoul(vector.at(INDEX_COORDINATE_Y));
            unsigned long width = std::stoul(vector.at(INDEX_WIDTH));
            unsigned long height = std::stoul(vector.at(INDEX_HEIGHT));

            // New a soft module
            HardModule* hardModule = new HardModule(name, width, height, coordinateX, coordinateY);
            auto retPair = hardModules.insert(std::make_pair(name, hardModule));
            if (retPair.second != true) {
                throw std::runtime_error("Inserting a hardModule to the hard module map fail.");
            }

            index++;
        }
        return hardModules;
    }

    // Get all information of net from file text
    // Include module names and weight
    static std::vector<Net*> initializeNets(std::fstream& file, int numberOfLines) {

        std::vector<Net*> nets = {};

        // Line Example : ¡uNet M10 GPU1 745¡v
        const int INDEX_MODULE_A = 1;
        const int INDEX_MODULE_B = 2;
        const int INDEX_WEIGHT = 3;

        std::string line;
        int index = 0;
        while (index < numberOfLines) {

            // Read data from the file object and put it into a string.
            getline(file, line);

            // declaring vector to store the string after split
            std::vector<std::string> vector = FloorplanFile::splitString(line);
            std::vector<std::string> modules = {};

            std::string moduleA = std::string(vector.at(INDEX_MODULE_A));
            std::string moduleB = std::string(vector.at(INDEX_MODULE_B));
            unsigned long weight = std::stoul(vector.at(INDEX_WEIGHT));

            // New a net
            modules.push_back(moduleA);
            modules.push_back(moduleB);
            Net* net = new Net(weight, modules);

            nets.push_back(net);
            index++;
        }
        return nets;
    }


    // 1. Hard modules are sorted in the non-decreasing order x-coordinates
    // 2. Hard modules with the same x-coordinate are ordered in the non - decreasing order of their y-coordinates.
    std::vector<HardModule*> getSortedHardModules(std::unordered_map<std::string, HardModule*> hardModules) {

        std::vector<HardModule*> orderedHardModules = {};
        for (auto& i : hardModules) {
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
        return orderedHardModules;
    }

    // Get a initial B*-Tree.
    std::shared_ptr<BStarTree> initializeCompeleteBinaryTree(FixedOutlineFloorplanning* floorplan) {

        std::unordered_map<std::string, SoftModule*> softModules = floorplan->getSoftModules();

        std::vector<SoftModule*> copySoftModules = {};
        for (auto& i : softModules) {
            copySoftModules.push_back(new SoftModule(i.second));
        }

        std::shuffle(copySoftModules.begin(), copySoftModules.end(), std::random_device());

        // Root Node
        SoftModule* module = *copySoftModules.begin();
        module->setRandomAsepectRatio();

        Node* root = new Node(module);
        std::shared_ptr<BStarTree> tree = std::make_shared<BStarTree>(root);

        for (auto it = std::next(copySoftModules.begin()); it != copySoftModules.end(); ++it) {
            SoftModule* module = *it;

            // Randomly determine width and height then insert it to B*-Tree
            module->setRandomAsepectRatio();
            tree->initializedInsert(module);
        }

        return tree;
    }

    // Get a initial floorplan at fist
    // We will do several perturbations to get a better initial solution.
    std::shared_ptr<BStarTree> oldInitializeFloorplan(FixedOutlineFloorplanning* floorplan) {

        auto sortedHardModules = FloorPlanAlogrithm::getSortedHardModules(floorplan->getHardModules());
        floorplan->setSortedHardModules(sortedHardModules);

        // Initialize a B*-Tree as a complete binary tree. let soft modules are compacted.
        std::shared_ptr<BStarTree> basedTree = FloorPlanAlogrithm::initializeCompeleteBinaryTree(floorplan);
        // --------------------------------------Testing---------------------------------------
        // basedTree->checkParent(basedTree->getRoot());
        // TreePrinter::printTree(basedTree->getRoot(), NULL, false);
        // --------------------------------------Testing---------------------------------------

        basedTree->setCost(FloorPlanAlogrithm::calculateCost(floorplan, basedTree));

        std::shared_ptr<BStarTree> optimalSolution = std::make_shared<BStarTree>(basedTree);
        std::shared_ptr<BStarTree> modifiedSolution = std::make_shared<BStarTree>(basedTree);

        bool isCopyFromOptimal = false;
        const unsigned TIMES_LIMIT = std::pow(floorplan->getNumSoftModules(), 4);
        try {
            for (int times = 0; times < TIMES_LIMIT; times++) {

                if (isCopyFromOptimal) {
                    modifiedSolution = std::make_shared<BStarTree>(optimalSolution);
                }

                FloorPlanAlogrithm::perturbSolution(modifiedSolution);

                // Compare solutions
                std::shared_ptr<Cost> modifiedCost = FloorPlanAlogrithm::calculateCost(floorplan, modifiedSolution);
                modifiedSolution->setCost(modifiedCost);
                double deltaCost = modifiedCost->getCost() - optimalSolution->getCost()->getCost();

                if (deltaCost < 0) {
                    optimalSolution = std::make_shared<BStarTree>(modifiedSolution);

                    isCopyFromOptimal = false;
                }
                else {
                    isCopyFromOptimal = true;
                }

                if (floorplan->isFeasibleFloorplan()) {
                    std::chrono::duration<double, std::milli> runningTime = std::chrono::high_resolution_clock::now() - RUNNING_START;
                    unsigned long optimalWirelength = floorplan->calculateWirelength(optimalSolution);
                    std::cout << "Initialize Floorplan Execution time :" << runningTime.count() << "ms, Wirelength : " << optimalWirelength << std::endl;
                    break;
                }

                std::chrono::duration<double, std::milli> runningTime = std::chrono::high_resolution_clock::now() - RUNNING_START;
                if (runningTime > TIME_LIMIT) {
                    std::cout << "Initialize Floorplan Execution is timeout." << std::endl;
                    break;
                }
            }
        }
        catch (const std::exception& error) {
            std::cout << error.what() << std::endl;
        }

        std::chrono::duration<double, std::milli> runningTime = std::chrono::high_resolution_clock::now() - RUNNING_START;
        std::cout << "Initialize Floorplan Execution time :" << runningTime.count() << "ms" << std::endl;

        return optimalSolution;
    }

    // Get a initial floorplan at fist
    std::shared_ptr<BStarTree> initializeFloorplan(FixedOutlineFloorplanning* floorplan) {

        auto sortedHardModules = FloorPlanAlogrithm::getSortedHardModules(floorplan->getHardModules());
        floorplan->setSortedHardModules(sortedHardModules);

        // Initialize a B*-Tree as a complete binary tree. let soft modules are compacted.
        std::shared_ptr<BStarTree> basedTree = FloorPlanAlogrithm::initializeCompeleteBinaryTree(floorplan);
        // --------------------------------------Testing---------------------------------------
        // basedTree->checkParent(basedTree->getRoot());
        // TreePrinter::printTree(basedTree->getRoot(), NULL, false);
        // --------------------------------------Testing---------------------------------------

        floorplan->initializeContour();
        floorplan->toFloorplan(basedTree);

        return basedTree;
    }

    // This probability will choose which move will be executed.
    // Including ¡uResize¡v¡B¡urotate¡v¡B¡uDelete and Insert¡v¡B¡uSwap¡v
    void perturbSolution(std::shared_ptr<BStarTree> tree) {

        int move = rand() % this->MOVE_SIZE;
        if (move == this->MOVE_RESIZE) {
            tree->resize(tree->returnRandomNode());
        }
        else if (move == this->MOVE_ROTATE) {
            tree->rotate(tree->returnRandomNode());
        }
        else if (move == this->MOVE_DELETE_AND_INSERT) {

            Node* deletedNode = tree->deleteNode(tree->returnRandomNode());
            if (deletedNode != NULL) {
                tree->insertNode(deletedNode);
            }
        }
        else if (move == this->MOVE_SWAP) {
            std::pair<Node*, Node*> differentNodes = tree->returnRandomTwoNodes();
            Node* nodeA = differentNodes.first;
            Node* nodeB = differentNodes.second;

            if (nodeA->getName() != nodeB->getName()) {
                tree->swap(nodeA, nodeB);
            }
            else {
                std::cout << "Same nodes can't be swaped." << std::endl;
            }
        }
    }

    // Cost includes ¡uAspectRatio¡v¡B¡uArea¡v¡B¡uWireLength¡v
    std::shared_ptr<Cost> calculateCost(FixedOutlineFloorplanning* floorplan, std::shared_ptr<BStarTree> tree) {

        floorplan->toFloorplan(tree);

        unsigned long area = floorplan->calculateArea(tree);
        unsigned long wireLength = floorplan->calculateWirelength(tree);

        std::shared_ptr<Cost> cost = std::make_shared<Cost>(area, floorplan->getWidth(), floorplan->getHeight(), wireLength, 0);

        if (floorplan->isFeasibleFloorplan()) {
            cost->setFeasible(true);
        }

        cost->setCost(area);
        return cost;
    }

    // Setup Area Norm and Wirelength Norm
    double getInitialTemperature(FixedOutlineFloorplanning* floorplan, std::shared_ptr<BStarTree> initalSolution) {

        const double PROBABILITY = 0.95;
        const int UP_HILL_LIMIT = floorplan->getNumSoftModules();

        int upHillTimes = 0;
        double totalUpHillCost = 0;

        floorplan->toFloorplan(initalSolution);
        std::shared_ptr<Cost> previousCost = FloorPlanAlogrithm::calculateCost(floorplan, initalSolution);

        std::shared_ptr<BStarTree> modifiedSolution = std::make_shared<BStarTree>(initalSolution);
        while (upHillTimes < UP_HILL_LIMIT) {

            // Perturb solution
            FloorPlanAlogrithm::perturbSolution(modifiedSolution);

            // Calculate cost
            floorplan->toFloorplan(modifiedSolution);
            std::shared_ptr<Cost> cost = FloorPlanAlogrithm::calculateCost(floorplan, initalSolution);
            if (cost->getCost() > previousCost->getCost()) {
                totalUpHillCost += cost->getCost();
                upHillTimes++;
            }
            else {
                previousCost = cost;
            }
        } // end while

        double averageCost = totalUpHillCost / UP_HILL_LIMIT;
        return -(averageCost) / std::log(PROBABILITY);
    }

    std::shared_ptr<BStarTree> SimulatedAnnealing(FixedOutlineFloorplanning* floorplan, std::shared_ptr<BStarTree> initalSolution) {

        const double R = 0.95;
        const int K = 20, N = K * std::pow(floorplan->getNumSoftModules(), 2);

        const double initialTemperature = FloorPlanAlogrithm::getInitialTemperature(floorplan, initalSolution);
        double temperature = initialTemperature;

        std::chrono::duration<double, std::milli> runningTime;

        std::shared_ptr<BStarTree> optimalSolution = std::make_shared<BStarTree>(initalSolution);
        std::shared_ptr<BStarTree> previousSolution = std::make_shared<BStarTree>(initalSolution);
        std::shared_ptr<BStarTree> modifiedSolution = std::make_shared<BStarTree>(initalSolution);

        bool isCopyFromPrevious = false, haveFeasibleSolution = false;
        std::vector<std::shared_ptr<BStarTree>> feasibleSolutions = {};
        if (initalSolution->getCost()->getFeasible()) {
            feasibleSolutions.push_back(initalSolution);
        }

        double rejectionRate = 0;

        // While not yet ¡ufrozen¡v
        do {
            // clock_t startTimePerTemperature = clock();
            //----------------------------------------testing----------------------------------------
            if (haveFeasibleSolution && ENABLE_ONLY_ONE_FEASIBLE) {
                break;
            }
            //----------------------------------------testing----------------------------------------
            int rejectCount = 0, MT = 0, upHillCount = 0;
            do {
                // Step.1 Perturb solution
                if (isCopyFromPrevious) {
                    modifiedSolution = std::make_shared<BStarTree>(previousSolution);
                }
                FloorPlanAlogrithm::perturbSolution(modifiedSolution);

                // Step.2 Calculate the cost between two solutions

                // Compare solutions
                std::shared_ptr<Cost> modifiedCost = FloorPlanAlogrithm::calculateCost(floorplan, modifiedSolution);
                double deltaCost = modifiedCost->getCost() - previousSolution->getCost()->getCost();

                MT++;

                // If solution is feasible then keep it.
                if (modifiedCost->getFeasible()) {
                    feasibleSolutions.push_back(std::make_shared<BStarTree>(modifiedSolution));
                }

                // Step.3 If getting better or equal, keep it
                if (deltaCost <= 0) {
                    // Accept this perturbed solution and copy it and delete previous solution
                    isCopyFromPrevious = false;
                    previousSolution = std::make_shared<BStarTree>(modifiedSolution);

                    if (modifiedCost->getCost() < optimalSolution->getCost()->getCost()) {
                        optimalSolution = std::make_shared<BStarTree>(modifiedSolution);
                    }
                }
                // Step.4 If getting worse, using a probability to choose if we should keep it.
                else {
                    double probability = std::exp(-1.0 * deltaCost / temperature);
                    double accecptProbability = this->probabilityModel->getProbability();
                    // std::cout << "||" << accecptProbability << "|| Probability = " << probability << " ||";

                    if (accecptProbability < probability) {
                        // Accept this perturbed solution, copy it and delete previous solution
                        isCopyFromPrevious = false;
                        previousSolution = std::make_shared<BStarTree>(modifiedSolution);

                        upHillCount++;
                    }
                    else {
                        // Reject this perturbed solution and recovery to previous solution
                        isCopyFromPrevious = true;

                        rejectCount++;
                    }
                }
            } while (upHillCount <= N || MT <= 2 * N); // end of while


            // Step5. reduce temperature (To change the probability coefficient)
            temperature *= R;
            // std::cout << std::endl << "Temperature = " << temperature << std::endl;

            // If out of time or rejectionRate is too high, we will terminate.
            runningTime = std::chrono::high_resolution_clock::now() - RUNNING_START;
            rejectionRate = (double)rejectCount / MT;

            // ¡uTIME_LIMIT¡v is in SettingParameter.h
            if (runningTime > TIME_LIMIT) {
                std::cout << "Thread : " << std::this_thread::get_id() << ", Simulated Annealing Execution timeout :" << runningTime.count() << "ms" << std::endl;
                break;
            }

            if (!feasibleSolutions.empty()) {
                haveFeasibleSolution = true;
            }

            // double runningTimePerTemperature = (double) (clock() - startTimePerTemperature) / CLOCKS_PER_SEC;
            // std::cout << "Execution time Per Temperature :" << runningTimePerTemperature << std::endl;
            if (rejectionRate > 0.95) {
                std::cout << "Thread : " << std::this_thread::get_id() << ", Simulated Annealing rejectionRate : " << rejectionRate << std::endl;
            }
        } while (rejectionRate <= 0.95); // end of while

        if (haveFeasibleSolution) {
            // Return the best feasible solution

            // Pair stores <wirelength, BStarTree>
            std::shared_ptr<BStarTree> optimalFeasibleSolution = feasibleSolutions.front();
            for (auto& i : feasibleSolutions) {

                //----------------------------------------testing----------------------------------------
                // FloorplanFile::writeOutputFile(floorplan, i, GLOBAL_OUTPUT_FILE_PATH);
                //----------------------------------------testing----------------------------------------
                unsigned long wirelength = i->getCost()->getWirelength();
                if (wirelength < optimalFeasibleSolution->getCost()->getWirelength()) {
                    optimalFeasibleSolution = i;
                }
            }

            std::cout << "Have a feasible solution solution. Wirelength = " << optimalFeasibleSolution->getCost()->getWirelength() << std::endl;
            return optimalFeasibleSolution;
        }
        else {
            // Return the best solution
            std::cout << "Only have a best solution." << std::endl;
            return optimalSolution;
        }
    }
};

#endif /* FLOOR_PLAN_ALOGRITHM */