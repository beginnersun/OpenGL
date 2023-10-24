#include "FrameBufferTest.h"
#include "cubeMap.h"
#include "SensorGLSL.h"
#include "asteroid.h"
#include "TsDecoder.h"

int main() {
	//main_framebuffer();
	//main_cubemap();
	//main_senior();
	main_decode_ts();
	//main_asteroid();
	return 1;
}

void testFun() {

	/*f

	std::string testFileName = "res/image/txt.txt";
	FILE *testFile;
	fopen_s(&testFile, testFileName.c_str(), "rb+");
	long cc;
	fread(&cc, sizeof(long), 1, testFile);
	std::cout << "cc = " << cc << std::endl;
	open_s(&testFile, testFileName.c_str(), "wb");
	long ac = 1024;
	fwrite(&ac, sizeof(ac), 1, testFile);
	fclose(testFile);*/
}