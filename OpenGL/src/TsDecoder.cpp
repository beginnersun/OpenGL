//TS�ļ� === TS��+TS��+TS��
//TS �� Header

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
	unsigned char continuity_counter; //�����ֽ� ����λ pid��ͬʱ������continuity_counter�ж��Ⱥ�˳��һ��Ϊ��0-15��������15֮���ٴ�0-15����
	//��ǰ�������� 
	unsigned char type;
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
	struct PMTPESDataInfo *pes_Infos;
	int pesNumber;
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
// ѭ������ = 23 - 9 - 4 = 10�ֽ� / 5 = ��ѭ��
// ��һ�� stream_type = 1B H256��ʽ��Ƶ����
// E1 00 ���·��� reserved = 111 ����λ elementary_pid = 0000100000000 = 256 �������������ݵİ���PID = 256
// F0 00 ���·��� reserved = 1111 ����λ 000000000000 = es_info_length ��������ES������������ֽ��� = 0 ����������0�ֽ�
// �ڶ��� stream_type = 0F ������ACC��Ƶ����
// E1 01 ���·��� reserved = 111 ����λ elementary_pid = 0000100000001 = 257 �������������ݵİ���PID = 257
// F0 00 ���·��� reserved = 1111 ����λ 000000000000 = es_info_length ��������������ֽ���λ = 0 ����������0�ֽ�
// 2F 44 B9 9B crc_32У��
struct PMTPESDataInfo
{
	unsigned char stream_type;               //��N��ѭ���� 1�ֽ� �������ı����ʽ ���Ϊ0x1B ����ΪH256��ʽ ΪPES��
	//��MPEG1��Ƶ��0x02��MPEG2��Ƶ��0x03:MPEG1��Ƶ��0x04��MPEG2��Ƶ��0x05��˽���ֶΣ�0x06������˽�����ݵ�PES�� ...
	unsigned char reserved_5;					 //��N��ѭ���� 2�ֽ�ǰ��λ ����λ
	unsigned short elementary_pid;			 //��N��ѭ���� 2�ֽں���λ + 3�ֽ� ������PES����PIDֵ
	unsigned char reserved_6;					 //��N��ѭ���� 4�ֽ�ǰ��λ ����λ
	unsigned short es_info_length;			 //��N��ѭ���� 4�ֽں���λ + 5�ֽ� ��������ES������������ֽ���
};
struct PESHeader{
	unsigned int packet_start_code_prefix; //PES�̶���ͷ 0x000001���ֽ�
	unsigned char stream_id; //������ 0xE0-EF Ϊ��Ƶ 0xC0-DF Ϊ��Ƶ
	unsigned short pes_package_lengts; // ������ �����TS��һֱѰ��
	unsigned char dBits; // ��λ 10
	unsigned char scramblingControl; // ��λ 00 ������
	unsigned char priority; // ���ȼ� 1λ 0 ���ȼ��� 1���ȼ��ߵ�
	unsigned char alignmentIndicator; //1 λ ���ݶ�λָʾ��
	unsigned char copyRight; //1λ ��Ȩ��Ϣ 1Ϊ�а�Ȩ 0Ϊ�ް�Ȩ
	unsigned char originalOrCopy; //1λԭʼ�򱸷� 1ԭʼ 0����
	unsigned char PTSDTSFlag; //��λ 10 ��ʾPESͷ����PTS�ֶ� 11 ��ʾ��PTS��DTS�ֶ� 00��ʾû�� 01����ֹ  pts��dtsÿ����ռ40λ
	unsigned char ESCRFlag; // 1λ ��ʾͷ����ESCR�ֶ� 0������ ռ42λ ��33λ��ESCR_Base + 9λ��
	unsigned char ESRATEFLag; // 1λ ͬ�ϣ�ͷ��RATE�ֶΣ�  rateռ�� 24λ
	unsigned char DSMTrickModeFlag; //1λ ͬ�ϣ�track_mode�ֶΣ���ʾ���� trick mode ��Ӧ������Ӧ����Ƶ�� trick_mode ��trick_mode_controlռǰ3��bit  
	unsigned char AdditionalCopyInfoFlag; //1λ ͬ�ϣ�additional_copy_info�ֶΣ�  7bits ��ʾ�Ͱ�Ȩ��ص�˽������.
	unsigned char PESCRCFlag; //1λ ͬ�ϣ�CRC�ֶ�)  16bits CRC У��ֵ
	unsigned char PESExtensionFlag; //1λ ͬ��ͷ����extension�ֶ�
	//Optional field ��PES��չ�ֶεĿ�ѡ�ֶ�����˳��Ϊ��
	/*
	PES_private_data�ֶΣ�˽���������ݣ�ռλ128bit��PES_private_data_flag == 1ʱ���ֶδ��ڣ�
		Pack_header_field�ֶΣ�Pack_header_field_flag == 1ʱ���ֶδ��ڣ��ֶ����˳�����£�
		Pack_field_length�ֶΣ�(UI)ָ�������field�ĳ��ȣ�ռλ8bit��
		pack_header_field()������ΪPack_field_lengthָ����
		Program_packet_sequence_counter�ֶΣ��������ֶΣ�16��bit����flag�ֶ�Program_packet_sequence_counter_flag == 1ʱ���ֶδ��ڣ��ֽ�˳������Ϊ��
		marker_bit��ռλ1bit��
		packet_sequence_counter�ֶΣ�(UI)ռλ7bit��
		marker_bit��ռλ1bit��
		MPEG1_MPEG2_identifier��ռλ1bit����λ1��ʾ��PES���ĸ�������MPEG1������λ0��ʾ��PES���ĸ�������PS����
		original_stuff_length��(UI)ռλ6bit����ʾPESͷ������ֽڳ��ȣ�
		P - STD_buffer�ֶΣ���ʾP - STD_buffer���ݣ�ռλ16bit��P - STD_buffer_flag == '1��ʱ���ֶδ��ڣ��ֽ�˳������Ϊ��
		��01���ֶΣ�ռλ2bit��
		P - STD_buffer_scale��ռλ1bit����ʾ�������ͺ���P - STD_buffer_size�ֶεı������ӣ����֮ǰ��stream_id��ʾ��Ƶ�������ֵӦΪ0����֮ǰ��stream_id��ʾ��Ƶ�������ֵӦΪ1����������stream���ͣ���ֵ����0��1��
		P - STD_buffer_size��ռλ13bit���޷������������ڻ��������P - STD���뻺������СBSn�����ֵ����P - STD_buffer_scale == 0����P - STD_buffer_size��128�ֽ�Ϊ��λ����P - STD_buffer_scale == 1����P - STD_buffer_size��1024�ֽ�Ϊ��λ��
		PES_extension 2���ֶΣ���չ�ֶε���չ�ֶΣ�ռ��N * 8��bit��PES_extension_flag_2 == '1��ʱ���ֶδ��ڣ��ֽ�˳������Ϊ��
		marker_bit��ռλ1bit��
		PES_extension_field_length��ռλ7bit����ʾ��չ����ĳ��ȣ�
		Reserved�ֶΣ�ռλ8*PES_extension_field_length��bit��
		*/

	unsigned char PESHeaderDataLength; //8λ��������7����ѡ���ݵĳ��ȣ� �� PES ��ͷ���п�ѡͷ���ֶκ����������ֽ���ռ�����ֽ�������ѡ�ֶε������������ 7 �� flag �����Ƶ�.
	unsigned char *esData;
};


struct TsPackageData
{
	unsigned char data[188];
};
//��ȡMap���б�ʽ�洢  pid��ͬʹ��ͬһid Ȼ��next׷Ѱ��ȥ
struct TsPackage
{
	TsHeader headr;
	PATHeader pat;
	PMTHeader pmt;
	TsPackage *next; //��Զֻ��¼��һ�� pi��ͬʱ������headr�е�continuity_controller ��ֵ�ж��Ⱥ�˳�� ���headr�е�payload_unit_start_indircationΪ0����˰�pat�Ŀ�ʼ����
	TsPackage *lastNode; //���lastNode == next ˵��PAT��С��ֻ��һ�����͸����� (����ÿ�μ�¼���һ��)
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


/*
  ��byte������Ӧ������λ
*/
unsigned int skipBits(unsigned char * &byte, int size) {

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
	//std::cout << "��ǰPAT������ " << num << " ��PMT��" << std::endl;
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
				std::cout << "programNumber = " << programNumber << " , ��Ŀ�ţ� nid = " << patHeader.program_info->network_PID << std::endl;
			}
		}
		else {
			program_info->program_map_PID = nidOrPid;
			program_info->network_PID = -1;
			if (isDebug)
			{
				std::cout << "programNumber = " << programNumber << " , ��Ŀ�ţ� pid = " << patHeader.program_info->program_map_PID << std::endl;
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
	//adaptation_filed_control == 11 �����е����ֶ� ��Ҫ����
	if (package->headr.adaptation_filed_control == 3)
	{
		adaptationFiledSize = *data;
		data++;
	}
	// ������Ӧ�ֽ���
	while (adaptationFiledSize > 0)
	{
		data++;
	}
	unsigned int pesStart = byteToInt(data, 1, 24);
	if (pesStart != 1)
	{
		cout << "��ʼ�ַ�����" << endl;
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
		//����1λ��ռλ��
		unsigned char additionalCopyInfo = byteToChar(data, 2, 7); //��Ȩ���˽������
		data++;
	}
	if (header.PESCRCFlag == 1)
	{
		unsigned char pesPackageCrc = byteToShort(data, 1, 16); //У��ֵ
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
			//��ȡ128bit�ֽ����� PES_private_data
		}
		if (esHeaderFieldFlag == 1)
		{
			//P��ȡ8Bit�� Pack_field_length + pack_header_field(length = Pack_field_length)
		}
		if (esPacketSequenceCounterFlag == 1)
		{
			//��ȡ16bit 
			/*	marker_bit��ռλ1bit��
				packet_sequence_counter�ֶΣ�(UI)ռλ7bit��
				marker_bit��ռλ1bit��
				MPEG1_MPEG2_identifier��ռλ1bit����λ1��ʾ��PES���ĸ�������MPEG1������λ0��ʾ��PES���ĸ�������PS����
				original_stuff_length��(UI)ռλ6bit����ʾPESͷ������ֽڳ��ȣ�*/
			unsigned char markerBit = byteToChar(data, 1, 1);
			//unsigned char packetSequ
		}
	}

}

bool isPESPackage(){
	return true;
}

// PAT = 0 , CAT = 1 , TSDT = 2, EIT|ST = 0x12 ,RST_ST = 0x13,TDT|TOT|ST = 0x14 ��ЩΪPSI���ݣ�Ҳ������Щ���
// PMT �ȶ�̬���ɵ�PID ��Ҫ�ж�type����ȷ������
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
		//ts��ͷ�̶����ֽڽ���
		detectTsPackageTsHeader(tsPackage);
		tsPackage->headr.type = TYPE_NONE;
		//std::cout << "pid = " << tsPackage->headr.pid << std::endl;
		bool isRoot = false;
		//��ǰpidΪ0����ͷ�����Ƚ�����ͷ Ȼ���ٸ��ݿ�ͷ�������(����ʹ�ö��еȷ�ʽ)
		//payload_unit_start_indircation == 1 �������ͷ��1�ֽ��ǵ������ݣ��������ɣ� �������ͬʱadaptation_filed_control == 3�����ͷ��һ���ֽ�������ֽڳ��ȡ�
		//����ֽ�=�������ֽڣ�0x00)+ ����-1��0xFF)
		//����Ϊpsiʱ������adaptation_field_control
		//&& tsPackage->headr.adaptation_filed_control != 3
		if (tsPackage->headr.payload_unit_start_indircation == 1 && isPSIPackage(tsPackage->headr.pid))
		{
			isRoot = true;
			//std::cout << "tsHeader ����һ�������ֽڣ����������ֽ� =" << (int)*(tsPackage->data) << "." << std::endl;
			tsPackage->data++;
		}
		if (tsPackage->headr.pid == 0)
		{
			//std::cout << "����PAT��" << (int)tsPackage->headr.payload_unit_start_indircation << " , continuity_counter = " << (int)tsPackage->headr.continuity_counter << std::endl;
			detectTsPackageTsPAT(tsPackage);
			//std::cout << "pmtId = " << (int)tsPackage->pat.program_info->program_map_PID << std::endl;
			tsPackage->headr.type = TYPE_PAT;
			TsPackage *root = packageMaps[tsPackage->headr.pid];
			if (root != NULL) {
				root->lastNode->next = tsPackage;
				root->lastNode = tsPackage;
				//std::cout << "����next" << std::endl;
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
		cout << "root���δ�ҵ�" << endl;
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
							cout << "�Ҳ�����pid = " << programPid << endl;
						}
					}
					infos++;
				}
				tsNode = tsNode->next;
			}
		}
		else if (tsNode->headr.type == TYPE_PMT)
		{
			//��������Ч�غɵ�Ԫ��ʼָʾ���ֱ�Ϊ1����ƵTS������֪��������һ֡�Ŀ�ʼ��
			while (tsNode != NULL)
			{
				cout << "��Ч�غ���ʼָʾ�� = " << (int)tsNode->headr.payload_unit_start_indircation << " , country" << (int)tsNode->headr.continuity_counter << endl;
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
				cout << "����һ��" << endl;
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