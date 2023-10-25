//TS�ļ� === TS��+TS��+TS��
//TS �� Header

#include<fstream>
#include<iostream>
#include <io.h>

#include<stdio.h>
#include<string>
#include<atlstr.h>

#include<map>

using namespace std;

bool isDebug = false;

struct TsHeader {
	unsigned char sync_byte; //��һ�ֽ� ͬ���ֽڣ��̶�0x47   
	bool transport_error_indicator; //�ڶ��ֽ� ��һλ ��λ---��λ ˳�� ��������ʶ �̶� 0
	bool payload_unit_start_indircation; //�ڶ����ֽ� �ڶ�λ 
	//���ص�Ԫ��ʼ��ʶ ������PES����ʱ
	//1 ��ʾTS������Ч�غ���PES���ĵ�һ�ֽڿ�ʼ��TS��header֮��Ҫ����һ�������ֽڣ�
	//0 ��ʶ������PES�����ǵ�һ����ʼ��
	//������PSI����ʱ ��Ϊ1����ʾTS������PSI�ĵ�һ�ֽڣ�����һ�ֽڴ���ָ��pointer_filed;��Ϊ0����ʾTS��������PSI���ֵĵ�һ�ֽڣ�����Ч�غ�ָ��û��pointer_filed.���ڿհ�����Ӧ����Ϊ0.
	bool transport_proirity; //�ڶ��ֽ� ����λ �������ȼ� �̶� 0
	unsigned short pid; //�ڶ��ֽں���λ+�����ֽ�   idΪ0 �����������ʱPAT��  idΪ1 �����������ΪCAT�� 2 Ϊ������������ 0XFFFF ��ʾ�հ�
	unsigned char transport_scrambling_control; //�����ֽ�ǰ��λ
	unsigned char adaptation_filed_control; //�����ֽ�3,4λ
	unsigned char continuity_counter; //�����ֽ� ����λ
};
struct PATHeader{
	unsigned char table_id; //��һ���ֽ� ��id
	unsigned char section_synatx_indication; //�ڶ��ֽ� ��һλ Ĭ��1
	unsigned char zero;                      //�ڶ��ֽ� �ڶ�λ �ߵ��� ����1λ Ĭ�� 0
	unsigned char reserved_1;                  //�ڶ��ֽ� ������λ 2λ
	unsigned short section_length;           //�ڶ��ֽ� ����λ + �����ֽ� 12λ �������ֽ����ڵ�crc_32λ����ռ���ֽ���
	unsigned short transport_stream_id;      //�������ֽ� ��16λ ���ֽڳ䵱��ǩ����ʶ�����ڴ˴������б����κ�����·����������ֵ���û��涨��
	unsigned char reserved_2;					 //�����ֽ� ����λ ��λ 11
	unsigned char version_number;			 //�����ֽ� ��5λ PAT�汾�ţ��汾���б����һ
	unsigned char current_next_indicator;	 //�����ֽ� ���һλ ��ʶ��Ĭ��1 Ϊ0��ʾ�ö��޷��ã���һ����ֶβſ�����
	unsigned char section_number;			 //�����ֽ� �ֶκ� ������TS������PAT��ĵڼ����ֶΣ��ֶκŴ�0��ʼ
	unsigned char last_section_number;		 //�ڰ��ֽ� ���һ���ֶκ� ���ֶκ�
	//ѭ����ȡ�������
	struct PATProgramInfo* program_info;
	int programNum;//ѭ������
	//ѭ������
	unsigned int crc_32;					 //CRCУ�� ����غ����ֽ�
};
struct PATProgramInfo
{
	unsigned short program_number;			 //ѭ����ʼ  1-2�ֽ� ��Ŀ���
	unsigned char reserved_n;					 //3�ֽ�ǰ��λ ����ֵ
	unsigned short network_PID;				 //NIT���PIDֵ 13λ 3-4�ֽ� ��Ŀ��Ϊ0�����
	unsigned short program_map_PID;			 //PMT��IDֵ  13λ 3-4�ֽ�  ѭ��β�� ������ռ�����ֽڣ�����ѭ�������ķ�����(section_length-6-4�ֽ�)/4
};
//eg: 47 40 00 11 00 00 B0 0D 00 01 C1 00 00 00 01 F0 00 2A B1 04 B2
// 47 40 00 11 ǰ���ֽ�
// ������payload_unit_start_indircation Ϊ�ڶ����ֽڵڶ�λ �ڶ����ֽ� 40 = 0100 0000 �ڶ�λΪ1����Ҫ����һ���ֽ�
// ������PIDΪ 0 0000�ڶ��ֽں���λ + �����ֽ� 00 PIDΪ0 �����������ΪPAT�� ����PAT������
// ����header֮��ĵ�һ���ֽ� 00 Ȼ�����PAT��
// 00 B0 0D 00 01 C1 00 00 00 01 F0 00 2A B1 04 B2
// table_id = 00
// B0  1011 0000
// ����λ section_synatx_indication Ϊ 1 ; ����λ zero Ϊ 0 ; ����λ reserved_1 Ϊ 11
// section_length = 0000 + 0D = 0000 0000 1101 = 13 ����֮����13���ֽڵ����� - 4�ֽ�(crc_32 ���ֽ�) - 5 (�ֽ�) = 4�ֽ� Ҳ����ѭ��һ��
// transport_stream_id = 00 01
// reserved_2 = C1 = 1100 0001 ��ǰ��λ������ 11
// version_number = C1 = 1100 0001 ��[3 - 7] λ 00000
// current_next_indicator = C1 = 1100 0001 ���һλ 1 ��ʾ�ֶο���
// section_number = 00 �ֶκ� 0 Ϊ0����ʼ��
// last_section_bumber = 00 �������ֶκ�Ϊ0 �˶�Ϊ������
// N = 0 program_number = 00 01 = 1 ��ʾ �����IDΪprogram_map_PID ָ��PMT���ID ������� 0 ��ʾ�����IDΪnetwork_ID 
// N = 0 reserved_n = F0 = 1111 0000 ��ǰ���ֽ� = 111
// N = 0 network_PID|program_map_PID = F0 �����ֽ� + 00 = 1 0000 0000 0000 = 2��12�η� 1024*2*2 = 4096
// crc_32 = 2A B1 04 B2
struct PMTHeader{
	unsigned char table_id; //��һ���ֽ� ��id
	unsigned char section_synatx_indication; //�ڶ��ֽ� ��һλ Ĭ��1
	unsigned char zero;                      //�ڶ��ֽ� �ڶ�λ �ߵ��� ����1λ Ĭ�� 0
	unsigned char reserved_1;                //�ڶ��ֽ� ������λ 2λ
	unsigned short section_length;           //�ڶ��ֽ� ����λ + �����ֽ� 12λ �������ֽ����ڵ�crc_32λ����ռ���ֽ���
	unsigned short program_number;			 //�������ֽ� ��ǰPMT��ӦPAT���program_number 
	unsigned char reserved_2;                //�����ֽ� ����λ ��λ 11
	unsigned char version_number;			 //�����ֽ� ��5λ PAT�汾�ţ��汾���б����һ
	unsigned char current_next_indicator;	 //�����ֽ� ���һλ ��ʶ��Ĭ��1 Ϊ0��ʾ�ö��޷��ã���һ����ֶβſ�����
	unsigned char section_number;			 //�����ֽ� �ֶκ� ������TS������PAT��ĵڼ����ֶΣ��ֶκŴ�0��ʼ
	unsigned char last_section_number;		 //�ڰ��ֽ� ���һ���ֶκ� ���ֶκ�
	unsigned char reserved_3;				 //�ھ����ֽ� ǰ��λ ����λ
	unsigned short PCR_PID;					 //�ھ�ʮ�ֽ� ��13λ ��ʾΪTS����PIDֵ 
	unsigned char reserved_4;				 //��ʮһ�ֽ� ǰ��λ ����λ
	unsigned short program_info_length;		 //��ʮһ����λ + ��ʮ���ֽ� 
	//ѭ����ʼ
	unsigned char stream_type;               //��N��ѭ���� 1�ֽ� �������ı����ʽ ���Ϊ0x1B ����ΪH256��ʽ ΪPES��
	//��MPEG1��Ƶ��0x02��MPEG2��Ƶ��0x03:MPEG1��Ƶ��0x04��MPEG2��Ƶ��0x05��˽���ֶΣ�0x06������˽�����ݵ�PES�� ...
	unsigned char reserved_5;					 //��N��ѭ���� 2�ֽ�ǰ��λ ����λ
	unsigned short elementary_pid;			 //��N��ѭ���� 2�ֽں���λ + 3�ֽ� ������PES����PIDֵ
	unsigned char reserved_6;					 //��N��ѭ���� 4�ֽ�ǰ��λ ����λ
	unsigned short es_info_length;			 //��N��ѭ���� 4�ֽں���λ + 5�ֽ� ��������ES������������ֽ���
	//ѭ������
	unsigned int crc_32;					 //CRCУ�� ���ֽ�
};
// 47 50 00 11 TSͷ
// 47 ͬ���ֽ� 
// 50 ǰ��λ���н���
// 50 ����λ + 00 =  1 0000 0000 0000 = 2��12�� = 4096 ��Ӧ�����PAT�����������program_map_ID
// 11 �ݲ�����
// ��һ������Ϊ
// 00 ����λ
// ��ЧPMT������ 02 B0 17  00 01  C1  00 00  E1 00  F0 00  1B E1 00 F0 00  0F E1 01 F0 00  2F 44 B9 9B
// table_id = 02
// B0 17 ���·��� ����λ 1 ����λ 0 ����λ 11 ��crc_32�����ֽ��� 000000010111 ���ֽ���Ϊ 23
// 00 01 ��ӦPAT���е�program_number = 1
// C1 ���·��� ����λ 11 PAT�汾�� 00000 ��ʾ�ֶο��� 1 
// 00 00 �ֱ��ʾ�������С�ֶκ�
// E1 00 ���·��� ����λ 111 TS����PIDֵ 0000100000000 = 256��PCRֵ
// F0 00 ���·��� ����λ 1111 ��󳤶� 000000000000 ��Ŀ��Ϣ��������С
// ѭ������ = 23 - 9 - 4 = 10�ֽ� / 2 = ��ѭ��
// ��һ�� stream_type = 1B H256��ʽ��Ƶ����
// E1 00 ���·��� reserved = 111 ����λ elementary_pid = 0000100000000 = 256 �������������ݵİ���PID = 256
// F0 00 ���·��� reserved = 1111 ����λ 000000000000 = es_info_length ��������ES������������ֽ��� = 0 ����������0�ֽ�
// �ڶ��� stream_type = 0F ������ACC��Ƶ����
// E1 01 ���·��� reserved = 111 ����λ elementary_pid = 0000100000001 = 257 �������������ݵİ���PID = 257
// F0 00 ���·��� reserved = 1111 ����λ 000000000000 = es_info_length ��������������ֽ���λ = 0 ����������0�ֽ�
// 2F 44 B9 9B crc_32У��
struct TsPackageData
{
	unsigned char data[188];
};
struct TsPackage
{
	TsHeader headr;
	PATHeader pat;
	PMTHeader pmt;
	unsigned char* data;
};
TsPackage decodePackage(unsigned char *data) {
	TsPackage tsPackage;

}

unsigned char byteToChar(unsigned char *& byte, int position, int size) {
	if (isDebug)
	{
		std::cout << "byte = " << (int)*byte << " , position = " << position << " , size = " << size << std::endl;
	}
	unsigned char result = 0;
	unsigned char a = *byte;
	int left = position - 1;
	if (isDebug)
	{
		std::cout << "���� left = " << left << std::endl;
	}
	a = a << left;
	if (position - 1 + size <= 8)
	{
		a = a >> (8 - size);
		size = 0;
	}
	else {
		a = a >> left;
		size = size - (8 - position + 1);
	}
	result = a;
	if (size <= 0)
	{
		if (isDebug)
		{
			std::cout << "size = " << size << " , result = " << (int)result << std::endl;
		}
		return result;
	}
	result = result << size;
	byte++;
	a = *byte;
	int right = 8 - size;
	a = a >> right;
	result = result + a;
	return result;
}
unsigned short byteToShort(unsigned char * &byte, int position, int size) {
	if (size < 8)
	{
		return byteToChar(byte, position, size);
	}
	unsigned short result = 0;
	while (size > 0)
	{
		if (position != 0 && (position - 1) + size >= 8)
		{
			if (isDebug)
			{
				std::cout << "position = " << position << " , size = " << size << std::endl;
			}
			unsigned char a = *byte;
			int left = position - 1;
			a = a << left;
			a = a >> left;
			result = a;
			size = size - (8 - position + 1);
			position = 0;
			byte++;
		}
		else if (position == 0 && size >= 8)
		{
			if (isDebug)
			{
				std::cout << "position = " << position << " , size = " << size << std::endl;
			}
			result = result << 8;
			unsigned char a = *byte;
			result = result + a;
			byte++;
			size = size - 8;
		}
		else if (size < 8) {
			if (isDebug)
			{
				std::cout << "position = " << position << " , size < 8 " << std::endl;
			}
			result = result << size;
			unsigned char a = *byte;
			a = a >> (8 - size);
			result = result + a;
			size = 0;
		}
		else {
			if (isDebug){
				std::cout << "else " << std::endl;
			}
		}
	}
	return result;
}
/*
position Ϊ�ֽ���ʼλ 1-8  8�������һλ
size ��Ҫ����16
*/
unsigned int byteToInt(unsigned char * &byte,int position,int size) {
	if (size < 16)
	{
		return byteToShort(byte, position, size);
	}
	unsigned int result = 0;

	while (size > 0)
	{
		if (position != 0 && (position - 1) + size >= 8)
		{
			if (isDebug) {
				std::cout << "1position = " << position << " , sie = " << size << std::endl;
			}
			unsigned char a = *byte;
			int left = position - 1;
			a = a << left;
			a = a >> left;
			result = a;
			size = size - (position - 1);
			position = 0;
			byte++;
			if (isDebug) {
				std::cout << "size = " << size << ",result = " << result << std::endl;
			}
		}
		else if (position == 0 && size >= 8){
			if (isDebug) {
				std::cout << "3position = " << position << " , sie = " << size << std::endl;
			}
			result = result << 8;
			unsigned char a = *byte;
			result = result + a;
			byte++;
			size = size - 8;
			if (isDebug) {
				std::cout << "size = " << size << ",result = " << result << std::endl;
			}
		}
		else if (position == 0 && size < 8)
		{
			if (isDebug) {
				std::cout << "2position = " << position << " , sie = " << size << std::endl;
			}
			result = result << size;
			unsigned char a = *byte;
			a = a >> (8 - size);
			result = result + a;
			size = 0;
			if (isDebug) {
				std::cout << "size = " << size << ",result = " << result << std::endl;
			}
		}
		else {
			if (isDebug) {
				std::cout << "4position = " << position << " , sie = " << size << std::endl;
			}
		}
	}
	return result;
}

void detectTsPackageTsHeader(TsPackage *tsPackage) {
	unsigned char *data = tsPackage->data;
	//����TsHeader
	TsHeader header;
	header.sync_byte = *data;
	data++;
	header.transport_error_indicator = byteToChar(data, 1, 1);
	header.payload_unit_start_indircation = byteToChar(data, 2, 1);
	header.transport_proirity = byteToChar(data, 3, 1);
	header.pid = byteToShort(data, 4, 13);
	header.transport_scrambling_control = byteToChar(data, 1, 2);
	header.adaptation_filed_control = byteToChar(data, 3, 2);
	header.continuity_counter = byteToChar(data, 5, 4);
	data++;
	tsPackage->headr = header;
	tsPackage->data = data;
}
void detectTsPackageTsPAT(TsPackage *tsPackage) {
	unsigned char *data = tsPackage->data;
	PATHeader patHeader;
	patHeader.table_id = *data;
	if (patHeader.table_id != 0)
	{
		std::cout << "PAT decode fail tableId != 0. table_id = " << (int)patHeader.table_id << std::endl;
		return;
	}
	data++;
	patHeader.section_synatx_indication = byteToChar(data, 1, 1);
	std::cout << "section_synatx_indication = " << (int)patHeader.section_synatx_indication << "." << std::endl;
	patHeader.zero = byteToChar(data, 2, 1);
	std::cout << "zero = " << (int)patHeader.zero << "." << std::endl;
	patHeader.reserved_1 = byteToChar(data, 3, 2);
	std::cout << "reserved_1 = " << (int)patHeader.reserved_1 << "." << std::endl;
	patHeader.section_length = byteToShort(data, 5, 12);
	std::cout << "section_length = " << (int)patHeader.section_length << "." << std::endl;
	patHeader.transport_stream_id = byteToShort(data, 1, 16);
	patHeader.reserved_2 = byteToChar(data, 1, 2);
	patHeader.version_number = byteToChar(data, 3, 5);
	patHeader.current_next_indicator = byteToChar(data, 7, 1);
	data++;
	patHeader.section_number = *data;
	data++;
	patHeader.last_section_number = *data;
	data++;
	int num = (patHeader.section_length - 5 - 4) / 4;
	std::cout << "��ǰPAT������ " << num << " ��PMT��" << std::endl;
	patHeader.program_info = new PATProgramInfo[num];
	patHeader.programNum = num;
	for (int i = 0; i < num; i++)
	{
		unsigned short programNumber = byteToShort(data, 1, 16);
		patHeader.program_info->program_number = programNumber;
		patHeader.program_info->reserved_n = byteToChar(data, 1, 3);
		unsigned short nidOrPid = byteToShort(data, 4, 13);
		std::cout << "programNumber = " << programNumber << " , ��Ŀ�ţ� nidOrPid = " << nidOrPid << std::endl;
		if (programNumber == 0)
		{
			patHeader.program_info->network_PID = nidOrPid;
			patHeader.program_info->program_map_PID = -1;
		}
		else {
			patHeader.program_info->program_map_PID = nidOrPid;
			patHeader.program_info->network_PID = -1;
		}
	}
	patHeader.crc_32 = byteToInt(data, 1, 32);
	tsPackage->data = data;
}
void detectPackageTsPMT(TsPackage *package) {
	unsigned char *data = package->data;
}

int main_decode_ts() {
	std::string filename = "C:\\Users\\ThemSun\\Downloads\\0";
	FILE* m_TsFile_Fp = fopen(filename.c_str(), "rb");
	if (m_TsFile_Fp == NULL)
	{
		std::cout << "file open fail." << std::endl;
		return -1;
	}
	int fileSize = filelength(fileno(m_TsFile_Fp));
	std::cout << "ts File size = " << fileSize << std::endl;
	float tsPreCount = fileSize / 188.0;
	int tsCount = fileSize / 188;
	if (tsCount * 1.0 == tsPreCount)
	{
		std::cout << "ts File have " << tsCount << " ts package" << std::endl;
	}
	TsPackage *tsPackage = (TsPackage *)malloc(tsCount * sizeof(TsPackage));
	map<int, TsPackage*> packageMaps;
	for (int i = 0; i < tsCount; i++)
	{
		unsigned char *data = new unsigned char[188];
		fread(data, 1, 188, m_TsFile_Fp);
		std::cout << "first read byte = " << (int)*(data) << std::endl;
		tsPackage->data = data;
		detectTsPackageTsHeader(tsPackage);
		std::cout << "pid = " << tsPackage->headr.pid << std::endl;
		if (tsPackage->headr.payload_unit_start_indircation == 1)
		{
			std::cout << "tsHeader ����һ�������ֽڣ����������ֽ� =" << (int)*(tsPackage->data) << "." << std::endl;
			tsPackage->data++;
		}
		if (tsPackage->headr.pid == 0)
		{
			std::cout << "����PAT��" << std::endl;
			detectTsPackageTsPAT(tsPackage);
		}
		std::cout << std::endl;
		packageMaps[tsPackage->headr.pid] = tsPackage;
		tsPackage++;
	}
	fclose(m_TsFile_Fp);
	TsPackage *rootPackage = packageMaps[0];
	if (rootPackage == NULL)
	{
		cout << "root���δ�ҵ�" << endl;
		return -1;
	}
	rootPackage->pat.programNum;
	PATProgramInfo *info = rootPackage->pat.program_info;
	for (int index = 0; index < rootPackage->pat.programNum; index++)
	{
		bool isNid = info->program_number == 0;
		if (isNid)
		{
			
		}
		else {
			cout << "PMT pid = " << info->program_map_PID << endl;
			TsPackage *pmtPackage = packageMaps[info->program_map_PID];
			detectTsPackageTsPAT(pmtPackage);
		}
	}

	return 0;
}