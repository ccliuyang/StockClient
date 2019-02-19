#include <iostream>
#include <cpr/api.h>
#include <nlohmann/json.hpp>
#include <vector>
#include "conv_utils.h"

#include "TdxTradeApi.h"


using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Crypt32.lib")


const char *url = "http://m.data.eastmoney.com/XG/XgrlList";

const std::string getCurrentSystemTime()
{
	auto tt = std::chrono::system_clock::to_time_t
	(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
		(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
	return std::string(date);
}
const std::string getCurrentToday()
{
	auto tt = std::chrono::system_clock::to_time_t
	(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday);
	return std::string(date);

}

bool isToday(string strData)
{
	auto tt = std::chrono::system_clock::to_time_t
	(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday);

	if (strData.compare(date) == 0)
		return true;
	return false;
}


const char *cal_url = "http://file.tushare.org/tsdata/calAll.csv";


class OperateCalCSV
{
public:
	OperateCalCSV(const char* csv) {}
	~OperateCalCSV() {}
	bool LoadCalCSV(const char *buf)
	{
		const char* pBegin = buf;
		const char* pEnd = strchr(pBegin, '\n');
		pBegin = pEnd + 1;
		pEnd = strchr(pBegin, '\n');

		while (pEnd)
		{
			string strTemp;
			strTemp.insert(0, pBegin, pEnd - pBegin);
			char delim = ',';
			vector<string> stringVec;//存储按照逗号分隔好的数据
			GetParamFromString(strTemp, stringVec, ',');
			m_calMap[stringVec.at(0)] = atoi(stringVec.at(1).c_str());


			pBegin = pEnd + 1;
			pEnd = strchr(pBegin, '\n');//寻找下一行

		}
		return true;
	};

	int GetParamFromString(string str, vector<string> &stringVec, char delim)
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

	bool isOpen(std::string date)
	{
		return m_calMap[date];
	};

	std::map<std::string, bool> m_calMap;

};


int main(int argc, char *argv[])
{
	//auto res = cpr::Get(cpr::Url(cal_url));
	//OperateCalCSV calc(0);
	//calc.LoadCalCSV(res.text.c_str());
	//if (!calc.isOpen(getCurrentToday()))
	//	return 0;


	TdxTradeApi api("http://127.0.0.1:19820/api");
	bool logRes = api.logon("61.139.133.60", 7708, "6.49", 32, "00000000", "00000000", "000000", "");
	std::cout << "登陆成功:" << logRes << std::endl;

	std::cout << "获取新股申购列表" << std::endl;

	auto rr = cpr::Get(cpr::Url{ url });
	json requestJson = json::parse(rr.text);
	auto stocks = requestJson["result"]["Calendarlist"];
	for (int i = 0; i < stocks.size(); i++)
	{
//		std::cout << utf8_to_gbk(stocks.at(i)["date"].dump()) << ":" << std::endl;
		//for (int j = 0; j < stocks.at(i)["sglist"].size(); j++)
		//{
		//	std::cout << utf8_to_gbk(stocks.at(i)["sglist"].at(j).dump()) << std::endl;
		//}

		if (isToday(stocks.at(i)["date"].get<string>()))
		{
			for (int j = 0; j < stocks.at(i)["sglist"].size(); j++)
			{
				string stockCode = stocks.at(i)["sglist"].at(j)["SubCode"].get<string>();				
				float price = stof(stocks.at(i)["sglist"].at(j)["fxj"].get<string>());
				std::cout << "申购新股:" << stockCode << " " << "价格:" << price << std::endl;
				string errInfo;

				bool bRet = api.send_order_ipo(stockCode.c_str(), price, errInfo);
				if (bRet)
				{
					std::cout << "申购新股成功" << std::endl;
				}
				else
				{
					std::cout << "申购新股失败:" << errInfo << std::endl;
				}
				
			}
		}

		
	}

	logRes = api.logoff();
	std::cout <<"退出登陆" <<  logRes << std::endl;
	system("PAUSE");
	return 0;
}