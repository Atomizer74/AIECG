#pragma once

#include <chrono>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>


struct ProfileInfo
{
	std::string name;
	std::vector<std::chrono::nanoseconds> frameTimes;
	int runCount;
	std::chrono::nanoseconds total;
	std::chrono::nanoseconds max;
	std::chrono::nanoseconds min;
};


class ProfileNode;
class Profile
{
	static ProfileNode* _currentNode;
	static ProfileNode* _root;

	static void GenerateProfileData(std::vector<ProfileInfo>& outData, std::string name, ProfileNode* current);

public:

	static void Begin(const char* szName);
	static void End();

	static std::vector<ProfileInfo> ProfileData();
};

class ProfileNode
{
	const int FRAMETIMES_MAXCOUNT = 20;

	friend Profile;

	std::string _name;
	ProfileNode* _parent;
	std::vector<ProfileNode*> _children;

	std::chrono::high_resolution_clock::time_point _start;

	std::queue<std::chrono::nanoseconds> _frameTimes;
	int _runCount;
	std::chrono::nanoseconds _total;
	std::chrono::nanoseconds _max;
	std::chrono::nanoseconds _min;


	ProfileNode(const char* szName, ProfileNode* pParent) : _runCount(0), _name(szName), _parent(pParent), _total(0), _max(std::chrono::nanoseconds::min()), _min(std::chrono::nanoseconds::max())
	{
		if (_parent != nullptr)
			_parent->_children.push_back(this);
	}
	~ProfileNode()
	{
		if (_parent != nullptr)
			_parent->_children.erase(std::remove(_parent->_children.begin(), _parent->_children.end(), this), _parent->_children.end());
	}

	void Begin()
	{
		_start = std::chrono::high_resolution_clock::now();
	}

	void End()
	{
		auto end = std::chrono::high_resolution_clock::now();

		auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - _start);
		_frameTimes.push(time);
		_total += time;
		_runCount++;

		_min = (time < _min) ? time : _min;
		_max = (time > _max) ? time : _max;

		if ((int)_frameTimes.size() >= FRAMETIMES_MAXCOUNT)
			_frameTimes.pop();
	}

};