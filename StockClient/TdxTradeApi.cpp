#include "TdxTradeApi.h"
#include <cpr/api.h>
#include "conv_utils.h"
#include <iostream>

using namespace std;

#define ERR_IPO_1		120156	// ���ί�����������ί�������Ϸ���У��ʧ��
#define ERR_IPO_2		251146	// ί���������ڿ��깺����
#define ERR_IPO_3		250235	// ֤ȯ�˻����Ʊ��¼������
#define ERR_IPO_4		120155	// ί�������������0, ����������λ�����ܳ���9λ, �Ϻ�ί���������ܳ���8λ

TdxTradeApi::TdxTradeApi(const char *url)
{
	m_url = url;
	m_client_id = -1;
}
TdxTradeApi::~TdxTradeApi()
{

}

bool TdxTradeApi::logon(const char * ip, int port, const char * version, int yyb_id, const char * account_id, const char * trade_account, const char * jy_password, const char * tx_password)
{
	json param = {
		{"ip", ip},
		{"port", port},
		{"version", version},
		{"yyb_id", yyb_id },
		{"account_no", account_id },
		{"trade_account", trade_account },
		{"jy_password", jy_password },
		{"tx_password", tx_password }
	};

	json resJson = call("logon", param);
	if (resJson["success"].get<bool>() == true)
	{
		m_client_id = resJson["data"]["client_id"].get<int>();
		string sh, sz;
		bool bRet = query_gddm(sh, sz);
		return bRet;
	}
	return false;
}

bool TdxTradeApi::logoff()
{
	if (m_client_id != -1)
	{
		json param;
		param["client_id"] = m_client_id;
		json resJson = call("logoff", param);
		if (resJson["success"].get<bool>() == true)
		{
			m_client_id = -1;
			return true;
		}
		return false;
	}
	return true;

}

json TdxTradeApi::query_data(int category)
{
	json param = {
		{"client_id", m_client_id},
		{"category", category }
	};
	return call("query_data", param);
}

bool TdxTradeApi::query_gddm(std::string & sh, std::string & sz)
{
	json res = query_data(5);
	if (res["success"].get<bool>() == true)
	{
		auto accData = res["data"];
		for (int i = 0; i < accData.size(); i++)
		{
			AccountInfo info;
			string gddm = gbk_to_utf8("�ɶ�����");
			string zhlb = gbk_to_utf8("�ʺ����");
			info.strCode = accData.at(i)[gddm];
			string strType = accData.at(i)[zhlb];
			if (strType.compare("0") == 0)
			{
				

				info.enType = EN_SZ;
				sz = info.strCode;
			}
			else if (strType.compare("1") == 0)
			{
				info.enType = EN_SH;
				sh = info.strCode;
			}
			else
			{
				info.enType = EN_UNKONW;
			}
			m_accInfos.insert(pair<AccountType, string>(info.enType, info.strCode));
//			m_accInfos.push_back(info);
		}
		return true;
	}
	return false;
}

json TdxTradeApi::send_order(int category, int price_type, const char * gddm, const char * zqdm, float price, int quantity)
{
	json param = {
		{ "client_id", m_client_id },
		{ "category", category },
		{"price_type", price_type },
		{"gddm", gddm },
		{"zqdm", zqdm },
		{"price", price },
		{"quantity", quantity}
	};
	return call("send_order", param);
}



bool TdxTradeApi::send_order_ipo(const char * zqdm, float price, std::string &errInfo)
{
	// ��ѯ�깺��Ʊ���˻�
	AccountType accType;
	
	// �����깺��¼
	int amountMax = 0;
	int amount = 0;

	if (zqdm != NULL)
	{
		if (zqdm[0] == 0x37)
		{
			accType = EN_SH;
		}
		else
		{
			accType = EN_SZ;
		}
	}
	// ��ѯ��ǰ֤ȯ�����깺��¼
	json res = send_order(0, 0, m_accInfos[accType].c_str(), zqdm, price, 10000000);
	if (res["success"].get<bool>() == false)
	{
		string strRes = res["error"].dump();
		size_t s = strRes.find("p_high_amount");
		if (s != string::npos)
		{
			size_t e = strRes.find(',', s);
			string str = strRes.substr(s, e - s);
			size_t amount_pos = str.find('=');
			string strAmount = str.substr(amount_pos + 1);
			amountMax = atoi(strAmount.c_str());
		}
		else
		{
			errInfo = utf8_to_gbk(strRes);
			return false;
		}
	}

	// ��ѯ�˻����깺���
	res = send_order(0, 0, m_accInfos[accType].c_str(), zqdm, price, amountMax);
	if (res["success"].get<bool>() == false)
	{
		string strRes = res["error"].dump();
		size_t s = strRes.find("v_enable_amount");
		if (s != string::npos)
		{
			size_t e = strRes.find(',', s);
			string str = strRes.substr(s, e - s);
			size_t amount_pos = str.find('=');
			string strAmount = str.substr(amount_pos + 1);
			amount = atoi(strAmount.c_str());
		}
		else
		{
			errInfo = utf8_to_gbk(strRes);
			return false;
		}
	}

	if (amount == 0)
	{
		errInfo = "��Ȳ���";
		return false;
	}

	// �깺
	res = send_order(0, 0, m_accInfos[accType].c_str(), zqdm, price, amount);
	if (res["success"].get<bool>() == false)
	{
		errInfo = utf8_to_gbk(res.dump());
		std::cout << errInfo << std::endl;
		return false;
	}
	else
	{
		return true;
	}

}





json TdxTradeApi::call(const char * func, json params)
{
	json json_obj;
	json_obj["func"] = func;
	if (!params.is_null())
		json_obj["params"] = params;

	auto rr = cpr::Post(cpr::Url{ m_url },
		cpr::Body{ json_obj.dump() },
		cpr::Header{ { "Content-Type", "text/plain" } });

	return json::parse(rr.text);
}