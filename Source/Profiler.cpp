#include "Profiler.h"

ProfileNode* Profile::_root = nullptr;
ProfileNode* Profile::_currentNode = nullptr;

void Profile::Begin(const char* szName)
{
	// If root node doesn't exist, we need to create one now
	if (_root == nullptr)
	{
		_root = new ProfileNode("ROOT", nullptr);
		_currentNode = _root;
	}

	bool foundNode = false;
	for each (auto node in _currentNode->_children)
	{
		if (node->_name == szName)
		{
			_currentNode = node; // If this node already exists, use it
			foundNode = true;
			break;
		}
	}

	if (!foundNode)
		_currentNode = new ProfileNode(szName, _currentNode);

	_currentNode->Begin();
}

void Profile::End()
{
	_currentNode->End();

	if (_currentNode == _root) // This shouldn't happen
		throw new std::exception("Attempting to Profile::End() on the root node");

	_currentNode = _currentNode->_parent;
}

std::vector<ProfileInfo> Profile::ProfileData()
{
	std::vector<ProfileInfo> profileMap;

	for each (auto node in _root->_children)
	{
		GenerateProfileData(profileMap, "", node);
	}
	

	return profileMap;
}

void Profile::GenerateProfileData(std::vector<ProfileInfo>& outData, std::string name, ProfileNode* current)
{
	// Append current name to the name string
	if (name.length() != 0)
		name += "::";
	name += current->_name;

	// Generate the frame data
	std::vector<std::chrono::nanoseconds> frameData;
	auto tempQueue = current->_frameTimes;
	
	while (tempQueue.size() > 0)
	{
		auto frame = tempQueue.front();
		frameData.push_back(frame);
		tempQueue.pop();
	}

	// Add this nodes data to the map
	ProfileInfo profile;
	profile.name = name;
	profile.frameTimes = frameData;
	profile.runCount = current->_runCount;
	profile.total = current->_total;
	profile.max = current->_max;
	profile.min = current->_min;
	outData.push_back(profile);

	for each (auto node in current->_children) // Base case is when no child nodes exist
	{
		GenerateProfileData(outData, name, node);
	}
}