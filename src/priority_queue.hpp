#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct node {
		T data;
		node *left;
		node *right;
		int dist; // distance to the nearest leaf (null node)

		node(const T &val) : data(val), left(nullptr), right(nullptr), dist(0) {}
	};

	node *root;
	size_t node_count;
	Compare cmp;

	// Helper function to calculate the distance of a node
	int get_dist(node *n) const {
		return n ? n->dist : -1;
	}

	// Helper function to merge two nodes (the core operation of leftist heap)
	node* merge_nodes(node *a, node *b) {
		if (!a) return b;
		if (!b) return a;

		// Ensure a has the larger priority (for max heap)
		if (cmp(a->data, b->data)) {
			std::swap(a, b);
		}

		// Merge b into a's right subtree
		a->right = merge_nodes(a->right, b);

		// Maintain leftist property: left child's dist >= right child's dist
		if (get_dist(a->left) < get_dist(a->right)) {
			std::swap(a->left, a->right);
		}

		// Update distance
		a->dist = get_dist(a->right) + 1;

		return a;
	}

	// Helper function to delete all nodes
	void clear_nodes(node *n) {
		if (!n) return;
		clear_nodes(n->left);
		clear_nodes(n->right);
		delete n;
	}

	// Helper function to clone a tree
	node* clone_nodes(node *n) {
		if (!n) return nullptr;
		node *new_node = new node(n->data);
		new_node->left = clone_nodes(n->left);
		new_node->right = clone_nodes(n->right);
		new_node->dist = n->dist;
		return new_node;
	}

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), node_count(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(nullptr), node_count(0) {
		root = clone_nodes(other.root);
		node_count = other.node_count;
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		clear_nodes(root);
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this != &other) {
			clear_nodes(root);
			root = clone_nodes(other.root);
			node_count = other.node_count;
		}
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if (empty()) {
			throw container_is_empty();
		}
		return root->data;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		node *new_node = new node(e);
		root = merge_nodes(root, new_node);
		node_count++;
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		if (empty()) {
			throw container_is_empty();
		}
		node *old_root = root;
		root = merge_nodes(root->left, root->right);
		delete old_root;
		node_count--;
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
		return node_count;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return root == nullptr;
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		root = merge_nodes(root, other.root);
		node_count += other.node_count;
		other.root = nullptr;
		other.node_count = 0;
	}
};

}

#endif