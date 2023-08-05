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

//fread函数 读取int等类型数据时按照小端序读取  正常int解析数据按照大端序读取
//所以在读取到值之后需要对大小端序进行转换
unsigned long OnChangeByteOrder(int indata)
{
	unsigned char *c = (unsigned char*)&indata;
	int sum = 0;
	for (int i = 0; i < sizeof(int); i++)
	{
		sum += (*(c + i) << ((sizeof(int) - 1 - i) * 8));
	}
	//printf("转序后的结果 : %d \n",sum);
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
	double Box[4]; //当前目标线的范围  顺序 Xmin Ymin Xmax Ymax
	int NumParts; //当前线 子线段数量
	int NumPoints; //当前线段所有点的数量
	int *Parts; //保存每一个线段的开始点在Points中的下标
	PolyPoint *Points;//保存所有点数据

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
		cout << "数据格式有误" << endl;
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
		cout << "数据格式有误" << endl;
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
		cout << "数据格式有误！！！" << endl;
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
		case 3: {//3 代表是线
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
	//cout << "输出大小 " << polyLine->NumPoints << endl;
	//cout << "写入文件:" << status << endl;
	/*writeShp(polyLine);*/
	//cc();

	readShpToJson("D://testmian.shp");
	return 1;
}

int writeShpPolygon(Polygongon *polygon,int num,const string path) {
	// 先不填写范围数据等遍历结束后计算范围 然后再seek到对应位置写入数据
		//入口解析Polygon数据 注意环在解析的时候应该创建好
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
	//循环写入5个 无用数字(int类型)做填充
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
	// 先不填写范围数据等遍历结束后计算范围 然后再seek到对应位置写入数据
		//入口解析PolyLine数据
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
	//循环写入5个 无用数字(int类型)做填充
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
	//先不填写范围数据等遍历结束后计算范围 然后再seek到对应位置写入数据
	//入口解析PolyLine数据
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
	//循环写入5个 无用数字(int类型)做填充
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
			printf("写入数据 %lf , %lf \n", polyLine->Points[j].positionX, polyLine->Points[j].positionY);
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
	//****打开坐标文件
/*	CFileDialog fDLG(true);
	if (fDLG.DoModal() != IDOK)
		return;
	filename = fDLG.GetPathName();*/

	std::string filename = path;
	//std::string filename = "C:\Users\ThemSun\Documents\Tencent Files\1985848358\FileRecv\MobileFile\point.shp";
	FILE* m_ShpFile_fp = fopen(filename.c_str(), "rb+");

	if (m_ShpFile_fp == NULL) {
		//MessageBox("Open File Failed");
		std::cout << "文件为空" << std::endl;
		exit(0);
	}
	Json::Value root;
	Json::Value poly;
	//****定义读取坐标文件头的变量
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

	//****读取坐标头文件
	std::cout << "开始读取头文件" << std::endl;
	fread(&FileCode, sizeof(int), 1, m_ShpFile_fp);  //从m_ShpFile_fp里面的值读到Filecode里面去，每次读一个int型字节的长度，读取一次 
	FileCode = OnChangeByteOrder(FileCode);          //将读取的FileCode的值转化为十进制的数 
	for (i = 0; i < 5; i++) {
		fread(&Unused, sizeof(int), 1, m_ShpFile_fp);\
	}
	fread(&FileLength, sizeof(int), 1, m_ShpFile_fp);
	FileLength = OnChangeByteOrder(FileLength);      //将FileLength转化为十进制的数 
	fread(&Version, sizeof(int), 1, m_ShpFile_fp);   //读取Version的值
	fread(&ShapeType, sizeof(int), 1, m_ShpFile_fp);//读取ShapeType的值 

	fread(&Xmin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Xmin里面去，每次读取一个double型字节长度，读取一次 
	fread(&Ymin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Ymin里面去，每次读取一个double型字节长度，读取一次 
	fread(&Xmax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Xmax里面去，每次读取一个double型字节长度，读取一次 
	fread(&Ymax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Ymax里面去，每次读取一个double型字节长度，读取一次 

	/*CRect rect(Xmin, Ymin, Xmax, Ymax);
	layer->setRect(rect);                         //设置图层的边界*/

	fread(&Zmin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Zmin里面去，每次读取一个double型字节长度，读取一次 
	fread(&Zmax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Zmax里面去，每次读取一个double型字节长度，读取一次 
	fread(&Mmin, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Mmin里面去，每次读取一个double型字节长度，读取一次 
	fread(&Mmax, sizeof(double), 1, m_ShpFile_fp);//从m_ShpFile_fp里面的值读到Mmax里面去，每次读取一个double型字节长度，读取一次 
	//****读取坐标文件头的内容结束

	//****读取面状目标的实体信息
	int RecordNumber;
	int ContentLength;
	int num = 0;
	std::cout << "头部读取完毕 ShapeType = " << ShapeType << std::endl;

	switch (ShapeType) {
	case 5: {  //polygon
		std::cout << "结果为Polygon" << std::endl;
		while ((fread(&RecordNumber, sizeof(int), 1, m_ShpFile_fp) != 0)) { //从第一个开始循环读取每一个Polygon

			poly["type"] = 5;
			Json::Value datas;

			fread(&ContentLength, sizeof(int), 1, m_ShpFile_fp);            //读取ContentLength
			RecordNumber = OnChangeByteOrder(RecordNumber);                 //转换为10进制
			ContentLength = OnChangeByteOrder(ContentLength);
			//****记录头读取结束
			printf("number = %d,contentLength = %d\n", RecordNumber, ContentLength);
			//****读取记录内容
			int shapeType;   //当前Polygon几何类型
			double Box[4];   //当前Polygon的上下左右边界
			int NumParts;    //当前Polygon所包含的子环的个数
			int NumPoints;   //当前Polygon所包含的点的个数
			int *Parts;      //当前Polygon所包含的子环的起点在NumPoints的编号
			fread(&shapeType, sizeof(int), 1, m_ShpFile_fp);
			printf("ShapType = %d", shapeType);
			for (i = 0; i < 4; i++) {                         //读Box
				fread(Box + i, sizeof(double), 1, m_ShpFile_fp);
				printf("Box[%d] =  %lf \b", i, *(Box + i));
			}
			fread(&NumParts, sizeof(int), 1, m_ShpFile_fp); //表示构成当前Polygon的子环的个数
			fread(&NumPoints, sizeof(int), 1, m_ShpFile_fp);//表示构成当前Polygon所包含的坐标点个数
			printf("NumParts = %d NumPoints = %d \n", NumParts, NumPoints);
			Parts = new int[NumParts];                      //记录了每个子环的坐标在Points数组中的起始位置
			for (i = 0; i < NumParts; i++)
				fread(Parts + i, sizeof(int), 1, m_ShpFile_fp);

			int pointNum;
			//CGeoPolygon* polygon = new CGeoPolygon();
			//polygon->circleNum = NumParts;                   //设定多边形的点数

			//****读取面中子环
			for (i = 0; i < NumParts; i++) {
				if (i != NumParts - 1)  pointNum = Parts[i + 1] - Parts[i];//每个子环的长度 ，非最后一个环
				else  pointNum = NumPoints - Parts[i];       //最后一个环
				double* PointsX = new double[pointNum];      //用于存放读取的点坐标x值;
				double* PointsY = new double[pointNum];      //用于存放y值
				//CGeoPolyline* polyline = new CGeoPolyline(); //每个环实际上就是首尾坐标相同的Polyline
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
					//polyline->AddPoint(point);               //把该子环所有的点添加到一条链上
					std::cout << "坐标 a = " << a << " , " << b << std::endl;
				}

				/*CPoint pt1 = polyline->pts[0]->GetCPoint();
				CPoint pt2 = polyline->pts[polyline->pts.size() - 1]->GetCPoint();
				if (pt1 != pt2) {  //若首位点不一致
					CString str;
					str.Format("%d数据首尾点不一致", RecordNumber);
					polyline->pts.push_back(p1);
				}*/

				//polygon->AddCircle(polyline);                 //将polyline链添加到对应polygon中
				delete[] PointsX;
				delete[] PointsY;
			}
			//****一个面的某个子环循环结束，同时该子环已加入到polygon

			//layer->AddObject((CGeoObject*)polygon);           //将该polygon加入到图层中
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
			printf("读取的经纬度%lf,%lf \n ", PointsX, PointsY);
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
			//读Box
			for (i = 0; i < 4; i++)
				fread(Box + i, sizeof(double), 1, m_ShpFile_fp);
			//读NumParts和NumPoints
			fread(&NumParts, sizeof(int), 1, m_ShpFile_fp);
			fread(&NumPoints, sizeof(int), 1, m_ShpFile_fp);
			std::cout << "NumParts = " << NumParts << " , NumPoints = " << NumPoints << std::endl;
			//读Parts和Points
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
					printf("经纬度数据 %lf , %lf \n", *(PointsX + j),*(PointsY + j));
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
	cout << "转成Json数据为" << jsonDadta;
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
	cout << "解析后的JSon数据 : " << jsonData << endl;
	return jsonData;
}

