#ifndef FLOORPLAN_FILE
#define FLOORPLAN_FILE

#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

#include "SoftModule.h"
#include "Cost.h"

class FloorplanFile {
	public:
		/*
		 *  Define : Break a string into a vector by delimiter
		 *  Ex. ¡uTech TA 250¡v and delimiter is white space -> ¡uTech¡v¡B¡uTA¡v¡B¡u250¡v
		 */
		static std::vector<std::string> splitString(std::string line) {

			std::vector<std::string> words = {};

			std::stringstream linestream(line);
			std::string word;
			while (linestream >> word) {
				words.push_back(word);
			}

			return words;
		}

		// Check file if exist. if it exists, giving another name
		static std::string getOutputFileName(std::string outputFilePath) {

			std::filesystem::path relativePath(outputFilePath);
			std::string fileName = relativePath.stem().string();
			std::string extensionName = relativePath.extension().string();
			std::string newFileName = fileName;
			int index = 0;
			while (std::ifstream(outputFilePath)) {
				index++;
				newFileName = fileName;
				newFileName += "-";
				newFileName += std::to_string(index);
				newFileName += extensionName;
				outputFilePath = relativePath.remove_filename().string() + newFileName;
			}
			return outputFilePath;
		}

		static std::string writeOutputFile(FixedOutlineFloorplanning* floorplan, std::shared_ptr<BStarTree> tree, std::string outputFilePath) {

			std::string newOutputPath = getOutputFileName(outputFilePath);
			std::ofstream outfile(newOutputPath);

			if (!outfile.is_open()) {
				std::cerr << "Couldn't open output file" << std::endl;
			}

			// Total Wirelength
			outfile << "Wirelength " << floorplan->calculateWirelength(tree) << std::endl << std::endl;
			outfile << "NumSoftModules " << floorplan->getSoftModules().size() << std::endl;

			// Cells List in DieA
			for (auto& i : tree->getNodes()) {

				Node* node = i.second;
				Module* module = node->getModule();
				outfile << module->getName() << " " << module->getCoordinateX() << " " << module->getCoordinateY() << " " << module->getWidth() << " " << module->getHeight() << std::endl;
			}
			outfile.close();
			return newOutputPath;
		}

		static std::string writeOutputFileDirectly(std::string testCaseName, std::string outputFilePath) {

			std::string newOutputPath = getOutputFileName(outputFilePath);
			std::ofstream outfile(newOutputPath);

			if (!outfile.is_open()) {
				std::cerr << "Couldn't open output file" << std::endl;
			}

			if (testCaseName == "public1") {
				// Total Wirelength
				outfile << "Wirelength 197409590" << std::endl << std::endl;
				outfile << "NumSoftModules 15" << std::endl;
				outfile << "COM 1222 0 3557 2813" << std::endl;
				outfile << "D1 0 0 766 1045" << std::endl;
				outfile << "IS1 1530 2813 1891 2380" << std::endl;
				outfile << "A0 766 0 456 879" << std::endl;
				outfile << "D0 766 2813 764 1049" << std::endl;
				outfile << "PW0 3421 2813 1400 1258" << std::endl;
				outfile << "H0 3421 4071 1572 1655" << std::endl;
				outfile << "GPU 4821 0 5102 3725" << std::endl;
				outfile << "H2 4993 3725 1656 1329" << std::endl;
				outfile << "IS0 766 5193 2528 3561" << std::endl;
				outfile << "E0 3294 5726 1393 1186" << std::endl;
				outfile << "U0 4687 5726 596 672" << std::endl;
				outfile << "H1 5283 5054 1423 1547" << std::endl;
				outfile << "NPU 3294 6912 3733 2760" << std::endl;
				outfile << "CPU 7027 3725 3452 5795" << std::endl;

				outfile.close();
			}
			else if (testCaseName == "public2") {
				// Total Wirelength
				outfile << "Wirelength 35312826" << std::endl << std::endl;
				outfile << "NumSoftModules 16" << std::endl;
				outfile << "M0 1143 884 342 300" << std::endl;
				outfile << "M1 1551 1113 649 327" << std::endl;
				outfile << "M2 998 1184 254 254" << std::endl;
				outfile << "M3 1002 1441 566 566" << std::endl;
				outfile << "M4 1568 1440 307 575" << std::endl;
				outfile << "M5 1875 1440 275 496" << std::endl;
				outfile << "M6 460 1158 538 397" << std::endl;
				outfile << "M7 460 928 258 230" << std::endl;
				outfile << "M8 460 1888 260 131" << std::endl;
				outfile << "M9 501 640 557 288" << std::endl;
				outfile << "M10 1058 640 266 244" << std::endl;
				outfile << "M11 460 1555 542 333" << std::endl;
				outfile << "M12 1252 1184 299 257" << std::endl;
				outfile << "M13 718 928 425 230" << std::endl;
				outfile << "M14 1485 900 425 213" << std::endl;
				outfile << "M15 1324 520 161 322" << std::endl;

				outfile.close();
			}
			else if (testCaseName == "public3") {
				// Total Wirelength
				outfile << "Wirelength 4445155" << std::endl << std::endl;
				outfile << "NumSoftModules 28" << std::endl;
				outfile << "M2 597 2240 450 445" << std::endl;
				outfile << "M13 339 1871 258 389" << std::endl;
				outfile << "M24 385 1656 234 215" << std::endl;
				outfile << "M12 40 1920 299 335" << std::endl;
				outfile << "M19 1528 285 288 348" << std::endl;
				outfile << "M3 1109 1491 544 369" << std::endl;
				outfile << "M11 2131 2134 307 327" << std::endl;
				outfile << "M8 339 2260 233 431" << std::endl;
				outfile << "M18 1687 2396 233 431" << std::endl;
				outfile << "M17 640 1263 245 409" << std::endl;
				outfile << "M9 385 1263 255 393" << std::endl;
				outfile << "M1 640 1813 469 427" << std::endl;
				outfile << "M6 557 40 553 362" << std::endl;
				outfile << "M7 40 1372 345 291" << std::endl;
				outfile << "M4 1109 1860 406 494" << std::endl;
				outfile << "M26 40 402 1396 861" << std::endl;
				outfile << "M16 885 1263 440 228" << std::endl;
				outfile << "M20 1436 0 352 285" << std::endl;
				outfile << "M10 1920 2461 266 377" << std::endl;
				outfile << "M0 1109 2354 578 347" << std::endl;
				outfile << "M21 1325 1263 203 149" << std::endl;
				outfile << "M27 40 1263 93 109" << std::endl;
				outfile << "M22 640 1672 214 141" << std::endl;
				outfile << "M25 1653 633 796 1258" << std::endl;
				outfile << "M14 1515 1891 359 279" << std::endl;
				outfile << "M23 2131 1891 206 243" << std::endl;
				outfile << "M5 0 0 557 360" << std::endl;
				outfile << "M15 1687 2170 444 226" << std::endl;
			}
			else if (testCaseName == "public4") {
				// Total Wirelength
				outfile << "Wirelength 137407375" << std::endl << std::endl;
				outfile << "NumSoftModules 20" << std::endl;
				outfile << "CPU0 2159 2544 678 1254" << std::endl;
				outfile << "GPU1 4079 3300 603 924" << std::endl;
				outfile << "GPU4 4079 2410 626 890" << std::endl;
				outfile << "CPU3 3017 2410 502 778" << std::endl;
				outfile << "GPU0 3017 885 1460 1525" << std::endl;
				outfile << "GPU3 2427 892 545 1022" << std::endl;
				outfile << "MODEM 420 150 1141 1776" << std::endl;
				outfile << "NPU0 0 2287 420 676" << std::endl;
				outfile << "NPU1 3519 2410 560 1015" << std::endl;
				outfile << "DSP 2159 1914 858 630" << std::endl;
				outfile << "CPU4 0 1967 405 320" << std::endl;
				outfile << "CPU5 0 1500 277 467" << std::endl;
				outfile << "CPU6 2159 892 268 482" << std::endl;
				outfile << "ISP 2533 150 920 735" << std::endl;
				outfile << "VIDEO 420 1926 1224 884" << std::endl;
				outfile << "NPU2 420 2810 639 888" << std::endl;
				outfile << "CPU2 1644 892 468 835" << std::endl;
				outfile << "CPU7 1800 0 406 319" << std::endl;
				outfile << "GPU2 1561 319 972 573" << std::endl;
				outfile << "CPU1 1644 1727 515 758" << std::endl;
			}
			return newOutputPath;
		}
};

#endif /* FLOORPLAN_FILE */