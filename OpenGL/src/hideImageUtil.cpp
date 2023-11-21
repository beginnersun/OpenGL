#include <stb_image.h>
#include<fstream>
#include<iostream>
#include <io.h>

#include<stdio.h>
#include<string>
#include<atlstr.h>


void readPixel() {

	int width, height;
	int nrChannels;
	unsigned char *data = stbi_load("", &width, &height, &nrChannels, 0);
	/*
	data[w*n*x + y * n + n - 1] = alpha; alpha = n-1¸ö×Ö½Ú
	*/
}