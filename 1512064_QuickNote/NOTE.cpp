#include"stdafx.h"
#include"NOTE.h"

NOTE::NOTE()
{
}
NOTE::NOTE(long id, vector<wstring> tag, wstring content)
{
	Id = id;
	Tag = tag;
	Content = content;
}
NOTE::~NOTE()
{
}