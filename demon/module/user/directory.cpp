#include "stdafx.h"
#include "directory.h"

DirectoryTree::DirectoryTree()
{
	root_node_ = new DirectoryNode("", 0);
}

DirectoryTree::~DirectoryTree()
{
	if (root_node_) {

	}
}

void DirectoryTree::Add(std::string node_code, DirectoryNode *node)
{
	DirectoryNode *parent = nullptr;
	parent = FindNode(node_code, root_node_);
	
	if (parent){
		node->parent = parent;
		if (parent->child) {
			DirectoryNode *pos = parent->child;
			while (pos->subling)
			{
				pos = pos->subling;
			}

			pos->subling = node;
		}
		else {
			parent->child = node;
		}
	}
}

std::vector<DirectoryNode> DirectoryTree::GetChildNodes(std::string node_code)
{
	std::vector<DirectoryNode> child_nodes;

	DirectoryNode *parent = FindNode(node_code, Root());
	if (parent && parent->child) {
		DirectoryNode *node = parent->child;
		while (node)
		{
			child_nodes.push_back(*node);
			node = node->subling;
		}
	}

	return child_nodes;
}

DirectoryNode * DirectoryTree::FindNode(std::string node_code, DirectoryNode * node)
{
	if (node_code.empty()) {
		return root_node_;
	}

	DirectoryNode *tmp_node = nullptr;
	if (node == nullptr) {
		return tmp_node;
	}

	if (node->node_code == node_code) {
		return node;
	}

	if (node->child) {
		tmp_node = FindNode(node_code, node->child);
		if (tmp_node) {
			return tmp_node;
		}
	}
	
	if (node->subling) {
		tmp_node = FindNode(node_code, node->subling);
		if (tmp_node) {
			return tmp_node;
		}
	}

	return tmp_node;
}
