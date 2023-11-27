//TS文件 === TS包+TS包+TS包
//TS 包 Header

#include<fstream>
#include<iostream>
#include <io.h>

#include<stdio.h>
#include<string>
#include<atlstr.h>

#include<map>
#include<queue>

using namespace std;

bool isDebug = false;
#define TYPE_PAT 1
#define TYPE_PMT 2
#define TYPE_PES 3
#define TYPE_MP4 0x1b
#define TYPE_ACC 5
#define TYPE_NONE -1

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
	unsigned char continuity_counter; //第四字节 后四位 pid相同时，根据continuity_counter判断先后顺序（一般为从0-15递增）到15之后再从0-15递增
	//当前数据类型 
	unsigned char type;
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
	struct PMTPESDataInfo *pes_Infos;
	int pesNumber;
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
// 循环次数 = 23 - 9 - 4 = 10字节 / 5 = 两循环
// 第一次 stream_type = 1B H256格式视频数据
// E1 00 重新分组 reserved = 111 保留位 elementary_pid = 0000100000000 = 256 表明包含流数据的包的PID = 256
// F0 00 重新分组 reserved = 1111 保留位 000000000000 = es_info_length 表明后续ES流描述的相关字节数 = 0 接下来跳过0字节
// 第二次 stream_type = 0F 代表是ACC音频数据
// E1 01 重新分组 reserved = 111 保留位 elementary_pid = 0000100000001 = 257 表明包含流数据的包的PID = 257
// F0 00 重新分组 reserved = 1111 保留位 000000000000 = es_info_length 表明后续流相关字节数位 = 0 接下来跳过0字节
// 2F 44 B9 9B crc_32校验
struct PMTPESDataInfo
{
	unsigned char stream_type;               //第N次循环的 1字节 表明流的编码格式 如果为0x1B 表明为H256格式 为PES流
	//：MPEG1视频，0x02：MPEG2视频，0x03:MPEG1音频，0x04：MPEG2音频，0x05：私有字段，0x06：含有私有数据的PES包 ...
	unsigned char reserved_5;					 //第N次循环的 2字节前三位 保留位
	unsigned short elementary_pid;			 //第N次循环的 2字节后五位 + 3字节 包含此PES流的PID值
	unsigned char reserved_6;					 //第N次循环的 4字节前四位 保留位
	unsigned short es_info_length;			 //第N次循环的 4字节后四位 + 5字节 表明后续ES流描述的相关字节数
};
struct PESHeader{
	unsigned int packet_start_code_prefix; //PES固定包头 0x000001三字节
	unsigned char stream_id; //流类型 0xE0-EF 为视频 0xC0-DF 为音频
	unsigned short pes_package_lengts; // 包长度 往后的TS包一直寻找
	unsigned char dBits; // 两位 10
	unsigned char scramblingControl; // 两位 00 不加密
	unsigned char priority; // 优先级 1位 0 优先级低 1优先级高点
	unsigned char alignmentIndicator; //1 位 数据定位指示器
	unsigned char copyRight; //1位 版权信息 1为有版权 0为无版权
	unsigned char originalOrCopy; //1位原始或备份 1原始 0备份
	unsigned char PTSDTSFlag; //两位 10 表示PES头部有PTS字段 11 表示有PTS或DTS字段 00表示没有 01被禁止  pts与dts每个各占40位
	unsigned char ESCRFlag; // 1位 表示头部有ESCR字段 0代表无 占42位 由33位的ESCR_Base + 9位的
	unsigned char ESRATEFLag; // 1位 同上（头部RATE字段）  rate占用 24位
	unsigned char DSMTrickModeFlag; //1位 同上（track_mode字段）表示哪种 trick mode 被应用于相应的视频流 trick_mode 由trick_mode_control占前3个bit  
	unsigned char AdditionalCopyInfoFlag; //1位 同上（additional_copy_info字段）  7bits 表示和版权相关的私有数据.
	unsigned char PESCRCFlag; //1位 同上（CRC字段)  16bits CRC 校验值
	unsigned char PESExtensionFlag; //1位 同上头部有extension字段
	//Optional field ：PES扩展字段的可选字段内容顺序为：
	/*
	PES_private_data字段：私有数据内容，占位128bit，PES_private_data_flag == 1时此字段存在；
		Pack_header_field字段：Pack_header_field_flag == 1时此字段存在；字段组成顺序如下：
		Pack_field_length字段：(UI)指定后面的field的长度，占位8bit；
		pack_header_field()：长度为Pack_field_length指定；
		Program_packet_sequence_counter字段：计数器字段，16个bit；当flag字段Program_packet_sequence_counter_flag == 1时此字段存在；字节顺序依次为：
		marker_bit：占位1bit；
		packet_sequence_counter字段：(UI)占位7bit；
		marker_bit：占位1bit；
		MPEG1_MPEG2_identifier：占位1bit；置位1表示此PES包的负载来自MPEG1流，置位0表示此PES包的负载来自PS流；
		original_stuff_length：(UI)占位6bit；表示PES头部填充字节长度；
		P - STD_buffer字段：表示P - STD_buffer内容，占位16bit；P - STD_buffer_flag == '1’时此字段存在；字节顺序依次为：
		’01’字段：占位2bit；
		P - STD_buffer_scale：占位1bit；表示用来解释后面P - STD_buffer_size字段的比例因子；如果之前的stream_id表示音频流，则此值应为0，若之前的stream_id表示视频流，则此值应为1，对于其他stream类型，此值可以0或1；
		P - STD_buffer_size：占位13bit；无符号整数；大于或等于所有P - STD输入缓冲区大小BSn的最大值；若P - STD_buffer_scale == 0，则P - STD_buffer_size以128字节为单位；若P - STD_buffer_scale == 1，则P - STD_buffer_size以1024字节为单位；
		PES_extension 2个字段：扩展字段的扩展字段；占用N * 8个bit，PES_extension_flag_2 == '1’时此字段存在；字节顺序依次为：
		marker_bit：占位1bit；
		PES_extension_field_length：占位7bit，表示扩展区域的长度；
		Reserved字段：占位8*PES_extension_field_length个bit；
		*/

	unsigned char PESHeaderDataLength; //8位（以上那7个可选内容的长度） 在 PES 包头部中可选头部字段和任意的填充字节所占的总字节数。可选字段的内容由上面的 7 个 flag 来控制的.
	unsigned char *esData;
};


struct TsPackageData
{
	unsigned char data[188];
};
//采取Map加列表方式存储  pid相同使用同一id 然后next追寻下去
struct TsPackage
{
	TsHeader headr;
	PATHeader pat;
	PMTHeader pmt;
	TsPackage *next; //永远只记录下一个 pi相同时，根据headr中的continuity_controller 的值判断先后顺序 如果headr中的payload_unit_start_indircation为0代表此包pat的开始部分
	TsPackage *lastNode; //如果lastNode == next 说明PAT很小。只有一个包就覆盖了 (用于每次记录最后一个)
	unsigned char* data;
};

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


/*
  将byte跳过对应数量的位
*/
unsigned int skipBits(unsigned char * &byte, int size) {

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
	//std::cout << "section_synatx_indication = " << (int)patHeader.section_synatx_indication << "." << std::endl;
	patHeader.zero = byteToChar(data, 2, 1);
	//std::cout << "zero = " << (int)patHeader.zero << "." << std::endl;
	patHeader.reserved_1 = byteToChar(data, 3, 2);
	//std::cout << "reserved_1 = " << (int)patHeader.reserved_1 << "." << std::endl;
	patHeader.section_length = byteToShort(data, 5, 12);
	//std::cout << "section_length = " << (int)patHeader.section_length << "." << std::endl;
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
	//std::cout << "当前PAT表中有 " << num << " 个PMT表" << std::endl;
	PATProgramInfo* program_info = new PATProgramInfo[num];
	patHeader.program_info = program_info;
	patHeader.programNum = num;
	for (int i = 0; i < num; i++)
	{
		unsigned short programNumber = byteToShort(data, 1, 16);
		program_info->program_number = programNumber;
		program_info->reserved_n = byteToChar(data, 1, 3);
		unsigned short nidOrPid = byteToShort(data, 4, 13);
		if (programNumber == 0)
		{
			program_info->network_PID = nidOrPid;
			program_info->program_map_PID = -1;
			if (isDebug)
			{
				std::cout << "programNumber = " << programNumber << " , 节目号： nid = " << patHeader.program_info->network_PID << std::endl;
			}
		}
		else {
			program_info->program_map_PID = nidOrPid;
			program_info->network_PID = -1;
			if (isDebug)
			{
				std::cout << "programNumber = " << programNumber << " , 节目号： pid = " << patHeader.program_info->program_map_PID << std::endl;
			}
		}
		program_info++;
	}
	patHeader.crc_32 = byteToInt(data, 1, 32);
	tsPackage->data = data;
	tsPackage->pat = patHeader;
}
void detectPackageTsPMT(TsPackage *package) {
	unsigned char *data = package->data;
	PMTHeader header;
	header.table_id = *data;
	data++;
	header.section_synatx_indication = byteToChar(data, 1, 1);
	header.zero = byteToChar(data, 2, 1);
	header.reserved_1 = byteToChar(data, 3, 2);
	header.section_length = byteToShort(data, 5, 12);
	header.program_number = byteToShort(data, 1, 16);
	header.reserved_2 = byteToChar(data, 1, 2);
	header.version_number = byteToChar(data, 3, 5);
	header.current_next_indicator = byteToChar(data, 8, 1);
	data++;
	header.section_number = byteToChar(data, 1, 8);
	data++;
	header.last_section_number = byteToChar(data, 1, 8);
	data++;
	header.reserved_3 = byteToChar(data, 1, 3);
	header.PCR_PID = byteToShort(data, 4, 13);
	header.reserved_4 = byteToChar(data, 1, 4);
	header.program_info_length = byteToShort(data, 5, 12);
	int packageSize = header.section_length;
	int esDataInfoSize = (packageSize - 9 - 4) / 2;

	PMTPESDataInfo *pes_Infos = new PMTPESDataInfo[esDataInfoSize];
	header.pes_Infos = pes_Infos;

	header.pesNumber = esDataInfoSize;
	for(int i = 0; i < esDataInfoSize ; i++){
		unsigned char stream_type = *data;
		data++;
		unsigned char reserved = byteToChar(data, 1, 3);
		unsigned short elementary_pid = byteToShort(data, 4, 13);
		unsigned char reserved_1 = byteToChar(data, 1, 4);
		unsigned short es_info_length = byteToShort(data, 5, 12);
		// header.pes_Infos[0]->stream_type = 
		pes_Infos->stream_type = stream_type;
		pes_Infos->reserved_5 = reserved;
		pes_Infos->elementary_pid = elementary_pid;
		pes_Infos->reserved_6 = reserved_1;
		pes_Infos->es_info_length = es_info_length;
		pes_Infos++;
	}
	header.crc_32 = byteToInt(data, 1, 32);
	package->data = data;
	package->pmt = header;
}

void detectPackageTsPES(TsPackage *package) {
	unsigned char *data = package->data;
	int adaptationFiledSize = -1;
	//adaptation_filed_control == 11 代表有调整字段 需要跳过
	if (package->headr.adaptation_filed_control == 3)
	{
		adaptationFiledSize = *data;
		data++;
	}
	// 跳过对应字节数
	while (adaptationFiledSize > 0)
	{
		data++;
	}
	unsigned int pesStart = byteToInt(data, 1, 24);
	if (pesStart != 1)
	{
		cout << "起始字符错误" << endl;
		return;
	}
	PESHeader header;
	header.packet_start_code_prefix = pesStart;
	header.stream_id = byteToChar(data, 1, 8);
	data++;
	header.pes_package_lengts = byteToShort(data, 1, 16);
	header.dBits = byteToChar(data, 1, 2);
	header.scramblingControl = byteToChar(data, 3, 2);
	header.priority = byteToChar(data, 5, 1);
	header.alignmentIndicator = byteToChar(data, 6, 1);
	header.copyRight = byteToChar(data, 7, 1);
	header.originalOrCopy = byteToChar(data, 8, 1);
	data++;
	header.PTSDTSFlag = byteToChar(data, 1, 2);
	header.ESCRFlag = byteToChar(data, 3, 1);
	header.ESRATEFLag = byteToChar(data, 4, 1);
	header.DSMTrickModeFlag = byteToChar(data, 5, 1);
	header.AdditionalCopyInfoFlag = byteToChar(data, 6, 1);
	header.PESCRCFlag = byteToChar(data, 7, 1);
	header.PESExtensionFlag = byteToChar(data, 8, 1);
	header.PESHeaderDataLength = byteToChar(data, 1, 8);
	data++;
	if (header.PTSDTSFlag == 2)
	{
		unsigned char bit5_3 = byteToChar(data, 5, 3);
		data++;
		unsigned short bit0_15 = byteToShort(data, 1, 15);
		data++;
		unsigned short bit0_16 = byteToShort(data, 1, 15);
		data++;
	}
	if (header.PTSDTSFlag == 3)
	{
		unsigned char bit5_3 = byteToChar(data, 5, 3);
		data++;
		unsigned short bit0_15 = byteToShort(data, 1, 15);
		data++;
		unsigned short bit0_16 = byteToShort(data, 1, 15);
		data++;
	}

	if (header.ESCRFlag == 1)
	{
		unsigned long bit33_1 = byteToInt(data, 1, 32);
		unsigned long escrBase = (bit33_1 << 1) | byteToChar(data, 1, 1);
		unsigned short escrExtension = byteToShort(data, 2, 9);
	}
	if (header.ESRATEFLag == 1)
	{
		unsigned int esRate = byteToInt(data, 1, 24);
	}
	if (header.DSMTrickModeFlag == 1)
	{
		unsigned char trickModeControl = byteToChar(data, 1, 3);
		unsigned char trickMode = byteToChar(data, 4, 5);
		data++;
	}
	if (header.AdditionalCopyInfoFlag == 1)
	{
		//跳过1位的占位符
		unsigned char additionalCopyInfo = byteToChar(data, 2, 7); //版权相关私有数据
		data++;
	}
	if (header.PESCRCFlag == 1)
	{
		unsigned char pesPackageCrc = byteToShort(data, 1, 16); //校验值
	}
	if (header.PESExtensionFlag == 1)
	{
		unsigned char esPrivateDataFlag = byteToChar(data, 1, 1);
		unsigned char esHeaderFieldFlag = byteToChar(data, 2, 1);
		unsigned char esPacketSequenceCounterFlag = byteToChar(data, 4, 1);
		unsigned char esReservedFlag = byteToChar(data, 5, 3);
		unsigned char extensionFlag2 = byteToChar(data, 8, 1);
		if (esPrivateDataFlag == 1)
		{
			//读取128bit字节数据 PES_private_data
		}
		if (esHeaderFieldFlag == 1)
		{
			//P读取8Bit的 Pack_field_length + pack_header_field(length = Pack_field_length)
		}
		if (esPacketSequenceCounterFlag == 1)
		{
			//读取16bit 
			/*	marker_bit：占位1bit；
				packet_sequence_counter字段：(UI)占位7bit；
				marker_bit：占位1bit；
				MPEG1_MPEG2_identifier：占位1bit；置位1表示此PES包的负载来自MPEG1流，置位0表示此PES包的负载来自PS流；
				original_stuff_length：(UI)占位6bit；表示PES头部填充字节长度；*/
			unsigned char markerBit = byteToChar(data, 1, 1);
			//unsigned char packetSequ
		}
	}

}

bool isPESPackage(){
	return true;
}

// PAT = 0 , CAT = 1 , TSDT = 2, EIT|ST = 0x12 ,RST_ST = 0x13,TDT|TOT|ST = 0x14 这些为PSI数据（也就是那些表格）
// PMT 等动态生成的PID 需要判断type才能确定类型
bool isPSIPackage(int pid){
	return true;
}

int main_decode_ts() {
	std::string filename = "C:\\Users\\ThemSun\\Downloads\\1";
	FILE* m_TsFile_Fp = fopen(filename.c_str(), "rb");
	if (m_TsFile_Fp == NULL)
	{
		std::cout << "file open fail." << std::endl;
		return -1;
	}
	int fileSize = filelength(fileno(m_TsFile_Fp));
	std::cout << "ts File size = " << fileSize << std::endl;
	float tsPreCount = fileSize / 188.0f;
	int tsCount = fileSize / 188;
	if (tsCount * 1.0 == tsPreCount)
	{
		std::cout << "ts File have " << tsCount << " ts package" << std::endl;
	}
	TsPackage *tsPackage = (TsPackage *)malloc(tsCount * sizeof(TsPackage));
	map<int, TsPackage*> packageMaps;
	map<int, bool> isAdded;
	for (int i = 0; i < tsCount; i++)
	{
		unsigned char *data = new unsigned char[188];
		fread(data, 1, 188, m_TsFile_Fp);
		//std::cout << "first read byte = " << (int)*(data) << std::endl;
		tsPackage->data = data;
		//ts包头固定四字节解析
		detectTsPackageTsHeader(tsPackage);
		tsPackage->headr.type = TYPE_NONE;
		//std::cout << "pid = " << tsPackage->headr.pid << std::endl;
		bool isRoot = false;
		//当前pid为0代表开头。。先解析开头 然后再根据开头往后解析(可以使用队列等方式)
		//payload_unit_start_indircation == 1 代表包后头面1字节是调节数据（跳过即可） 但是如果同时adaptation_filed_control == 3代表包头后一个字节是填充字节长度。
		//填充字节=（调整字节（0x00)+ 长度-1个0xFF)
		//数据为psi时不考虑adaptation_field_control
		//&& tsPackage->headr.adaptation_filed_control != 3
		if (tsPackage->headr.payload_unit_start_indircation == 1 && isPSIPackage(tsPackage->headr.pid))
		{
			isRoot = true;
			//std::cout << "tsHeader 后是一个调整字节，跳过调整字节 =" << (int)*(tsPackage->data) << "." << std::endl;
			tsPackage->data++;
		}
		if (tsPackage->headr.pid == 0)
		{
			//std::cout << "解析PAT表" << (int)tsPackage->headr.payload_unit_start_indircation << " , continuity_counter = " << (int)tsPackage->headr.continuity_counter << std::endl;
			detectTsPackageTsPAT(tsPackage);
			//std::cout << "pmtId = " << (int)tsPackage->pat.program_info->program_map_PID << std::endl;
			tsPackage->headr.type = TYPE_PAT;
			TsPackage *root = packageMaps[tsPackage->headr.pid];
			if (root != NULL) {
				root->lastNode->next = tsPackage;
				root->lastNode = tsPackage;
				//std::cout << "设置next" << std::endl;
			}
			else {
				packageMaps[tsPackage->headr.pid] = tsPackage;
				tsPackage->lastNode = tsPackage;
			}
		} else {
			TsPackage *root = packageMaps[tsPackage->headr.pid];
			if (root != NULL) {
				root->lastNode->next = tsPackage;
				root->lastNode = tsPackage;
			}
			else {
				packageMaps[tsPackage->headr.pid] = tsPackage;
				tsPackage->lastNode = tsPackage;
			}
		}
		tsPackage++;
	}
	fclose(m_TsFile_Fp);
	TsPackage *rootPackage = packageMaps[0];
	if (rootPackage == NULL)
	{
		cout << "root结点未找到" << endl;
		return -1;
	}
	cout << "packageMaps.size = " << packageMaps.size() << endl;
	queue<TsPackage*> rootQueue;
	rootQueue.push(rootPackage);
	while (!rootQueue.empty()) {
		TsPackage *tsNode = rootQueue.front();
		rootQueue.pop();
		if (tsNode->headr.type == TYPE_PAT)
		{
			while (tsNode != NULL)
			{
				int pmtNum = tsNode->pat.programNum;
				PATProgramInfo *infos = tsNode->pat.program_info;
				for (int i = 0; i < pmtNum; i++)
				{
					int programPid = infos->program_map_PID;
					int programNumber = infos->program_number;
					if (!isAdded[programPid])
					{
						TsPackage* programPkg = packageMaps[programPid];
						if (programPkg != NULL)
						{
							programPkg->headr.type = TYPE_PMT;
							rootQueue.push(programPkg);
							isAdded[programPid] = true;
							TsPackage *pmNext = programPkg->next;
							while (pmNext != NULL)
							{
								pmNext->headr.type = TYPE_PMT;
								pmNext = pmNext->next;
							}
						}
						else {
							cout << "找不到的pid = " << programPid << endl;
						}
					}
					infos++;
				}
				tsNode = tsNode->next;
			}
		}
		else if (tsNode->headr.type == TYPE_PMT)
		{
			//当碰到有效载荷单元起始指示符又变为1的视频TS包，就知道这是下一帧的开始了
			while (tsNode != NULL)
			{
				cout << "有效载荷起始指示符 = " << (int)tsNode->headr.payload_unit_start_indircation << " , country" << (int)tsNode->headr.continuity_counter << endl;
				detectPackageTsPMT(tsNode);
				int pesNum = tsNode->pmt.pesNumber;
				PMTPESDataInfo *infos = tsNode->pmt.pes_Infos;
				cout << "pesNum = " << pesNum << endl;
				for (int i = 0; i < pesNum; i++)
				{
					TsPackage *pesPackage = packageMaps[infos->elementary_pid];
					if (pesPackage != NULL && !isAdded[infos->elementary_pid])
					{
						pesPackage->headr.type = infos->stream_type;
						rootQueue.push(pesPackage);
						isAdded[pesPackage->headr.pid] = true;
						TsPackage *pmNext = pesPackage->next;
						while (pmNext != NULL)
						{
							pmNext->headr.type = infos->stream_type;
							pmNext = pmNext->next;
						}
					}
					if ((int)infos->stream_type != TYPE_MP4)
					{
						cout << "type = " << (int)infos->stream_type << " , pid = " << infos->elementary_pid << " , length = " << infos->es_info_length << endl;
					}
				}
				cout << "结束一段" << endl;
				tsNode = tsNode->next;
			}
			//for (int i = 0; i < pesNum; i++)
			//{
				//int pesPid = infos->elementary_pid;
				//TsPackage* programPkg = packageMaps[pesPid];
				//programPkg->headr.type = TYPE_PMT;
				//rootQueue.push(programPkg);
			//}
		}
		else if (tsNode->headr.type == TYPE_MP4)
		{

		}
	}
	/*rootPackage->pat.programNum;
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
			detectPackageTsPMT(pmtPackage);
		}
	}*/
	return 0;
}