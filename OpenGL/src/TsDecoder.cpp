//TS文件 === TS包+TS包+TS包
//TS 包 Header

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
	unsigned char sync_byte; //第一字节 同步字节：固定0x47   
	bool transport_error_indicator; //第二字节 第一位 高位---低位 顺序 传输错误标识 固定 0
	bool payload_unit_start_indircation; //第二个字节 第二位 
	//负载单元开始标识 当包含PES数据时
	//1 表示TS包的有效载荷以PES包的第一字节开始（TS的header之后要跳过一个调整字节）
	//0 标识后续的PES包不是第一个开始包
	//当包含PSI数据时 置为1，表示TS包带有PSI的第一字节，即第一字节带有指针pointer_filed;置为0，表示TS包不带有PSI部分的第一字节，即有效载荷指针没有pointer_filed.对于空包，则应该置为0.
	bool transport_proirity; //第二字节 第三位 传输优先级 固定 0
	unsigned short pid; //第二字节后五位+第三字节   id为0 代表后面数据时PAT表  id为1 代表后面数据为CAT表 2 为程序流描述表 0XFFFF 表示空包
	unsigned char transport_scrambling_control; //第四字节前两位
	unsigned char adaptation_filed_control; //第四字节3,4位
	unsigned char continuity_counter; //第四字节 后四位
};
struct PATHeader{
	unsigned char table_id; //第一个字节 表id
	unsigned char section_synatx_indication; //第二字节 第一位 默认1
	unsigned char zero;                      //第二字节 第二位 高到低 保留1位 默认 0
	unsigned char reserved_1;                  //第二字节 第三四位 2位
	unsigned short section_length;           //第二字节 后四位 + 第三字节 12位 包含该字节在内到crc_32位置所占总字节数
	unsigned short transport_stream_id;      //第四五字节 共16位 该字节充当标签，标识网络内此传输流有别于任何其他路复用流。其值由用户规定。
	unsigned char reserved_2;					 //第六字节 保留位 两位 11
	unsigned char version_number;			 //第六字节 共5位 PAT版本号，版本号有变则加一
	unsigned char current_next_indicator;	 //第六字节 最后一位 标识：默认1 为0表示该段无法用，下一个表分段才可以用
	unsigned char section_number;			 //第七字节 分段号 表明该TS包属于PAT表的第几个分段，分段号从0开始
	unsigned char last_section_number;		 //第八字节 最后一个分段号 最大分段号
	//循环读取表格内容
	struct PATProgramInfo* program_info;
	int programNum;//循环次数
	//循环结束
	unsigned int crc_32;					 //CRC校验 最后载荷四字节
};
struct PATProgramInfo
{
	unsigned short program_number;			 //循环开始  1-2字节 节目编号
	unsigned char reserved_n;					 //3字节前三位 保留值
	unsigned short network_PID;				 //NIT表的PID值 13位 3-4字节 节目号为0用这个
	unsigned short program_map_PID;			 //PMT的ID值  13位 3-4字节  循环尾部 （整体占用四字节，计算循环次数的方法：(section_length-6-4字节)/4
};
//eg: 47 40 00 11 00 00 B0 0D 00 01 C1 00 00 00 01 F0 00 2A B1 04 B2
// 47 40 00 11 前四字节
// 解析到payload_unit_start_indircation 为第二个字节第二位 第二个字节 40 = 0100 0000 第二位为1代表要跳过一个字节
// 解析出PID为 0 0000第二字节后五位 + 第三字节 00 PID为0 代表后面数据为PAT表 解析PAT表内容
// 弃置header之后的第一个字节 00 然后解析PAT表
// 00 B0 0D 00 01 C1 00 00 00 01 F0 00 2A B1 04 B2
// table_id = 00
// B0  1011 0000
// 保留位 section_synatx_indication 为 1 ; 保留位 zero 为 0 ; 保留位 reserved_1 为 11
// section_length = 0000 + 0D = 0000 0000 1101 = 13 代表之后有13个字节的数据 - 4字节(crc_32 六字节) - 5 (字节) = 4字节 也就是循环一次
// transport_stream_id = 00 01
// reserved_2 = C1 = 1100 0001 的前两位二进制 11
// version_number = C1 = 1100 0001 的[3 - 7] 位 00000
// current_next_indicator = C1 = 1100 0001 最后一位 1 表示分段可用
// section_number = 00 分段号 0 为0代表开始段
// last_section_bumber = 00 代表最大分段号为0 此段为结束段
// N = 0 program_number = 00 01 = 1 表示 下面的ID为program_map_PID 指向PMT表的ID 如果等于 0 表示下面的ID为network_ID 
// N = 0 reserved_n = F0 = 1111 0000 的前三字节 = 111
// N = 0 network_PID|program_map_PID = F0 后五字节 + 00 = 1 0000 0000 0000 = 2的12次方 1024*2*2 = 4096
// crc_32 = 2A B1 04 B2
struct PMTHeader{
	unsigned char table_id; //第一个字节 表id
	unsigned char section_synatx_indication; //第二字节 第一位 默认1
	unsigned char zero;                      //第二字节 第二位 高到低 保留1位 默认 0
	unsigned char reserved_1;                //第二字节 第三四位 2位
	unsigned short section_length;           //第二字节 后四位 + 第三字节 12位 包含该字节在内到crc_32位置所占总字节数
	unsigned short program_number;			 //第四五字节 当前PMT对应PAT表的program_number 
	unsigned char reserved_2;                //第六字节 保留位 两位 11
	unsigned char version_number;			 //第六字节 共5位 PAT版本号，版本号有变则加一
	unsigned char current_next_indicator;	 //第六字节 最后一位 标识：默认1 为0表示该段无法用，下一个表分段才可以用
	unsigned char section_number;			 //第七字节 分段号 表明该TS包属于PAT表的第几个分段，分段号从0开始
	unsigned char last_section_number;		 //第八字节 最后一个分段号 最大分段号
	unsigned char reserved_3;				 //第九子字节 前三位 保留位
	unsigned short PCR_PID;					 //第九十字节 共13位 表示为TS包的PID值 
	unsigned char reserved_4;				 //第十一字节 前四位 保留位
	unsigned short program_info_length;		 //第十一后四位 + 第十二字节 
	//循环开始
	unsigned char stream_type;               //第N次循环的 1字节 表明流的编码格式 如果为0x1B 表明为H256格式 为PES流
	//：MPEG1视频，0x02：MPEG2视频，0x03:MPEG1音频，0x04：MPEG2音频，0x05：私有字段，0x06：含有私有数据的PES包 ...
	unsigned char reserved_5;					 //第N次循环的 2字节前三位 保留位
	unsigned short elementary_pid;			 //第N次循环的 2字节后五位 + 3字节 包含此PES流的PID值
	unsigned char reserved_6;					 //第N次循环的 4字节前四位 保留位
	unsigned short es_info_length;			 //第N次循环的 4字节后四位 + 5字节 表明后续ES流描述的相关字节数
	//循环结束
	unsigned int crc_32;					 //CRC校验 四字节
};
// 47 50 00 11 TS头
// 47 同步字节 
// 50 前三位自行解析
// 50 后五位 + 00 =  1 0000 0000 0000 = 2的12次 = 4096 对应上面的PAT表解析出来的program_map_ID
// 11 暂不解析
// 有一步丢弃为
// 00 丢弃位
// 有效PMT表数据 02 B0 17  00 01  C1  00 00  E1 00  F0 00  1B E1 00 F0 00  0F E1 01 F0 00  2F 44 B9 9B
// table_id = 02
// B0 17 重新分组 保留位 1 保留位 0 保留位 11 到crc_32的总字节数 000000010111 总字节数为 23
// 00 01 对应PAT表中的program_number = 1
// C1 重新分组 保留位 11 PAT版本号 00000 表示分段可用 1 
// 00 00 分别表示最大与最小分段号
// E1 00 重新分组 保留位 111 TS包的PID值 0000100000000 = 256是PCR值
// F0 00 重新分组 保留位 1111 最后长度 000000000000 节目信息描述符大小
// 循环次数 = 23 - 9 - 4 = 10字节 / 2 = 两循环
// 第一次 stream_type = 1B H256格式视频数据
// E1 00 重新分组 reserved = 111 保留位 elementary_pid = 0000100000000 = 256 表明包含流数据的包的PID = 256
// F0 00 重新分组 reserved = 1111 保留位 000000000000 = es_info_length 表明后续ES流描述的相关字节数 = 0 接下来跳过0字节
// 第二次 stream_type = 0F 代表是ACC音频数据
// E1 01 重新分组 reserved = 111 保留位 elementary_pid = 0000100000001 = 257 表明包含流数据的包的PID = 257
// F0 00 重新分组 reserved = 1111 保留位 000000000000 = es_info_length 表明后续流相关字节数位 = 0 接下来跳过0字节
// 2F 44 B9 9B crc_32校验
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
		std::cout << "左移 left = " << left << std::endl;
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
position 为字节起始位 1-8  8代表最后一位
size 需要大于16
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
	//解析TsHeader
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
	std::cout << "当前PAT表中有 " << num << " 个PMT表" << std::endl;
	patHeader.program_info = new PATProgramInfo[num];
	patHeader.programNum = num;
	for (int i = 0; i < num; i++)
	{
		unsigned short programNumber = byteToShort(data, 1, 16);
		patHeader.program_info->program_number = programNumber;
		patHeader.program_info->reserved_n = byteToChar(data, 1, 3);
		unsigned short nidOrPid = byteToShort(data, 4, 13);
		std::cout << "programNumber = " << programNumber << " , 节目号： nidOrPid = " << nidOrPid << std::endl;
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
			std::cout << "tsHeader 后是一个调整字节，跳过调整字节 =" << (int)*(tsPackage->data) << "." << std::endl;
			tsPackage->data++;
		}
		if (tsPackage->headr.pid == 0)
		{
			std::cout << "解析PAT表" << std::endl;
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
		cout << "root结点未找到" << endl;
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