#include"stdafx.h"
#include "List.h"




vector<wstring> LIST::addTag(wstring rawTag)/////////////////////////
{
	wstringstream s;
	s << "Total notes, " << rawTag;
	rawTag = s.str();

	vector<wstring> listTag;
	wstringstream rawTags(rawTag);
	wstring sTag;
	bool b = false;
	while (getline(rawTags, sTag, L','))
	{
		sTag = stdTag(sTag);
		if (sTag != L"")
		{
			b = false;
			for (int i = 0; i < listTag.size(); i++)
			{
				if (sTag == listTag[i])
				{
					b = true;
					break;
				}
			}
			if (!b)
			{
				listTag.push_back(sTag);
			}
		}

	}
	return listTag;
}

wstring LIST::stdTag(wstring tag)///////////////////
{
	while (tag.size() > 1)
	{
		if (tag[tag.size() - 1] == ' ')
		{
			tag.pop_back();
		}
		else
		{
			break;
		}
	}
	while (tag.size() > 0)
	{
		if (tag[0] == ' ')
		{
			tag.erase(0, 1);
		}
		else
		{
			break;
		}
	}
	
	return tag;
}

wstring LIST::ListTag(vector<wstring> tagList)////////////////////////
{
	wstring tagString;
	if (tagList.size() > 1)
	{
		for (int i = 1; i < tagList.size(); i++)
		{
			tagString += tagList[i] + L", ";
		}
	}
	return tagString;

}

int LIST::saveListNote(vector<wstring> tags, wstring content, int currNoteId = -1)///////////////////////////
{
	//save tags
	bool b = false;
	for (int j = 0; j < tags.size(); j++)
	{
		b = false;
		for (int i = 0; i < TagList.size(); i++)
		{
			if (tags[j] == TagList[i].TagName)
			{
				if (currNoteId == -1)
				{
					TagList[i].Id.push_back(NoteList.size());
				}
				else
				{
					bool exist = false;
					for (int i = 0; i < TagList[i].Id.size(); i++)
					{
						if (currNoteId == TagList[i].Id[i])
						{
							exist = true;
							break;
						}
					}
					if (exist == false)
					{
						TagList[i].Id.push_back(currNoteId);
					}
				}
				b = true;
				break;
			}
		}

		if (b == false)
		{
			if (currNoteId != -1)
			{
				TAG newTag(tags[j], currNoteId);
				TagList.push_back(newTag);

			}
			else
			{
				TAG newTag(tags[j], NoteList.size());
				TagList.push_back(newTag);

			}
		}
	}

	
	if (currNoteId == -1)
	{
		NOTE newNote(NoteList.size(), tags, content);
		NoteList.push_back(newNote);

	}
	else
	{
		NOTE newNote(currNoteId, tags, content);
		NoteList[currNoteId] = newNote;
	}
	return 0;
}

int LIST::loadFile()///////////////////////////////////////
{
	wfstream file;
	file.open("Tags.txt");
	if (!file.is_open())
	{
		return 1;
	}
	file.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));

	wstring nameTag, id, strList;
	wstringstream  idList;
	while (getline(file, nameTag))
	{
		getline(file, strList);
		std::wstringstream().swap(idList);
		idList << strList;

		TAG currTag(nameTag);
		while (getline(idList, id, L' '))
		{
			currTag.Id.push_back(wcstod(id.c_str(), NULL));
		}

		TagList.push_back(currTag);
	}
	file.close();

	wfstream Note;

	HANDLE hFind;
	WIN32_FIND_DATA data;

	wstringstream path, temp;
	wstring strTag, strTemp;
	path <<"DataNote/" << "*.txt";
	hFind = FindFirstFile(path.str().c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do
		{
			wstringstream().swap(path);
			path << "DataNote/" << data.cFileName;
			Note.open(path.str());
			if (!Note.is_open())
			{
				return 2;
			}
			Note.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
			getline(Note, strTag);

			wstringstream().swap(temp);
			while (getline(Note, strTemp))
			{
				temp << strTemp;
			}

			id = data.cFileName;
			id[id.size() - 4] = '\0';
			NOTE currNote(wcstol(id.c_str(), NULL, 10), addTag(strTag), temp.str());
			NoteList.push_back(currNote);
			Note.close();
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	return 0;
}

int LIST::saveFile()////////////////////////////////
{
	wofstream Tags;
	wofstream Notes;
	Tags.open("Tags.txt");

	if (!Tags.is_open())
	{
		return 1;
	}

	Tags.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
	for (int i = 0; i < TagList.size(); i++)
	{
		Tags << TagList[i].TagName << endl;
		for (int j = 0; j < TagList[i].Id.size(); j++)
		{
			Tags << TagList[i].Id[j] << " ";
		}
		Tags << endl;
	}
	Tags.close();

	//write notes
	for (int i = 0; i < NoteList.size(); i++)
	{
		wstringstream s;
		s << "DataNote/" << NoteList[i].Id << ".txt";
		Notes.open(s.str());
		if (!Notes.is_open())
		{
			return 2;
		}
		Notes.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));

		for (int j = 0; j < NoteList[i].Tag.size(); j++)
		{
			Notes << NoteList[i].Tag[j] << ",";
		}
		Notes << endl;
		Notes << NoteList[i].Content;
		Notes.close();
	}

	return 0;
}

