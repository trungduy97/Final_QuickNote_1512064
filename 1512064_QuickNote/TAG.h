#pragma once
#include "stdafx.h"
class TAG
{
public:
	wstring TagName;
	vector<long> Id;
public:

	TAG()
	{
	}
	TAG(wstring tagName)
	{
		TagName = tagName;
	}
	TAG(wstring tagName, long id)
	{
		TagName = tagName;
		Id.push_back(id);
	}
	TAG(wstring tagName, vector<long> id)
	{
		TagName = tagName;
		Id = id;
	}

	~TAG()
	{
	}
};

