#include "PicReader.h"
#include "FastPrinter.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <ctime>
#pragma comment(lib,"winmm.lib")

/*
//	Picture class
*/
class Picture {
protected:
	PicReader imread;
	Array rgb_data;//rgba array (two-dimensional)
	size_t x, y;//Image x and y dimensions
	size_t x_scale, y_scale;//Image x and y scaling ratios
public:
	/****************************************
	Parameter: fname image file name
	Function: Read image and calculate grayscale value
	*****************************************/
	Picture(char *fname) {
		//Read image data
		cout << "Reading " << fname << ".....";
		imread.readPic(fname);
		rgb_data = imread.getData();

		//Image scaling
		x = rgb_data.shape[0];
		y = rgb_data.shape[1];
		x_scale = 1;
		y_scale = 1;
		double xmax = 160;
		double ymax = 200;
		if (x > xmax || y > ymax) {
			if (x / xmax > y / ymax) {
				x_scale = (size_t)ceil((double)x / xmax);
				x = x / x_scale;
				y_scale = x_scale;
				y = y / y_scale;
			}
			else {
				y_scale = (size_t)ceil((double)y / ymax);
				y = y / y_scale;
				x_scale = y_scale;
				x = x / x_scale;
			}
		}

		//Calculate RGB values 0-255 each multiplied by 0.299, 0.587, 0.114
		double ratio[256][3];
		for (int i = 0; i < 256; i++) {
			ratio[i][0] = i * 0.299;
			ratio[i][1] = i * 0.587;
			ratio[i][2] = i * 0.114;
		}

		//Convert RGB to grayscale, reshape rgb_data to two dimensions
		for (int i = 0; i < rgb_data.shape[0]; i += x_scale) {
			for (int j = 0; j < rgb_data.shape[1]; j += y_scale) {
				rgb_data[i][j][3] = int(ratio[rgb_data[i][j][0]][0] + ratio[rgb_data[i][j][1]][1] + ratio[rgb_data[i][j][2]][2]);
			}
		}
		cout << "Done" << endl;
	}

	/****************************************
	Parameter: style image style (0: black and white ASCII image; 1: foreground color ASCII image; 2: foreground and background color ASCII image)
	Function: Convert image to character string and display
	*****************************************/
	void show(int style) {
		cout << "Processing.....";
		//Divide grayscale into 15 levels
		char asciiStrength[] = { 'M','N','H','Q','$','O','C','?','7','>','!',':','-',';','.' };
		//char asciiStrength[] = "@%&M$#H8*X6ZQOUCL7>\![}(1|+;:-~'.";
		
		//Create buffer for image, allocate memory space
		char* charBuffer = new(nothrow)char[2 * x * y];
		BYTE* frontColorBuffer = new BYTE[2 * x * y * 3];
		BYTE* backColorBuffer = new BYTE[2 * x * y * 3];
		if (charBuffer == nullptr) {
			cout << "Character buffer memory allocation failed!" << endl;
			exit(1);
		}
		if (frontColorBuffer == nullptr) {
			cout << "Character buffer memory allocation failed!" << endl;
			exit(1);
		}
		if (backColorBuffer == nullptr) {
			cout << "Character buffer memory allocation failed!" << endl;
			exit(1);
		}

		//Convert grayscale values to characters
		for (size_t i = 0; i < x; i++) {
			for (size_t j = 0; j < y; j++) {
				size_t iscale = i * x_scale;
				size_t jscale = j * y_scale;
				size_t index = 6 * (i * y + j);
				unsigned char asciiIndex = (BYTE)rgb_data[iscale][jscale][3] / 18;
				memcpy(frontColorBuffer + index, rgb_data[iscale][jscale].data+rgb_data[iscale][jscale].index, 3);
				memcpy(frontColorBuffer + index + 3, frontColorBuffer + index, 3);
				//frontColorBuffer[index] = (BYTE)rgb_data[iscale][jscale][0];frontColorBuffer[index + 1] = (BYTE)rgb_data[iscale][jscale][1];
				//frontColorBuffer[index + 2] = (BYTE)rgb_data[iscale][jscale][2];frontColorBuffer[index + 3] = (BYTE)rgb_data[iscale][jscale][0];
				//frontColorBuffer[index + 4] = (BYTE)rgb_data[iscale][jscale][1];frontColorBuffer[index + 5] = (BYTE)rgb_data[iscale][jscale][2];
				charBuffer[2 * (i * y + j)] = charBuffer[2 * (i * y + j)+1] = asciiStrength[asciiIndex];
			}
		}
		cout << "Done" << endl;
		
		//Display image
		FastPrinter printer(2 * y, x, 5);
		if (style == 2) {
			memcpy(backColorBuffer, frontColorBuffer, 2 * x * y * 3);
		}
		printer.cleanSrceen();
		printer.setData(charBuffer, frontColorBuffer, backColorBuffer);
		printer.draw(style);
		char c = getchar();
		//Release memory space
		delete[] charBuffer;
		delete[] frontColorBuffer;
		delete[] backColorBuffer;
	}
};

/*
//	Video class
*/
class Video:protected Picture{
public:
	Video(char* filename):Picture(filename){}

	/****************************************
	Parameter: n number of frames
	Function: Convert video to character string and display
	*****************************************/
	void show(int n) {
		char asciiStrength[] = { 'M','N','H','Q','$','O','C','?','7','>','!',':','-',';','.' };
		//Allocate memory space
		char* charBuffer = new(nothrow)char[2 * x * y];
		BYTE* frontColorBuffer = new BYTE[2 * x * y * 3];
		BYTE* backColorBuffer = new BYTE[2 * x * y * 3];
		if (charBuffer == nullptr) {
			cout << "Character buffer memory allocation failed!" << endl;
			exit(1);
		}
		if (frontColorBuffer == nullptr) {
			cout << "Character buffer memory allocation failed!" << endl;
			exit(1);
		}
		if (backColorBuffer == nullptr) {
			cout << "Character buffer memory allocation failed!" << endl;
			exit(1);
		}
		//Set foreground color to black, background color to white
		memset(frontColorBuffer, 0, 2 * x * y * 3);
		memset(backColorBuffer, 255, 2 * x * y * 3);

		//Calculate RGB values 0-255 each multiplied by 0.299, 0.587, 0.114
		double ratio[256][3];
		for (int i = 0; i < 256; i++) {
			ratio[i][0] = i * 0.299;
			ratio[i][1] = i * 0.587;
			ratio[i][2] = i * 0.114;
		}

		//Set up printer
		FastPrinter printer(2 * y, x, 4);	
		//Play sound
		PlaySoundA("video\\1.wav", NULL, SND_FILENAME | SND_ASYNC);
		Sleep(1000);//Wait for sound to start

		//Loop to display each frame
		for (int ii = 1; ii <= n; ii++) {
			clock_t t_start = clock();
			char filename[] = { 'v','i','d','e','o','/','1',' ',char(ii / 100) + '0',char((ii % 100) / 10) + '0',char(ii % 10) + '0','.','j','p','g','\0' };
			imread.readPic(filename);
			rgb_data = imread.getData();

			//Calculate grayscale values
			for (size_t i = 0; i < (size_t)rgb_data.shape[0]; i += x_scale) {
				for (size_t j = 0; j < (size_t)rgb_data.shape[1]; j += y_scale) {
					//double sum = 0;
					//size_t i1[4] = { i,i,i + x_scale,i + x_scale };
					//size_t j1[4] = { j,j + y_scale,j,j + y_scale };
					//for (int k = 0; k < 4; k++) {
					//	if (i1[k] < x && j1[k] < y) {
					//		sum += int(ratio[rgb_data[i1[k]][j1[k]][0]][0] + ratio[rgb_data[i1[k]][j1[k]][1]][1] + ratio[rgb_data[i1[k]][j1[k]][2]][2]);
					//	}
					//	else{
					//		sum += int(ratio[rgb_data[i][j][0]][0] + ratio[rgb_data[i][j][1]][1] + ratio[rgb_data[i][j][2]][2]);
					//	}
					//}		
					//rgb_data[i][j][3] = int(sum / 4);
					rgb_data[i][j][3] = int(ratio[rgb_data[i][j][0]][0] + ratio[rgb_data[i][j][1]][1] + ratio[rgb_data[i][j][2]][2]);
				}
			}

			//Convert grayscale values to ASCII characters
			for (size_t i = 0; i < x; i++) {
				for (size_t j = 0; j < y; j++) {
					size_t iscale = i * x_scale;
					size_t jscale = j * y_scale;
					size_t index = 6 * (i * y + j);
					unsigned char asciiIndex = (BYTE)rgb_data[iscale][jscale][3] / 18;
					charBuffer[2 * (i * y + j)] = charBuffer[2 * (i * y + j) + 1] = asciiStrength[asciiIndex];
				}
			}
			//Display image
			printer.cleanSrceen();
			printer.setData(charBuffer, frontColorBuffer, backColorBuffer);
			printer.draw(0);

			//Calculate time
			size_t t = clock() - t_start;
			//If time exceeds 40ms, skip frames to maintain synchronization
			if (t > 50) {
				int k = t / 50;
				t %= 50;
				ii += k;
			}
			Sleep(50 - t);
		}
		delete[] charBuffer;
		delete[] frontColorBuffer;
		delete[] backColorBuffer;
	}
};
int main() {
	char fname[11][50] = { "classic_picture/airplane.jpg","classic_picture/baboon.jpg" ,"classic_picture/barbara.jpg" ,
		"classic_picture/cameraman.jpg","classic_picture/compa.png" ,"classic_picture/goldhill.jpg" ,"classic_picture/lena.jpg" ,
		"classic_picture/lena1.jpg" ,"classic_picture/milkdrop.jpg" ,"classic_picture/peppers.jpg","classic_picture/woman.jpg" };
	char videoname[] = "video/1 001.jpg";
	int choice;
	cout << "-----Welcome to Image to ASCII Converter-----" << endl;
	cout << "--------0.Display black and white ASCII image--------" << endl;
	cout << "------1.Display foreground color ASCII image------" << endl;
	cout << "--2.Display foreground and background color ASCII image--" << endl;
	cout << "-----------------------------------" << endl;	
	cout << "Please choose: ";
	cin >> choice;
	if (choice < 0 || choice>2) {
		cout << "Invalid input" << endl;
		return 0;
	}
	char c = getchar();
	//Display images
	for (int i = 0; i < 11; i++) {
		Picture p(fname[i]);
		p.show(choice);
	}
	
	Video v(videoname);
	v.show(389);
	return 0;
}