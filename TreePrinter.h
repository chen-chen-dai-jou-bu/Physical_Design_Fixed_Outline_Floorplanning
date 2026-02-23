#pragma once
#ifndef TREE_PRINTER
#define TREE_PRINTER

#include <string>

#include "BStarTree.h"


class Trunk {
public:
	Trunk* prev;
	std::string str;

	Trunk(Trunk* prev, std::string str) {
		this->prev = prev;
		this->str = str;
	}

	// Helper function to print branches of the binary tree
	static void showTrunks(Trunk* p) {
		if (p == nullptr) {
			return;
		}

		showTrunks(p->prev);
		std::cout << p->str;
	}
};

class TreePrinter {
	public:
		static void printTree(Node* root, Trunk* prev, bool isLeft) {
			if (root == nullptr) {
				return;
			}

			std::string prev_str = "    ";
			Trunk* trunk = new Trunk(prev, prev_str);

			printTree(root->getRight(), trunk, true);

			if (!prev) {
				trunk->str = "¡X¡X¡X";
			}
			else if (isLeft) {
				trunk->str = ".¡X¡X¡X";
				prev_str = "   |";
			}
			else {
				trunk->str = "`¡X¡X¡X";
				prev->str = prev_str;
			}

			Trunk::showTrunks(trunk);
			std::cout << " " << root->getModule()->getName() << std::endl;

			if (prev) {
				prev->str = prev_str;
			}
			trunk->str = "   |";

			printTree(root->getLeft(), trunk, false);
		}
};


#endif /* TREE_PRINTER */