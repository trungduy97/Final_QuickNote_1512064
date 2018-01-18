#pragma once
#include "stdafx.h"

class LIST
{
public:
	vector<NOTE> NoteList;
	vector<TAG> TagList;

	vector<wstring> addTag(wstring rawTag);
	wstring stdTag(wstring tag);
	wstring ListTag(vector<wstring> tagList);
	int saveListNote(vector<wstring> tags, wstring content, int currNoteId );
	int saveFile();
	int loadFile();	

};

