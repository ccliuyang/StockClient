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
	AccountType enType;			// �˺����
	std::string strCode;	// �ɶ�����
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
	0 �ʽ�
	1 �ɷ�
	2 ����ί��
	3 ���ճɽ�
	4 �ɳ���
	5 �ɶ�����
	6 �������
	7 ��ȯ���
	8 ����֤ȯ
	9
	10
	11
	12 ���깺�¹ɲ�ѯ
	13 �¹��깺��Ȳ�ѯ
	14 ��Ų�ѯ
	15 ��ǩ��ѯ
	"""
	*/

	json query_data(int category);
	bool query_gddm(std::string& sh, std::string& sz);

	/**
	 @param category: 0����1����2��������3��ȯ����4��ȯ��ȯ5��ȯ����6��ȯ��ȯ7����Ʒ����8����Ʒ����
	 @param price_type: 0�޼�ί�У��Ϻ��޼�ί��/�����޼�ί��
						1�м�ί��(���ڶԷ����ż۸�)
						2�м�ί��(���ڱ������ż۸�)
						3�м�ί��(���ڼ�ʱ�ɽ�ʣ�೷��)
						4�м�ί��(�Ϻ��嵵����ʣ��/�����嵵����ʣ��)
						5�м�ί��(����ȫ��ɽ�����)6�м�ί��(�Ϻ��嵵����ת�޼�)
	@param gddm:	�ɶ����룻�����Ϻ���Ʊ���Ϻ��Ĺɶ����룬�������ڵĹ�Ʊ�������ڵĹɶ�����
	@param zqdm:	֤ȯ����
	@param price:	ί�м۸�
	@param quantity:ί������

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
