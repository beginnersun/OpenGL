#include<fstream>
#include<iostream>
#include <io.h>
#include"hideImageUtil.h"

#include<stdio.h>
#include<atlstr.h>

//#define STB_IMAGE_IMPLEMENTATION
#include"GLTextureImage.h"
#include<stb_image.h>
#include<stb_image_write.h>

/*
 * @params offset 当nrChannels==4时 值为0 1 2 3 分别代表rgba. 当nrChannels==3以此类推
 */
unsigned char getPixel(unsigned char *data, int i, int j, int width, int height, int nrChannels, int offset) {
	if (nrChannels <= 2 || offset > 3) {
		return 0;
	}
	int index = (i * width + j) * nrChannels + offset;
	return data[index];
}

/*
 * @params offset 当nrChannels==4时 值为0 1 2 3 分别代表rgba. 当nrChannels==3以此类推
 */
bool setPixel(unsigned char *data, int i, int j, int width, int height, int nrChannels, int offset, unsigned char pixelValue) {
	if (nrChannels <= 2 || offset > 3) {
		return false;
	}
	int index = (i * width + j) * nrChannels + offset;
	data[index] = pixelValue;
	return true;
}

/*
 得到混合后的隐藏图
*/
int getMixImage(string blackPath,string whitePath,string outPutPath) {

	//图片对应的宽高
	int blackWidth, blackHeight;
	//加载当前图片的色彩数 1 = 灰度; 2 = 黑白 ; 3 = RGB ; 4 = RGBA;
	int blackNrChannelds;
	unsigned char *blackData = stbi_load(blackPath.c_str(), &blackWidth, &blackHeight, &blackNrChannelds, 0);

	int whiteWidth,whiteHeight;
	int whiteNrChannelds;
	unsigned char *whiteData = stbi_load(whitePath.c_str(), &whiteWidth, &whiteHeight, &whiteNrChannelds, 0);

	if(blackWidth != whiteWidth || blackHeight != whiteHeight){
		stbi_image_free(whiteData);
		stbi_image_free(blackData);
		return -1;
	}
	if(whiteNrChannelds < 3){
		return -2;
	}

	//混合图 无论黑底或者白底是rgb还是rgba，一律取rgba
	int outPutWidth = whiteWidth;
	int outPutHeight = whiteHeight;
	int size = outPutWidth * outPutHeight * 4;
	unsigned char *mixImageData = new unsigned char[size];

	//黑底图，衰减亮度
	double attenuationBrightness = 0.4;
	int width = outPutWidth;
	int height = outPutHeight;
	for(int i = 0 ; i < width; i ++){
		for(int j = 0 ; j < height; j ++){
			// unsigned char r = data[]
			unsigned char whiteR = getPixel(whiteData, i, j, whiteWidth, whiteHeight, whiteNrChannelds, 0);
			unsigned char whiteG = getPixel(whiteData, i, j, whiteWidth, whiteHeight, whiteNrChannelds, 1);
			unsigned char whiteB = getPixel(whiteData, i, j, whiteWidth, whiteHeight, whiteNrChannelds, 2);
			int whiteGray = ((int)whiteR + (int)whiteG + (int)whiteB) / 3;

			unsigned char balckR = getPixel(blackData,i,j,blackWidth,blackHeight,blackNrChannelds,0);
			unsigned char blackG = getPixel(blackData,i,j,blackWidth,blackHeight,blackNrChannelds,1);
			unsigned char blackB = getPixel(blackData,i,j,blackWidth,blackHeight,blackNrChannelds,2);
			int blackGray = (int)(((int)balckR + (int)blackG + (int)blackB) / 3 * attenuationBrightness);

			if(whiteGray - blackGray < 0){
				double resultGray = blackGray / 255.0;
				double resultAlpha = 1.0;

				unsigned char gray = (unsigned char)(resultGray * 255.0);
				unsigned char alpha = (unsigned char)(resultAlpha * 255.0);
				setPixel(mixImageData,i,j,width,height,4,0,gray);
				setPixel(mixImageData,i,j,width,height,4,1,gray);
				setPixel(mixImageData,i,j,width,height,4,2,gray);
				setPixel(mixImageData,i,j,width,height,4,3,alpha);
			} else {
				double resultGray = (blackGray / 255.0) / (1 - (whiteGray / 255.0 - blackGray / 255.0));
				double resultAlpha = 1 - (whiteGray / 255.0 - blackGray / 255.0);

				unsigned char gray = (unsigned char)(resultGray * 255.0);
				unsigned char alpha = (unsigned char)(resultAlpha * 255.0);
				setPixel(mixImageData, i, j, width, height, 4, 0, gray);
				setPixel(mixImageData, i, j, width, height, 4, 1, gray);
				setPixel(mixImageData, i, j, width, height, 4, 2, gray);
				setPixel(mixImageData, i, j, width, height, 4, 3, alpha);
			}
		}
	}
	/*
	data[w*n*x + y * n + n - 1] = alpha; alpha = n-1个字节
	*/
	//stbi_write_png(outPutPath.c_str(),outPutWidth,outPutHeight,4,mixImageData,0);
	stbi_image_free(mixImageData);
	stbi_image_free(whiteData);
	stbi_image_free(blackData);
}