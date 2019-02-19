#ifndef TDXTRADE_API_H
#define TDXTRADE_API_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>

using json = nlohmann::json;

typedef enum {
	EN_SZ = 0,
	EN_SH = 1,
	EN_UNKONW = 2
}AccountType;

typedef struct
{
	AccountType enType;			// 账号类别
	std::string strCode;	// 股东代码
}AccountInfo;



class TdxTradeApi
{
public:
	TdxTradeApi(const char *url);
	~TdxTradeApi();

	//static int logon();
	//static int logoff(int clientId);
	bool logon(const char* ip,
		int port,
		const char* version,
		int yyb_id,
		const char* account_id,
		const char* trade_account,
		const char* jy_password,
		const char* tx_password);

	bool logoff();

	/*
	"""
	0 资金
	1 股份
	2 当日委托
	3 当日成交
	4 可撤单
	5 股东代码
	6 融资余额
	7 融券余额
	8 可融证券
	9
	10
	11
	12 可申购新股查询
	13 新股申购额度查询
	14 配号查询
	15 中签查询
	"""
	*/

	json query_data(int category);
	bool query_gddm(std::string& sh, std::string& sz);

	/**
	 @param category: 0买入1卖出2融资买入3融券卖出4买券还券5卖券还款6现券还券7担保品买入8担保品卖出
	 @param price_type: 0限价委托；上海限价委托/深圳限价委托
						1市价委托(深圳对方最优价格)
						2市价委托(深圳本方最优价格)
						3市价委托(深圳即时成交剩余撤销)
						4市价委托(上海五档即成剩撤/深圳五档即成剩撤)
						5市价委托(深圳全额成交或撤销)6市价委托(上海五档即成转限价)
	@param gddm:	股东代码；交易上海股票填上海的股东代码，交易深圳的股票填入深圳的股东代码
	@param zqdm:	证券代码
	@param price:	委托价格
	@param quantity:委托数量

	*/
	json send_order(int category, int price_type, const char* gddm, const char* zqdm, float price, int quantity);
	
	bool send_order_ipo(const char* zqdm, float price, std::string &errInfo);



private:
	json call(const char *func, json params);
	std::string m_url;

	int m_client_id;
	std::string m_shgddm;
	std::string m_szgddm;
	std::map<AccountType, std::string> m_accInfos;

//	std::vector<AccountInfo> m_accInfos;

};


#endif
