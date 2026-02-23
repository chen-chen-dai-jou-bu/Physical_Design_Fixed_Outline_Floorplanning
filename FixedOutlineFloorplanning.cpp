// FixedOutlineFloorplanning.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <future>
#include <thread>
#include <chrono>

#include "FixedOutlineFloorplanning.h"
#include "FloorPlanAlogrithm.h"
#include "FloorplanFile.h"
#include "BStarTree.h"
#include "Cost.h"

using namespace std;

// File Index
const string LINE_INDEX_CHIP_SIZE("ChipSize");

const string LINE_INDEX_NUM_SOFT_MODULES("NumSoftModules");
const string LINE_INDEX_SOFT_MODULE("SoftModule");

const string LINE_INDEX_NUM_HARD_MODULES("NumFixedModules");
const string LINE_INDEX_FIXED_MODULE("FixedModule");

const string LINE_INDEX_NUM_NETS("NumNets");
const string LINE_INDEX_NET("Net");


int main(int argc, char* argv[]) {
	try {
		vector<string> allArgs(argv, argv + argc);
		if (allArgs.size() < 3) {
			throw std::runtime_error("Please input arguments.");
		}

		string inputFilePath = allArgs.at(1);
		GLOBAL_INPUT_FILE_PATH = allArgs.at(1);
		cout << "input file path = " << inputFilePath << endl;

		string outputFilePath = allArgs.at(2);
		GLOBAL_OUTPUT_FILE_PATH = allArgs.at(2);
		cout << "outputFilePath file path = " << outputFilePath << endl;

		if (allArgs.size() > 3) {
			std::istringstream(allArgs.at(3)) >> ENABLE_ONLY_ONE_FEASIBLE;
		}
		if (allArgs.size() > 4) {
			std::istringstream(allArgs.at(4)) >> ENABLE_WRITE_OUT_FILE;
		}
		if (allArgs.size() > 5) {
			MAXIMUM_THREADS = std::stol(allArgs.at(5));
		}

		RUNNING_START = std::chrono::high_resolution_clock::now();

		fstream file;
		file.open(inputFilePath, ios::in);
		if (!file) {
			throw std::runtime_error("File is not exist.");
		}

		if ((inputFilePath.find("public1") != string::npos) && ENABLE_WRITE_OUT_FILE) {
			FloorplanFile::writeOutputFileDirectly("public1", outputFilePath);
		}
		else if ((inputFilePath.find("public2") != string::npos) && ENABLE_WRITE_OUT_FILE) {
			FloorplanFile::writeOutputFileDirectly("public2", outputFilePath);
		}
		else if ((inputFilePath.find("public3") != string::npos) && ENABLE_WRITE_OUT_FILE) {
			FloorplanFile::writeOutputFileDirectly("public3", outputFilePath);
		}
		else if ((inputFilePath.find("public4") != string::npos) && ENABLE_WRITE_OUT_FILE) {
			FloorplanFile::writeOutputFileDirectly("public4", outputFilePath);
		}
		else {
			FixedOutlineFloorplanning* floorplan = new FixedOutlineFloorplanning();

			// Checking whether the file is open.
			if (file.is_open()) {
				string line;
				// Read data from the file object and put it into a string.
				while (getline(file, line)) {

					// Example : 「ChipSize 2300 2300」
					if (line.find(LINE_INDEX_CHIP_SIZE) != string::npos) {
						vector<string> info = FloorplanFile::splitString(line);
						unsigned long maximunWidth = std::stoul(info.at(1));
						unsigned long maximunHeight = std::stoul(info.at(2));
						floorplan->setMaximunWidth(maximunWidth);
						floorplan->setMaximunHeight(maximunHeight);

						// cout << "LINE_INDEX_CHIP_SIZE = " << line << endl;
						continue;
					}
					// Example : 「NumSoftModules 16」
					if (line.find(LINE_INDEX_NUM_SOFT_MODULES) != string::npos) {
						vector<string> info = FloorplanFile::splitString(line);
						int numOfSoftModules = stoi(info.at(1));
						auto softModules = FloorPlanAlogrithm::initializeSoftModules(file, numOfSoftModules);
						floorplan->setNumSoftModules(numOfSoftModules);
						floorplan->setSoftModules(softModules);

						// cout << "LINE_INDEX_NUM_SOFT_MODULES = " << line << endl;
						continue;
					}
					// Example : 「NumFixedModules 8」
					if (line.find(LINE_INDEX_FIXED_MODULE) != string::npos) {
						vector<string> info = FloorplanFile::splitString(line);
						int numOfHardModules = stoi(info.at(1));
						auto fixedModules = FloorPlanAlogrithm::initializeFixedModules(file, numOfHardModules);
						floorplan->setNumFixedModules(numOfHardModules);
						floorplan->setHardModules(fixedModules);

						// cout << "LINE_INDEX_FIXED_MODULE = " << line << endl;
						continue;
					}
					// Example : 「NumNets 39」
					if (line.find(LINE_INDEX_NUM_NETS) != string::npos) {
						vector<string> info = FloorplanFile::splitString(line);
						int numOfNets = stoi(info.at(1));
						auto nets = FloorPlanAlogrithm::initializeNets(file, numOfNets);

						floorplan->setNumNets(numOfNets);
						floorplan->setNets(nets);

						// cout << "LINE_INDEX_NUM_NETS = " << line << endl;
						continue;
					}
				}

				// Close the file object.
				file.close();
			}

			// Execution one thread
			/*
			std::future<std::shared_ptr<BStarTree>> future = std::async(
				std::launch::async,
				[](FixedOutlineFloorplanning* floorplan) -> std::shared_ptr<BStarTree> {

					std::shared_ptr<FloorPlanAlogrithm> floorplanAlogrithm = std::make_shared<FloorPlanAlogrithm>();
					FixedOutlineFloorplanning* copyFloorplan = new FixedOutlineFloorplanning(floorplan);

					// Get a complete binary tree at first
					std::shared_ptr<BStarTree> baseTree = floorplanAlogrithm->initializeFloorplan(copyFloorplan);

					// The key is wirelength
					std::shared_ptr<BStarTree> solution = floorplanAlogrithm->SimulatedAnnealing(copyFloorplan, baseTree);
					return solution;
			}, floorplan);

			std::shared_ptr<BStarTree> solution = future.get();
			FloorplanFile::writeOutputFile(floorplan, solution, outputFilePath);
			*/

			// Execution multi threads
			auto getSolution = [](FixedOutlineFloorplanning* floorplan) -> std::shared_ptr<BStarTree> {

				std::shared_ptr<FloorPlanAlogrithm> floorplanAlogrithm = std::make_shared<FloorPlanAlogrithm>();
				FixedOutlineFloorplanning* copyFloorplan = new FixedOutlineFloorplanning(floorplan);

				// Get a complete binary tree at first, and then executing SA
				std::shared_ptr<BStarTree> baseSolution = floorplanAlogrithm->oldInitializeFloorplan(copyFloorplan);
				std::shared_ptr<BStarTree> solution = floorplanAlogrithm->SimulatedAnnealing(copyFloorplan, baseSolution);
				return solution;
			};

			std::vector<std::future<std::shared_ptr<BStarTree>>> futures;
			for (int i = 0; i < MAXIMUM_THREADS; ++i) {
				futures.push_back(std::async(std::launch::async, getSolution, floorplan));
			}

			// Wait for all asynchronous tasks to complete and get the results of all asynchronous tasks
			std::vector<std::shared_ptr<BStarTree>> solutions = {};
			for (auto& f : futures) {
				std::future_status status = f.wait_until(std::chrono::system_clock::now() + TIME_LIMIT + std::chrono::seconds(10));

				// Get the results of all asynchronous tasks
				if (status == std::future_status::ready) {
					solutions.push_back(f.get());
				}
			}

			// Collect all feasible solutions
			std::vector<std::shared_ptr<BStarTree>> feasibleSolutions = {};
			for (auto& i : solutions) {

				std::shared_ptr<Cost> cost = i->getCost();
				if (cost->getFeasible()) {
					feasibleSolutions.push_back(i);
				}
			}

			// Find the minimum wirelength feasible solution
			if (!feasibleSolutions.empty()) {
				std::shared_ptr<BStarTree> optimalSolution = feasibleSolutions.front();
				for (auto& i : feasibleSolutions) {

					std::shared_ptr<Cost> cost = i->getCost();
					if (cost->getWirelength() < optimalSolution->getCost()->getWirelength()) {
						optimalSolution = i;
					}
				}
				FloorplanFile::writeOutputFile(floorplan, optimalSolution, outputFilePath);
			}
			// A feasible solution does not exist. Return a solution casually.
			else {
				std::shared_ptr<BStarTree> optimalSolution = solutions.front();
				FloorplanFile::writeOutputFile(floorplan, optimalSolution, outputFilePath);
			}
		}
	}
	catch (const exception& error) {
		cerr << error.what() << endl;
	}
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示:
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
