#pragma once
#ifndef B_STAR_TREE
#define B_STAR_TREE

#include <unordered_map>
#include <queue>
#include <random>
#include <regex>


#include "Module.h"
#include "SoftModule.h"
#include "Contour.h"
#include "Probability.h"
#include "Cost.h"

class Node {
	private:
		Node* parent;
		Node* left;
		Node* right;
		Module* module;
		std::string name;

	public:
		Node() {
			throw std::runtime_error("Node constructor shouldn't be called with parameter.");
		}

		Node(Module* _module) {
			this->name = _module->getName();
			this->module = _module;
			this->parent = NULL;
			this->left = NULL;
			this->right = NULL;
		}

		~Node() {
			delete (this->module);
		}

		std::string getName() {
			return this->name;
		}

		Module* getModule() {
			return this->module;
		}

		Node* getLeft() {
			return this->left;
		}

		Node* getRight() {
			return this->right;
		}

		Node* getParent() {
			return this->parent;
		}

		void setName(std::string _name) {
			this->name = _name;
		}

		void setModule(Module* _module) {
			this->module = _module;
		}

		void setLeft(Node* _left) {
			this->left = _left;
		}

		void setRight(Node* _right) {
			this->right = _right;
		}

		void setParent(Node* _parent) {
			this->parent = _parent;
		}
};

class Point {
	public :
		unsigned long x, y;
};

class BStarTree {

	private:
		Node* root = NULL;
		std::unordered_map<std::string, Node*> nodes = {};
		std::shared_ptr<ProbabilityModel> probabilityModel = std::make_shared<ProbabilityModel>();
		std::shared_ptr<Cost> cost = NULL;

	public:
		BStarTree(Node* _root) {

			this->root = _root;
			this->nodes[_root->getName()] = root;
		}

		BStarTree(std::shared_ptr<BStarTree> tree) {

			this->root = copyTree(tree->getRoot(), NULL);
			if (tree->getCost() != NULL) {
				this->cost = std::make_shared<Cost>(tree->getCost());
			}
			// setupParentPointers(this->root, this->root->getParent());
		}

		~BStarTree() {
			// To do something destructor
			deleteTree(this->root);
		}

		void deleteTree(Node* node) {
			if (node == NULL) {
				return;
			}
			else {
				deleteTree(node->getLeft());
				deleteTree(node->getRight());
			}
			delete(node);
		}

		std::unordered_map<std::string, Node*> getNodes() {
			return this->nodes;
		}

		Node* getRoot() {
			return this->root;
		}

		void setCost(std::shared_ptr<Cost> _cost) {
			this->cost = _cost;
		}

		std::shared_ptr<Cost> getCost() {
			return this->cost;
		}

		void setupParentPointers(Node* root, Node* parent) {

			if (root == NULL) {
				return;
			}
			else {
				root->setParent(parent);
				setupParentPointers(root->getLeft(), root);
				setupParentPointers(root->getRight(), root);
			}
		}

		Node* copyTree(Node* source, Node* parent) {

			if (source == NULL) {
				return NULL;
			}
			else {

				// TODO Code can be improved
				// Copy Data
				SoftModule module = (SoftModule*)source->getModule();
				SoftModule* copyModule = new SoftModule(module);

				Node* node = new Node(copyModule);
				this->nodes[node->getName()] = node;

				if (parent != NULL) {
					node->setParent(parent);
				}

				node->setLeft(copyTree(source->getLeft(), node));
				node->setRight(copyTree(source->getRight(), node));

				return node;
			}
		}

		// For creating a initial floorplan (Complete Binary Tree)
		Node* initializedInsert(SoftModule* module) {

			std::queue<Node*> queue;
			queue.push(this->root);

			Node* newNode = new Node(module);
			while (!queue.empty()) {
				Node* node = queue.front();
				queue.pop();
				if (node->getLeft() && node->getRight()) {
					queue.push(node->getLeft());
					queue.push(node->getRight());
					continue;
				}
				// Put the module on right side
				if (!node->getLeft()) {

					node->setLeft(newNode);
					newNode->setParent(node);
					this->nodes[module->getName()] = newNode;

					return newNode;
				}
				// Put the module on upper side
				if (!node->getRight()) {

					node->setRight(newNode);
					newNode->setParent(node);
					this->nodes[module->getName()] = newNode;

					return newNode;
				}
			}
			return NULL;
		} // end of function

		Node* findByNodeName(Node* node, std::string name) {

			// Traversing the tree in InOrder way using recursion
			if (node == NULL) {
				return NULL;
			}
			else {
				if (node->getName() == name) {
					return node;
				}
				if (node->getLeft() != NULL) {
					Node* temp = findByNodeName(node->getLeft(), name);
					if (temp != NULL) {
						if (temp->getName() == name) {
							return temp;
						}
					}
				}
				if (node->getRight() != NULL) {
					Node* temp = findByNodeName(node->getRight(), name);
					if (temp != NULL) {
						if (temp->getName() == name) {
							return temp;
						}
					}
				}
				return NULL;
			}
		}

		void checkParent(Node* node) {

			// Traversing the tree in InOrder way using recursion
			if (node == NULL) {
				return;
			}

			if (node->getParent() == NULL) {
				std::cout << node->getName() << ", parent = NULL" << std::endl;
			}
			else {
				std::cout << node->getName() << ", parent =" << node->getParent()->getName() << std::endl;
			}

			if (node->getLeft() != NULL) {
				if (node->getLeft()->getParent()->getName() != node->getName()) {
					throw std::runtime_error("Left Child's parent pointer is not correct.");
				}
				checkParent(node->getLeft());
			}
			if (node->getRight() != NULL) {
				if (node->getRight()->getParent()->getName() != node->getName()) {
					throw std::runtime_error("Right Child's pointer is not correct.");
				}
				checkParent(node->getRight());
			}
		}

		void display(Node* root) {

			// Traversing the tree in InOrder way using recursion
			if (root == NULL) {
				return;
			}
			display(root->getLeft());
			Module* module = root->getModule();
			std::cout << "name = " << module->getName() << ", x-coordinate = " << module->getCoordinateX() << ", y-coordinate = " << module->getCoordinateY();
			std::cout << ", width = " << module->getWidth() << ", height = " << module->getHeight() << std::endl;
			display(root->getRight());
		}

		// For randomly choose a node from B*-Tree
		Node* returnRandomNode() {

			// Method ONE (too slow because everytime have to create the uniform_int_distribution)
			// std::random_device rd;
			// std::mt19937 gen(rd());
			// std::uniform_int_distribution<int> uniformIntegerDistribution(0, this->nodes.size() - 1);
			// std::default_random_engine randomEngine(gen);
			// std::advance(it, uniformIntegerDistribution(randomEngine));
			// return nodes[it->first];

			// Method TWO
			auto it = nodes.begin();
			int index = rand() % nodes.size();
			std::advance(it, index);

			return nodes[it->first];
		}

		// For randomly choose two nodes from B*-Tree
		std::pair<Node*, Node*> returnRandomTwoNodes() {

			auto iterateFirst = nodes.begin();
			int index = rand() % nodes.size();
			std::advance(iterateFirst, index);

			auto iterateSecond = nodes.begin();
			std::advance(iterateSecond, index == this->nodes.size() - 1 ? 0 : index + 1);

			return std::make_pair(nodes[iterateFirst->first], nodes[iterateSecond->first]);
		}

		// Move - delete
		Node* deleteNode(Node* node) {

			Node* deletedNode = NULL;
			if (node == NULL) {
				throw std::runtime_error("Error: Can't find node in deleteByNodeName.");
			}

			Node* parent = node->getParent();
			// Root Node
			if (parent == NULL) {
				return NULL;
			}

			// Case.1 no children
			if (node->getLeft() == NULL && node->getRight() == NULL) {

				// node is parent's left child
				if (parent->getLeft() != NULL) {

					Node* parentLeftNode = parent->getLeft();
					if (parentLeftNode->getName() == node->getName()) {
						parent->setLeft(NULL);
					}
				}

				// node is parent's right child
				if (parent->getRight() != NULL) {

					Node* parentRightNode = parent->getRight();
					if (parentRightNode->getName() == node->getName()) {
						parent->setRight(NULL);
					}
				}

				// Node* check = findByNodeName(this->root, parent->getName());
				deletedNode = node;
			} // end of Case.1


			// Case.2 one child (left child)
			else if (node->getLeft() != NULL && node->getRight() == NULL) {

				Node* left = node->getLeft();

				// node is parent's left child
				if (parent->getLeft() != NULL) {

					Node* parentLeftNode = parent->getLeft();
					if (parentLeftNode->getName() == node->getName()) {
						left->setParent(parent);
						parent->setLeft(left);
						node->setLeft(NULL);
					}
				}
				// node is parent's right child
				if (parent->getRight() != NULL) {

					Node* parentRightNode = parent->getRight();
					if (parentRightNode->getName() == node->getName()) {
						left->setParent(parent);
						parent->setRight(left);
						node->setLeft(NULL);
					}
				}

				// Node* check = findByNodeName(this->root, parent->getModule()->getName());
				deletedNode = node;
			} // end of Case.2

			// Case.3 one child (right child)
			else if (node->getLeft() == NULL && node->getRight() != NULL) {

				Node* right = node->getRight();

				// node is parent's left child
				if (parent->getLeft() != NULL) {

					Node* parentLeftNode = parent->getLeft();
					if (parentLeftNode->getModule()->getName() == node->getModule()->getName()) {
						right->setParent(parent);
						parent->setLeft(right);
						node->setRight(NULL);
					}
				}

				// node is parent's right child
				if (parent->getRight() != NULL) {

					Node* parentRightNode = parent->getRight();
					if (parentRightNode->getModule()->getName() == node->getModule()->getName()) {
						right->setParent(parent);
						parent->setRight(right);
						node->setRight(NULL);
					}
				}

				if (node->getLeft() != NULL || node->getRight() != NULL) {
					throw std::runtime_error("Error: wrong relationship between the node and the parent. Case.3");
				}

				deletedNode = node;
			} // end of Case.3


			// Case.4 two children
			else {

				Node* leaf = this->findRandomLeaf(node);
				Node* leafParent = leaf->getParent();

				// The child node and the parent node differ by only one level.
				if (leafParent->getName() == node->getName()) {

					// Leaf is node's left child
					if (node->getLeft()->getName() == leaf->getName()) {
						leaf->setRight(node->getRight());
						leaf->setParent(parent);
						node->getRight()->setParent(leaf);
					}
					// Leaf is node's right child
					else {
						leaf->setLeft(node->getLeft());
						leaf->setParent(parent);
						node->getLeft()->setParent(leaf);
					}

					// node is parent's left child
					if (parent->getLeft() != NULL) {

						Node* parentLeftNode = parent->getLeft();
						if (parentLeftNode->getName() == node->getName()) {
							parent->setLeft(leaf);
						}
					}
					// node is parent's right child
					if (parent->getRight() != NULL) {

						Node* parentRightNode = parent->getRight();
						if (parentRightNode->getName() == node->getName()) {
							parent->setRight(leaf);
						}
					}
					node->setLeft(NULL);
					node->setRight(NULL);

					deletedNode = node;
				}
				else {
					leaf->setParent(parent);
					leaf->setRight(node->getRight());
					leaf->setLeft(node->getLeft());

					node->getRight()->setParent(leaf);
					node->getLeft()->setParent(leaf);


					// leaf is leafParent's left child
					if (leafParent->getLeft() != NULL) {

						Node* leafParentLeftNode = leafParent->getLeft();
						if (leafParentLeftNode->getName() == leaf->getName()) {
							leafParent->setLeft(NULL);
						}
					}
					// leaf is leafParent's right child
					if (leafParent->getRight() != NULL) {

						Node* leafParentRightNode = leafParent->getRight();
						if (leafParentRightNode->getName() == leaf->getName()) {
							leafParent->setRight(NULL);
						}
					}

					// node is parent's left child
					if (parent->getLeft() != NULL) {

						Node* parentLeftNode = parent->getLeft();
						if (parentLeftNode->getName() == node->getName()) {
							parent->setLeft(leaf);
						}
					}
					// node is parent's right child
					if (parent->getRight() != NULL) {

						Node* parentRightNode = parent->getRight();
						if (parentRightNode->getName() == node->getName()) {
							parent->setRight(leaf);
						}
					}
					node->setLeft(NULL);
					node->setRight(NULL);

					deletedNode = node;
				}

			} // end of Case.4


			if (deletedNode != NULL) {

				deletedNode->setParent(NULL);
				int deleteNum = this->nodes.erase(deletedNode->getName());
				if (deleteNum == 0) {
					throw std::runtime_error("DeleteNode Error: deleteNum = 0.");
				}

				if (deletedNode->getLeft() != NULL || deletedNode->getRight() != NULL || deletedNode->getParent() != NULL) {
					throw std::runtime_error("DeleteNode Error: DeletedNode has a non NULL pointer.");
				}
			}

			return deletedNode;
		}

		static double getProbability() {

			// std::random_device rd;
			// std::mt19937 gen(rd());
			// std::uniform_real_distribution<double> uniformRealDistribution(0, 1);
			// std::default_random_engine randomEngine(gen);

			// Getting a random double value
			// return uniformRealDistribution(randomEngine);
			return ((double)rand() / (RAND_MAX));
		}

		Node* findRandomLeaf(Node* node) {

			Node* leaf = node;
			while (leaf->getLeft() != NULL || leaf->getRight() != NULL) {

				if (leaf->getLeft() == NULL && leaf->getRight() != NULL) {
					leaf = leaf->getRight();
				}
				else if (leaf->getLeft() != NULL && leaf->getRight() == NULL) {
					leaf = leaf->getLeft();
				}
				else {
					if (probabilityModel->getProbability() > 0.5) {
						leaf = leaf->getLeft();
					}
					else {
						leaf = leaf->getRight();
					}
				}
			}
			return leaf;
		}

		// Move - insert
		void insertNode(Node* node) {

			if (node->getLeft() != NULL || node->getRight() != NULL || node->getParent() != NULL) {
				throw std::runtime_error("InsertNode Error: Node has a non NULL pointer.");
			}

			while (true) {

				Node* parent = this->returnRandomNode();
				if (parent->getModule()->getName() == node->getModule()->getName()) {
					throw std::runtime_error("InsertNode Error: parent has same module with node.");
				}

				if (parent->getLeft() != NULL && parent->getRight() != NULL) {
					if (probabilityModel->getProbability() > 0.5) {
						// node as parent's left child
						node->setParent(parent);
						node->setLeft(parent->getLeft());
						node->setRight(parent->getRight());

						node->getLeft()->setParent(node);
						node->getRight()->setParent(node);
						parent->setLeft(node);
						parent->setRight(NULL);
					}
					else {
						// node as parent's right child
						node->setParent(parent);
						node->setLeft(parent->getLeft());
						node->setRight(parent->getRight());

						node->getLeft()->setParent(node);
						node->getRight()->setParent(node);
						parent->setRight(node);
						parent->setLeft(NULL);
					}
				}
				else if (parent->getLeft() == NULL && parent->getRight() != NULL) {
					node->setParent(parent);
					parent->setLeft(node);
				}
				else if (parent->getLeft() != NULL && parent->getRight() == NULL) {
					node->setParent(parent);
					parent->setRight(node);
				}
				else {
					if (probabilityModel->getProbability() > 0.5) {
						node->setParent(parent);
						parent->setLeft(node);
					}
					else {
						node->setParent(parent);
						parent->setRight(node);
					}
				}

				this->nodes[node->getName()] = node;
				break;
			} // end while
		}

		// Move - Swap
		void swap(Node* nodeA, Node* nodeB) {

			if (nodeA != NULL && nodeB != NULL) {

				int deleteNum = this->nodes.erase(nodeA->getName());
				if (deleteNum == 0) {
					throw std::runtime_error("Swap Error: deleteNum = 0.");
				}

				deleteNum = this->nodes.erase(nodeB->getName());
				if (deleteNum == 0) {
					throw std::runtime_error("Swap Error: deleteNum = 0.");
				}

				Module* moduleA = nodeA->getModule();
				std::string nodeNameA = nodeA->getName();

				nodeA->setName(nodeB->getName());
				nodeA->setModule(nodeB->getModule());
				nodeB->setModule(moduleA);
				nodeB->setName(nodeNameA);

				this->nodes[nodeA->getName()] = nodeA;
				this->nodes[nodeB->getName()] = nodeB;
			}
		}

		// Move - Rotate
		void rotate(std::string name) {

			Node* node = this->findByNodeName(this->root, name);
			if (node != NULL) {
				Module* module = node->getModule();
				unsigned long width = module->getWidth();
				module->setWidth(module->getHeight());
				module->setHeight(width);

				// Node* check = findByNodeName(this->root, node->getName());
			}
		}

		// Move - Rotate
		void rotate(Node* node) {

			if (node != NULL) {
				Module* module = node->getModule();
				unsigned long width = module->getWidth();
				module->setWidth(module->getHeight());
				module->setHeight(width);
			}
		}

		// Move - Resize
		void resize(std::string name) {

			Node* node = this->findByNodeName(this->root, name);
			if (node != NULL) {
				// TODO Code can be improved
				SoftModule* module = (SoftModule*)node->getModule();
				module->setRandomAsepectRatio();

				// Node* check = findByNodeName(this->root, node->getName());
			}
		}

		// Move - Resize
		void resize(Node* node) {

			if (node != NULL) {
				// TODO Code can be improved
				SoftModule* module = (SoftModule*)node->getModule();
				module->setRandomAsepectRatio();
			}
		}
};


#endif /* B_STAR_TREE */