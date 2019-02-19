#pragma once


#include <assert.h>
#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
using namespace std;
typedef unsigned long   u32;

class OperateCSV
{
private:
	map<u32, map<u32, string>> m_stringMap;
	string m_CSVName;
public:
	OperateCSV() {}
	OperateCSV(const char *path)
	{
		assert(LoadCSV(path));
	}
	~OperateCSV() {}

	bool LoadCSV(const char *path);
	bool SaveCSV(const char *path = NULL);

	bool GetIntValue(u32 uiRow, u32 uiCol, int &riValue);
	bool GetFloatValue(u32 uiRow, u32 uiCol, float &rfValue);
	string* GetStringValue(u32 uiRow, u32 uiCol);

	int GetParamFromString(string str, vector<string> &stringVec, char delim = ',');//根据逗号，进行字符串的分隔

	map<u32, map<u32, string>>& GetCSVMap()
	{
		return m_stringMap;
	}
};


bool OperateCSV::LoadCSV(const char *path)
{
	FILE *pFile = fopen(path, "r");

	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);//重定位流上的文件指针.其作用是将位置指针移到离文件头0个字节处，即放在文件开头
		u32 uSize = ftell(pFile);//用来获取文件的当前读写位置,返回值是当前读写位置偏离文件头部的字节数.用以衡量数据量
		rewind(pFile);//使文件pFile的位置指针指向文件开始。

		char *fileBuffer = new char[uSize];//
		fread(fileBuffer, 1, uSize, pFile);//用来读入一个数据块

		map<u32, string> stringMap;
		u32 uiIndex = 1;

		char *pBegin = fileBuffer;//字符串的首地址
		char *pEnd = strchr(pBegin, '\n');//在一个字符串中查找一个特定的字符


		pBegin = pEnd + 1;//第二行的起始，舍弃了第一行
		pEnd = strchr(pBegin, '\n');

		while (pEnd)
		{
			string strTemp;
			strTemp.insert(0, pBegin, pEnd - pBegin);
			assert(!strTemp.empty());
			stringMap[uiIndex++] = strTemp;//将每一行的字符串存到map中，key为顺序号，value为字符串
			pBegin = pEnd + 1;
			pEnd = strchr(pBegin, '\n');//寻找下一行
		}
		delete[]fileBuffer;
		fileBuffer = NULL;
		pBegin = NULL;
		pEnd = NULL;

		map<u32, string>::iterator iter = stringMap.begin();//遍历每一行字符串
		for (; iter != stringMap.end(); ++iter)
		{
			vector<string> stringVec;//存储按照逗号分隔好的数据
			map<u32, string> stringMapTemp;
			assert(GetParamFromString(iter->second, stringVec) > 0);

			vector<string>::size_type idx = 0;
			for (; idx != stringVec.size(); ++idx)
			{
				stringMapTemp[idx + 1] = stringVec[idx];//
			}

			m_stringMap[iter->first] = stringMapTemp;//每一行的存储情况
		}

		fclose(pFile);
		m_CSVName = path;
		return true;
	}
	else
	{
		return false;
	}
}

bool OperateCSV::SaveCSV(const char *path /* = NULL */)
{
	if (path != NULL)
	{
		m_CSVName = path;
	}

	FILE *pFile = fopen(m_CSVName.c_str(), "w");
	if (pFile)
	{
		map<u32, map<u32, string>>::iterator iter = m_stringMap.begin();
		for (; iter != m_stringMap.end(); ++iter)
		{
			map<u32, string> &rStringMap = iter->second;
			map<u32, string>::iterator it = rStringMap.begin();
			for (; it != rStringMap.end(); ++it)
			{
				string strTemp = it->second;
				strTemp += ',';
				fwrite(strTemp.c_str(), 1, 1, pFile);
			}

			char delim = '\n';
			fwrite(&delim, 1, 1, pFile);
		}

		fclose(pFile);
		return true;
	}
	else
	{
		return false;
	}
}

bool OperateCSV::GetIntValue(u32 uiRow, u32 uiCol, int &riValue)
{
	string *pStr = GetStringValue(uiRow, uiCol);
	if (pStr)
	{
		riValue = atoi(pStr->c_str());
		return true;
	}
	else
	{
		return false;
	}
}

bool OperateCSV::GetFloatValue(u32 uiRow, u32 uiCol, float &rfValue)
{
	string *pStr = GetStringValue(uiRow, uiCol);
	if (pStr)
	{
		rfValue = atof(pStr->c_str());
		return true;
	}
	else
	{
		return false;
	}
}

string* OperateCSV::GetStringValue(u32 uiRow, u32 uiCol)
{
	map<u32, map<u32, string>>::iterator iter = m_stringMap.find(uiRow);//因为刚好行号为关键字，所以可以用find进行查找
	if (iter != m_stringMap.end())
	{
		map<u32, string> &rStrMap = iter->second;
		map<u32, string>::iterator it = rStrMap.find(uiCol);
		if (it != rStrMap.end())
		{
			return &(it->second);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

//用于分割字符串，将CSV表格中的一行按照规则解析成一组字符串，存储在一个vector中
//根据CSV表格中所存储的数据的不同，重载各函数
int OperateCSV::GetParamFromString(string str, vector<string> &stringVec, char delim)
{
	char *token = strtok(const_cast<char *>(str.c_str()), &delim);
	while (token)
	{
		string strTemp = token;
		stringVec.push_back(strTemp);
		token = strtok(NULL, &delim);
	}

	return stringVec.size();
}