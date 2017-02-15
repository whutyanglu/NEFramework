#ifndef MSYK_MODULE_USER_DIRECTORY_H__
#define MSYK_MODULE_USER_DIRECTORY_H__

#include <string>

struct DirectoryNode
{
	DirectoryNode() {}
	DirectoryNode(std::string node_code, int code_level)
		: node_code(node_code)
		, code_level(code_level)
	{
		// TODO :
		name = node_code;
	}

	std::string id;
	std::string description;
	std::string name;
	bool is_end = false;
	std::string tag_id;
	std::string dir_id;
	int code_level = 0; // root
	std::string node_code;

	// TODO : luyang
	DirectoryNode *parent  = nullptr;
	DirectoryNode *child   = nullptr;
	DirectoryNode *subling = nullptr;
};

struct DirectoryNodeVideo
{
	DirectoryNodeVideo() {}
	DirectoryNodeVideo(int i = 0, std::string n = "", std::string ut = "", std::wstring pp = L"")
		: id(i), name(n), upload_time(ut), picture_path(pp)
	{}

	int id;
	std::string name;
	std::string upload_time;
	std::string url;
	std::string review_url;
	std::string picture_url;
	std::wstring picture_path;
};

using DirNodeVideoVec = std::vector<DirectoryNodeVideo>;

class DirectoryTree
{
public:
	DirectoryTree();
	virtual ~DirectoryTree();

	void Add(std::string node_code, DirectoryNode *node);
	std::vector<DirectoryNode> GetChildNodes(std::string node_code);
	DirectoryNode *FindNode(std::string node_code, DirectoryNode *node);
	DirectoryNode *Root(){ return root_node_; }

private:
	DirectoryNode *root_node_;
};


#endif // ~MSYK_MODULE_USER_DIRECTORY_H__