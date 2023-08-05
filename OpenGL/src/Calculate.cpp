#include<stdio.h>
#include<string>
#include<atlstr.h>
#include<fstream>
#include<iostream>
#include<json.h>


#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

using namespace std;

//fread���� ��ȡint����������ʱ����С�����ȡ  ����int�������ݰ��մ�����ȡ
//�����ڶ�ȡ��ֵ֮����Ҫ�Դ�С�������ת��
unsigned long OnChangeByteOrder(int indata)
{
	unsigned char *c = (unsigned char*)&indata;
	int sum = 0;
	for (int i = 0; i < sizeof(int); i++)
	{
		sum += (*(c + i) << ((sizeof(int) - 1 - i) * 8));
	}
	//printf("ת���Ľ�� : %d \n",sum);
	return sum;
}

struct PolyPoint {
	double positionX;
	double positionY;

	int getLength() {
		return 4 + sizeof(double) * 2;
	}
};

struct PolyPointEntity
{
	double Box[4];
};

struct PolyLine
{
	double Box[4]; //��ǰĿ���ߵķ�Χ  ˳�� Xmin Ymin Xmax Ymax
	int NumParts; //��ǰ�� ���߶�����
	int NumPoints; //��ǰ�߶����е������
	int *Parts; //����ÿһ���߶εĿ�ʼ����Points�е��±�
	PolyPoint *Points;//�������е�����

	int getLength() {
		//ShapeType  + Box + NumParts + NumPoints + parts + points
		return 4 + (sizeof(double) * 4) + sizeof(int) + sizeof(int) + sizeof(int)*NumParts + sizeof(PolyPoint)*NumPoints;
	}
};

struct Polygongon
{
	double Box[4];
	int NumParts;
	int NumPoints;
	int *Parts;
	PolyPoint *Points;

	int getLength() {
		return 4 + (sizeof(double) * 4) + sizeof(int) + sizeof(int) + sizeof(int)*NumParts + sizeof(PolyPoint)*NumPoints;
	}
};

int readPoint(string polyInfo,PolyPoint * &polypoints) {
	Json::Value root;
	const string info = polyInfo;
	Json::Reader reader = Json::Reader();
	if (!reader.parse(polyInfo,root))
	{
		cout << "���ݸ�ʽ����" << endl;
	}
	int length = root.size();
	polypoints = (PolyPoint *)malloc(sizeof(PolyPoint*)*length);
	for (int i = 0; i < length; i++)
	{
		int type = root[i]["type"].asInt();
		Json::Value points = root[i]["datas"];
		PolyPoint *polypoint = polypoints + i;
		if (points.size() > 0)
		{
			Json::Value point = points[0];
			polypoint->positionX = point["x"].asDouble();
			polypoint->positionY = point["y"].asDouble();
		}
	}
	return length;
}

int readPolygon(string polyInfo,Polygongon * &polygons) {
	Json::Value root;
	const string info = polyInfo;
	Json::Reader reader = Json::Reader();
	if (!reader.parse(polyInfo,root))
	{
		cout << "���ݸ�ʽ����" << endl;
	}
	int length = root.size();
	polygons = (Polygongon *)malloc(sizeof(Polygongon)*length);
	for (int i = 0; i < length; i++)
	{
		int type = root[i]["type"].asInt();
		Json::Value points = root[i]["datas"];
		Polygongon *polygon = polygons + i;
		polygon->NumParts = 1;
		polygon->Parts = (int *)malloc(sizeof(int) * 1);
		polygon->Parts[0] = 0;
		polygon->NumPoints = points.size();
		polygon->Points = (PolyPoint*)malloc(sizeof(PolyPoint*)*points.size());
		double minx = 300, miny = 300, maxx = -300, maxy = -300;
		switch (type)
		{
		case 5:{
			for (int j = 0; j < length; j++)
			{
				Json::Value point = points[j];
				double x = point["x"].asDouble();
				double y = point["y"].asDouble();
				(polygon->Points + j)->positionX = x;
				(polygon->Points + j)->positionY = y;
				if (minx > x)
				{
					minx = x;
				}
				if (miny > y)
				{
					miny = y;
				}
				if (maxx < x)
				{
					maxx = x;
				}
				if (maxy < y)
				{
					maxy = y;
				}
			}
		}
		default:
			break;
		}
		polygon->Box[0] = minx;
		polygon->Box[1] = miny;
		polygon->Box[2] = maxx;
		polygon->Box[3] = maxy;
	}
	return length;
}

int read(string polyInfo,PolyLine * &polylines) {
	Json::Value root;
	const string info = polyInfo;
	Json::Reader reader = Json::Reader();
	if (!reader.parse(polyInfo,root))
	{
		cout << "���ݸ�ʽ���󣡣���" << endl;
	}
	int length = root.size();
	polylines = (PolyLine *)malloc(sizeof(PolyLine)*length);
	for (int i = 0; i < length; i++)
	{
		int type = root[i]["type"].asInt();
		Json::Value points = root[i]["datas"];
		PolyLine *polyLine = polylines + i;
		polyLine->NumParts = 1;
		polyLine->Parts = (int*)malloc(sizeof(int)*polyLine->NumParts);
		polyLine->Parts[0] = 0;
		polyLine->NumPoints = points.size();
		polyLine->Points = (PolyPoint*)malloc(sizeof(PolyPoint*)*points.size());
		double minx = 300,miny = 300,maxx = -300,maxy = -300;
		printf("datas.size = %d\n", polyLine->NumParts);
		switch (type)
		{
		case 3: {//3 ��������
			for (int j = 0; j < points.size(); j++)
			{
				Json::Value point = points[j];
				double x = point["x"].asDouble();
				double y = point["y"].asDouble();
				(polyLine->Points + j)->positionX = x;
				(polyLine->Points + j)->positionY = y;
				printf("j = %d ;  %lf , %lf \n",j, x, y);
				if (x > maxx)
				{
					maxx = x;
				}
				if (x < minx)
				{
					minx = x;
				}
				if (y > maxy)
				{
					maxy = y;
				}
				if (y < miny)
				{
					miny = y;
				}
			}
			break;
		}
		default:
			break;
		}
		polyLine->Box[0] = minx;
		polyLine->Box[1] = miny;
		polyLine->Box[2] = maxx;
		polyLine->Box[3] = maxy;
	}
	return length;
}

int writeShp(PolyLine *polyLine,int num,const string path);
int writeShpPoint(PolyPoint *polyPoints,int num,const string path);
int writeShpPolygon(Polygongon *polygons,int num,const string path);
int writeShpFromJson(const string jsonInfo,int type,const string path);

string readShpToJson(const string path);

string cc(const string path);

int cccdd(void) {
	//writeShpPolygon();
	//cc();
	//PolyLine *polyLine;


	//int status = writeShpFromJson("[{\"type\":3,\"datas\":[{\"x\":118.34563,\"y\":24.34533},{\"x\":117.87648,\"y\":25.37897},{\"x\":118.38764,\"y\":24.8974}]}]", 3,"D://outPutLine.shp");
	//cout << "�����С " << polyLine->NumPoints << endl;
	//cout << "д���ļ�:" << status << endl;
	/*writeShp(polyLine);*/
	//cc();

	readShpToJson("D://testmian.shp");
	return 1;
}

int writeShpPolygon(Polygongon *polygon,int num,const string path) {
	// �Ȳ���д��Χ���ݵȱ�����������㷶Χ Ȼ����seek����Ӧλ��д������
		//��ڽ���Polygon���� ע�⻷�ڽ�����ʱ��Ӧ�ô�����
 	int i, j;
	int polyNum = 10;
	int FileCode = 9994;
	int Unused = 0;
	int FileLength = 1000;
	int Version = 1000;
	int ShapeType = 5;
	int unUsed = 0;
	double Xmin = 300;
	double Ymin = 300;
	double Xmax = -300;
	double Ymax = -300;
	double Zmin = 0;
	double Zmax = 0;
	double Mmin = 0;
	double Mmax = 0;
	polyNum = num;
	Xmin = polygon->Box[0];
	Ymin = polygon->Box[1];
	Xmax = polygon->Box[2];
	Ymax = polygon->Box[3];
	double x1, x2, y1, y2;
	for (int i = 0; i < num; i++)
	{
		x1 = (polygon + i)->Box[0];
		y1 = (polygon + i)->Box[1];
		x2 = (polygon + i)->Box[2];
		y2 = (polygon + i)->Box[3];
		Xmin = Xmin > x1 ? x1 : Xmin;
		Ymin = Ymin > y1 ? y1 : Ymin;
		Xmax = Xmax > x2 ? Xmax : x2;
		Ymax = Ymax > y2 ? Ymax : y2;
	}
	std::string  filename = path;
	FILE *m_ShpFile_fp = fopen(filename.c_str(), "wb+");
	//ѭ��д��5�� ��������(int����)�����
	FileCode = OnChangeByteOrder(FileCode);
	fwrite(&FileCode, sizeof(int), 1, m_ShpFile_fp);

	for (i = 0; i < 5; i++)
	{
		fwrite(&Unused, sizeof(int), 1, m_ShpFile_fp);
	}
	FileLength = OnChangeByteOrder(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);

	fwrite(&Version, sizeof(int), 1, m_ShpFile_fp);
	fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);

	fwrite(&Xmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Xmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmax, sizeof(double), 1, m_ShpFile_fp);

	int RecordNum;
	int ContentLength;
	FileLength = 0;
	for (i = 1; i <= polyNum; i++)
	{
		RecordNum = OnChangeByteOrder(i);
		fwrite(&RecordNum, sizeof(int), 1, m_ShpFile_fp);
		int temp_Length = (polygon)->getLength();
		int ContentLength = temp_Length / 2;
		FileLength = FileLength + temp_Length + 8;

		ContentLength = OnChangeByteOrder(ContentLength);
		fwrite(&ContentLength, sizeof(int), 1, m_ShpFile_fp);

		fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);
		for (j = 0; j < 4; j++)
		{
			fwrite(&(polygon->Box[j]), sizeof(double), 1, m_ShpFile_fp);
		}
		fwrite(&(polygon->NumParts), sizeof(int), 1, m_ShpFile_fp);
		fwrite(&(polygon->NumPoints), sizeof(int), 1, m_ShpFile_fp);
		int PointNum;
		int start = 0;
		for (j = 0; j < polygon->NumParts; j++)
		{
			fwrite(&(polygon->Parts[j]), sizeof(int), 1, m_ShpFile_fp);
		}
		for (j = 0; j < polygon->NumPoints; j++)
		{
			fwrite(&(polygon->Points[j].positionX), sizeof(double), 1, m_ShpFile_fp);
			fwrite(&(polygon->Points[j].positionY), sizeof(double), 1, m_ShpFile_fp);
		}
		polygon++;
	}
	fseek(m_ShpFile_fp, 24, SEEK_SET);
	FileLength = FileLength / 2;
	FileLength = OnChangeByteOrder(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);

	fclose(m_ShpFile_fp);
	return 1;
}

int writeShpPoint(PolyPoint *polyPoint,int num,const string path) {
	// �Ȳ���д��Χ���ݵȱ�����������㷶Χ Ȼ����seek����Ӧλ��д������
		//��ڽ���PolyLine����
	int i, j;
	int polyNum = 10;
	//PolyPoint *polyPoint;
	int FileCode = 9994;
	int Unused = 0;
	int FileLength = 1000;
	int Version = 1000;
	int ShapeType = 1;
	int unUsed = 0;
	double Xmin = 300;
	double Ymin = 300;
	double Xmax = -300;
	double Ymax = -300;
	double Zmin = 0;
	double Zmax = 0;
	double Mmin = 0;
	double Mmax = 0;
	polyNum = num;
	//polyPoint = (PolyPoint *)malloc(sizeof(PolyPoint)*polyNum);
	for (int i = 0; i < polyNum; i++)
	{
		double x = (polyPoint + i)->positionX;
		double y = (polyPoint + i)->positionY;
		Xmin = Xmin > x ? x : Xmin;
		Ymin = Ymin > y ? y : Ymin;
		Xmax = Xmax > x ? Xmax : x;
		Ymax = Ymax > y ? Ymax : y;
	}
	std::string  filename = path;
	FILE *m_ShpFile_fp = fopen(filename.c_str(), "wb+");
	//ѭ��д��5�� ��������(int����)�����
	FileCode = OnChangeByteOrder(FileCode);
	fwrite(&FileCode, sizeof(int), 1, m_ShpFile_fp);

	for (i = 0; i < 5; i++)
	{
		fwrite(&Unused, sizeof(int), 1, m_ShpFile_fp);
	}
	FileLength = OnChangeByteOrder(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);

	fwrite(&Version, sizeof(int), 1, m_ShpFile_fp);
	fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);

	fwrite(&Xmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Xmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmax, sizeof(double), 1, m_ShpFile_fp);

	int RecordNum;
	int ContentLength;
	FileLength = 0;
	for (i = 1; i <= polyNum; i++)
	{
		RecordNum = OnChangeByteOrder(i);
		fwrite(&RecordNum, sizeof(int), 1, m_ShpFile_fp);
		int temp_Length = (polyPoint + i - 1)->getLength();
		int ContentLength = temp_Length / 2;
		FileLength = FileLength + temp_Length + 8;

		ContentLength = OnChangeByteOrder(ContentLength);
		fwrite(&ContentLength, sizeof(int), 1, m_ShpFile_fp);

		fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);
		fwrite(&((polyPoint + i -1)->positionX), sizeof(double), 1, m_ShpFile_fp);
		fwrite(&((polyPoint + i -1)->positionY), sizeof(double), 1, m_ShpFile_fp);
	}
	fseek(m_ShpFile_fp, 24, SEEK_SET);
	FileLength = FileLength / 2;
	FileLength = OnChangeByteOrder(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);
	fclose(m_ShpFile_fp);
	return 1;
}

int writeShp(PolyLine *polyLine,int num,const string path) {
	//�Ȳ���д��Χ���ݵȱ�����������㷶Χ Ȼ����seek����Ӧλ��д������
	//��ڽ���PolyLine����
	int i, j;
	int polyNum = 10;
	int FileCode = 9994;
	int Unused = 0;
	int FileLength = 1000;
	int Version = 1000;
	int ShapeType = 3;
	int unUsed = 0;
	double Xmin;
	double Ymin;
	double Xmax;
	double Ymax;
	double Zmin = 0;
	double Zmax = 0;
	double Mmin = 0;
	double Mmax = 0;
	polyNum = num;
	Xmin = polyLine->Box[0];
	Ymin = polyLine->Box[1];
	Xmax = polyLine->Box[2];
	Ymax = polyLine->Box[3];
	
	std::string  filename = path;
	FILE *m_ShpFile_fp = fopen(filename.c_str(), "wb+");
	//ѭ��д��5�� ��������(int����)�����
	FileCode = OnChangeByteOrder(FileCode);
	fwrite(&FileCode, sizeof(int), 1, m_ShpFile_fp);

	for (i = 0; i < 5; i++)
	{
		fwrite(&Unused, sizeof(int), 1, m_ShpFile_fp);
	}
	FileLength = OnChangeByteOrder(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);

	fwrite(&Version, sizeof(int), 1, m_ShpFile_fp);
	fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);

	fwrite(&Xmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Xmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Ymax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Zmax, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmin, sizeof(double), 1, m_ShpFile_fp);
	fwrite(&Mmax, sizeof(double), 1, m_ShpFile_fp);

	int RecordNum;
	int ContentLength;
	FileLength = 0;
	for (i = 1; i <= polyNum; i++)
	{
		RecordNum = OnChangeByteOrder(i);
		fwrite(&RecordNum, sizeof(int), 1, m_ShpFile_fp);
		int temp_Length = (polyLine)->getLength();
		int ContentLength = temp_Length / 2;
		FileLength = FileLength + temp_Length + 8;

		ContentLength = OnChangeByteOrder(ContentLength);
		fwrite(&ContentLength, sizeof(int), 1, m_ShpFile_fp);

		fwrite(&ShapeType, sizeof(int), 1, m_ShpFile_fp);
		for (j = 0; j < 4; j++)
		{
			fwrite(&(polyLine->Box[j]), sizeof(double), 1, m_ShpFile_fp);
		}
		fwrite(&(polyLine->NumParts), sizeof(int), 1, m_ShpFile_fp);
		fwrite(&(polyLine->NumPoints), sizeof(int), 1, m_ShpFile_fp);
		int PointNum;
		int start = 0;
		for (j = 0; j < polyLine->NumParts; j++)
		{
			fwrite(&(polyLine->Parts[j]), sizeof(int), 1, m_ShpFile_fp);
		}
		for (j = 0; j < polyLine->NumPoints; j++)
		{
			printf("д������ %lf , %lf \n", polyLine->Points[j].positionX, polyLine->Points[j].positionY);
			fwrite(&(polyLine->Points[j].positionX), sizeof(double), 1, m_ShpFile_fp);
			fwrite(&(polyLine->Points[j].positionY), sizeof(double), 1, m_ShpFile_fp);
		}
		polyLine++;
	}
	fseek(m_ShpFile_fp, 24, SEEK_SET);
	FileLength = FileLength / 2;
	FileLength = OnChangeByteOrder(FileLength);
	fwrite(&FileLength, sizeof(int), 1, m_ShpFile_fp);

	fclose(m_ShpFile_fp);
	return 1;
}

string cc(const string path)
{
	//****�������ļ�
/*	CFileDialog fDLG(true);
	if (fDLG.DoModal() != IDOK)
		return;
	filename = fDLG.GetPathName();*/

	std::string filename = path;
	//std::string filename = "C:\Users\ThemSun\Documents\Tencent Files\1985848358\FileRecv\MobileFile\point.shp";
	FILE* m_ShpFile_fp = fopen(filename.c_str(), "rb+");

	if (m_ShpFile_fp == NULL) {
		//MessageBox("Open File Failed");
		std::cout << "�ļ�Ϊ��" << std::endl;
		exit(0);
	}
	Json::Value root;
	Json::Value poly;
	//****�����ȡ�����ļ�ͷ�ı���
	int i;
	int j;
	int FileCode = -1;
	int Unused = -1;
	int FileLength = -1;
	int Version = -1;
	int ShapeType = -1;
	int ccdd = 0x0008c5fa;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
	unsigned char a4;
	double Xmin;
	double Ymin;
	double Xmax;
	double Ymax;
	double Zmin;
	double Zmax;
	double Mmin;
	double Mmax;
	std::cout << ccdd << std::endl;

	//****��ȡ����ͷ�ļ�
	std::cout << "��ʼ��ȡͷ�ļ�" << std::endl;
	fread(&FileCode, sizeof(int), 1, m_ShpFile_fp);  //��m_ShpFile_fp�����ֵ����Filecode����ȥ��ÿ�ζ�һ��int���ֽڵĳ��ȣ���ȡһ�� 
	FileCode = OnChangeByteOrder(FileCode);          //����ȡ��FileCode��ֵת��Ϊʮ���Ƶ��� 
	for (i = 0; i < 5; i++) {
		fread(&Unused, sizeof(int), 1, m_ShpFile_fp);\
	}
	fread(&FileLength, sizeof(int), 1, m_ShpFile_fp);
	FileLength = OnChangeByteOrder(FileLength);      //��FileLengthת��Ϊʮ���Ƶ��� 
	fread(&Version, sizeof(int), 1, m_ShpFile_fp);   //��ȡVersion��ֵ
	fread(&ShapeType, sizeof(int), 1, m_ShpFile_fp);//��ȡShapeType��ֵ 

	fread(&Xmin, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Xmin����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	fread(&Ymin, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Ymin����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	fread(&Xmax, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Xmax����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	fread(&Ymax, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Ymax����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 

	/*CRect rect(Xmin, Ymin, Xmax, Ymax);
	layer->setRect(rect);                         //����ͼ��ı߽�*/

	fread(&Zmin, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Zmin����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	fread(&Zmax, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Zmax����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	fread(&Mmin, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Mmin����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	fread(&Mmax, sizeof(double), 1, m_ShpFile_fp);//��m_ShpFile_fp�����ֵ����Mmax����ȥ��ÿ�ζ�ȡһ��double���ֽڳ��ȣ���ȡһ�� 
	//****��ȡ�����ļ�ͷ�����ݽ���

	//****��ȡ��״Ŀ���ʵ����Ϣ
	int RecordNumber;
	int ContentLength;
	int num = 0;
	std::cout << "ͷ����ȡ��� ShapeType = " << ShapeType << std::endl;

	switch (ShapeType) {
	case 5: {  //polygon
		std::cout << "���ΪPolygon" << std::endl;
		while ((fread(&RecordNumber, sizeof(int), 1, m_ShpFile_fp) != 0)) { //�ӵ�һ����ʼѭ����ȡÿһ��Polygon

			poly["type"] = 5;
			Json::Value datas;

			fread(&ContentLength, sizeof(int), 1, m_ShpFile_fp);            //��ȡContentLength
			RecordNumber = OnChangeByteOrder(RecordNumber);                 //ת��Ϊ10����
			ContentLength = OnChangeByteOrder(ContentLength);
			//****��¼ͷ��ȡ����
			printf("number = %d,contentLength = %d\n", RecordNumber, ContentLength);
			//****��ȡ��¼����
			int shapeType;   //��ǰPolygon��������
			double Box[4];   //��ǰPolygon���������ұ߽�
			int NumParts;    //��ǰPolygon���������ӻ��ĸ���
			int NumPoints;   //��ǰPolygon�������ĵ�ĸ���
			int *Parts;      //��ǰPolygon���������ӻ��������NumPoints�ı��
			fread(&shapeType, sizeof(int), 1, m_ShpFile_fp);
			printf("ShapType = %d", shapeType);
			for (i = 0; i < 4; i++) {                         //��Box
				fread(Box + i, sizeof(double), 1, m_ShpFile_fp);
				printf("Box[%d] =  %lf \b", i, *(Box + i));
			}
			fread(&NumParts, sizeof(int), 1, m_ShpFile_fp); //��ʾ���ɵ�ǰPolygon���ӻ��ĸ���
			fread(&NumPoints, sizeof(int), 1, m_ShpFile_fp);//��ʾ���ɵ�ǰPolygon����������������
			printf("NumParts = %d NumPoints = %d \n", NumParts, NumPoints);
			Parts = new int[NumParts];                      //��¼��ÿ���ӻ���������Points�����е���ʼλ��
			for (i = 0; i < NumParts; i++)
				fread(Parts + i, sizeof(int), 1, m_ShpFile_fp);

			int pointNum;
			//CGeoPolygon* polygon = new CGeoPolygon();
			//polygon->circleNum = NumParts;                   //�趨����εĵ���

			//****��ȡ�����ӻ�
			for (i = 0; i < NumParts; i++) {
				if (i != NumParts - 1)  pointNum = Parts[i + 1] - Parts[i];//ÿ���ӻ��ĳ��� �������һ����
				else  pointNum = NumPoints - Parts[i];       //���һ����
				double* PointsX = new double[pointNum];      //���ڴ�Ŷ�ȡ�ĵ�����xֵ;
				double* PointsY = new double[pointNum];      //���ڴ��yֵ
				//CGeoPolyline* polyline = new CGeoPolyline(); //ÿ����ʵ���Ͼ�����β������ͬ��Polyline
				//polyline->circleID = i;

				Json::Value item;
				for (int j = 0; j < pointNum; j++) {
					fread(PointsX + j, sizeof(double), 1, m_ShpFile_fp);
					fread(PointsY + j, sizeof(double), 1, m_ShpFile_fp);
					double a = PointsX[j];
					double b = PointsY[j];
					item["x"] = a;
					item["y"] = b;
					datas.append(item);
					//CPoint* point = new CPoint(a, b);
					//polyline->AddPoint(point);               //�Ѹ��ӻ����еĵ���ӵ�һ������
					std::cout << "���� a = " << a << " , " << b << std::endl;
				}

				/*CPoint pt1 = polyline->pts[0]->GetCPoint();
				CPoint pt2 = polyline->pts[polyline->pts.size() - 1]->GetCPoint();
				if (pt1 != pt2) {  //����λ�㲻һ��
					CString str;
					str.Format("%d������β�㲻һ��", RecordNumber);
					polyline->pts.push_back(p1);
				}*/

				//polygon->AddCircle(polyline);                 //��polyline����ӵ���Ӧpolygon��
				delete[] PointsX;
				delete[] PointsY;
			}
			//****һ�����ĳ���ӻ�ѭ��������ͬʱ���ӻ��Ѽ��뵽polygon

			//layer->AddObject((CGeoObject*)polygon);           //����polygon���뵽ͼ����
			delete[] Parts;
			poly["datas"] = datas;
			root.append(poly);
		}
	}
			break;
	case 1://point
		while ((fread(&RecordNumber, sizeof(int), 1, m_ShpFile_fp) != 0))
		{
			poly["type"] = 1;
			Json::Value datas;
			fread(&ContentLength, sizeof(int), 1, m_ShpFile_fp);
			RecordNumber = OnChangeByteOrder(RecordNumber);
			ContentLength = OnChangeByteOrder(ContentLength);
			int shapeType;
			fread(&shapeType, sizeof(int), 1, m_ShpFile_fp);
			double PointsX;
			double PointsY;
			Json::Value item;
			fread(&PointsX, sizeof(double), 1, m_ShpFile_fp);
			fread(&PointsY, sizeof(double), 1, m_ShpFile_fp);
			item["x"] = PointsX;
			item["y"] = PointsY;
			datas.append(item);
			poly["datas"] = datas;
			root.append(poly);
			printf("��ȡ�ľ�γ��%lf,%lf \n ", PointsX, PointsY);
		}
		break;
	case 3://polyline
	{
		while ((fread(&RecordNumber, sizeof(int), 1, m_ShpFile_fp) != 0))
		{
			poly["type"] = 3;
			Json::Value datas;
			fread(&ContentLength, sizeof(int), 1, m_ShpFile_fp);
			RecordNumber = OnChangeByteOrder(RecordNumber);
			ContentLength = OnChangeByteOrder(ContentLength);
			//std::cout << "Number = " << RecordNumber << "Length = " << ContentLength << std::endl;
			int shapeType;
			double Box[4];
			int NumParts;
			int NumPoints;
			int *Parts;
			fread(&shapeType, sizeof(int), 1, m_ShpFile_fp);
			//��Box
			for (i = 0; i < 4; i++)
				fread(Box + i, sizeof(double), 1, m_ShpFile_fp);
			//��NumParts��NumPoints
			fread(&NumParts, sizeof(int), 1, m_ShpFile_fp);
			fread(&NumPoints, sizeof(int), 1, m_ShpFile_fp);
			std::cout << "NumParts = " << NumParts << " , NumPoints = " << NumPoints << std::endl;
			//��Parts��Points
			Parts = new int[NumParts];
			for (i = 0; i < NumParts; i++) {
				fread(Parts + i, sizeof(int), 1, m_ShpFile_fp);
				printf("Parts[%d] = %d\n",i,*(Parts + i));
			}
			int pointNum;
			for (i = 0; i < NumParts; i++)
			{
				if (i != NumParts - 1)
					pointNum = Parts[i + 1] - Parts[i];
				else
					pointNum = NumPoints - Parts[i];
				double *PointsX;
				double *PointsY;

				PointsX = new double[pointNum];
				PointsY = new double[pointNum];
				printf("pointNum = %d \n", pointNum);
				Json::Value item;
				for (j = 0; j < pointNum; j++)
				{
					fread(PointsX + j, sizeof(double), 1, m_ShpFile_fp);
					fread(PointsY + j, sizeof(double), 1, m_ShpFile_fp);
					item["x"] = *(PointsX + j);
					item["y"] = *(PointsY + j);
					datas.append(item);
					printf("��γ������ %lf , %lf \n", *(PointsX + j),*(PointsY + j));
				}
				delete[] PointsX;
				delete[] PointsY;
			}
			delete[] Parts;
			poly["datas"] = datas;
			root.append(poly);
		}
	}
		break;
	default:
		break;
	}

	Json::FastWriter writer;
	string jsonDadta = writer.write(root);
	cout << "ת��Json����Ϊ" << jsonDadta;
	return jsonDadta;
}

int writeShpFromJson(const string jsonInfo,int type,const string path) {
	Polygongon *polygons;
	int num = readPolygon(jsonInfo, polygons);
	int writeStatus = -1;
	switch (type)
	{
	case 1: {
		PolyPoint *points;
		int num = readPoint(jsonInfo,points);
		writeStatus = writeShpPoint(points,num,path);
		break;
	}
	case 3: {
		PolyLine *polyLines;
		int num = read(jsonInfo,polyLines);
		writeStatus = writeShp(polyLines, num, path);
		break;
	}
	case 5: {
		Polygongon *polygons;
		int num = readPolygon(jsonInfo,polygons);
		writeStatus = writeShpPolygon(polygons, num, path);
		break;
	}
	default:
		break;
	}
	return writeStatus;
}

string readShpToJson(const string path) {
	string jsonData = cc(path);
	cout << "�������JSon���� : " << jsonData << endl;
	return jsonData;
}

