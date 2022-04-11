#include "PicReader.h"
#include "FastPrinter.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <ctime>
#pragma comment(lib,"winmm.lib")

/*
//	图片类
*/
class Picture {
protected:
	PicReader imread;
	Array rgb_data;//rgba数据(四维)
	size_t x, y;//图片x、y方向宽度
	size_t x_scale, y_scale;//图片x、y方向缩放比例
public:
	/****************************************
	参数：fname图片文件名
	功能：读取图片，并计算灰度值
	*****************************************/
	Picture(char *fname) {
		//读入图片数据
		cout << "正在读入" << fname << ".....";
		imread.readPic(fname);
		rgb_data = imread.getData();

		//图片缩放
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

		//提前计算RGB值0-255每个数乘0.299、0.587、0.114的值
		double ratio[256][3];
		for (int i = 0; i < 256; i++) {
			ratio[i][0] = i * 0.299;
			ratio[i][1] = i * 0.587;
			ratio[i][2] = i * 0.114;
		}

		//RGB转灰度值，存在rgb_data第四维中
		for (int i = 0; i < rgb_data.shape[0]; i += x_scale) {
			for (int j = 0; j < rgb_data.shape[1]; j += y_scale) {
				rgb_data[i][j][3] = int(ratio[rgb_data[i][j][0]][0] + ratio[rgb_data[i][j][1]][1] + ratio[rgb_data[i][j][2]][2]);
			}
		}
		cout << "完成" << endl;
	}

	/****************************************
	参数：style图片风格(0:无色ASCII图像;1:带前景色ASCII图像;2:带前景色和背景色ASCII图像)
	功能：将图片转为字符画并显示
	*****************************************/
	void show(int style) {
		cout << "正在处理.....";
		//灰度分为15级
		char asciiStrength[] = { 'M','N','H','Q','$','O','C','?','7','>','!',':','-',';','.' };
		//char asciiStrength[] = "@%&M$#H8*X6ZQOUCL7>\![}(1|+;:-~'.";
		
		//绘图buffer申请内存空间
		char* charBuffer = new(nothrow)char[2 * x * y];
		BYTE* frontColorBuffer = new BYTE[2 * x * y * 3];
		BYTE* backColorBuffer = new BYTE[2 * x * y * 3];
		if (charBuffer == nullptr) {
			cout << "字符数组申请内存空间失败！" << endl;
			exit(1);
		}
		if (frontColorBuffer == nullptr) {
			cout << "字符数组申请内存空间失败！" << endl;
			exit(1);
		}
		if (backColorBuffer == nullptr) {
			cout << "字符数组申请内存空间失败！" << endl;
			exit(1);
		}

		//灰度值转字符
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
		cout << "完成" << endl;
		
		//显示图片
		FastPrinter printer(2 * y, x, 5);
		if (style == 2) {
			memcpy(backColorBuffer, frontColorBuffer, 2 * x * y * 3);
		}
		printer.cleanSrceen();
		printer.setData(charBuffer, frontColorBuffer, backColorBuffer);
		printer.draw(style);
		char c = getchar();
		//释放内存空间
		delete[] charBuffer;
		delete[] frontColorBuffer;
		delete[] backColorBuffer;
	}
};

/*
//	视频类
*/
class Video:protected Picture{
public:
	Video(char* filename):Picture(filename){}

	/****************************************
	参数：n 图片数量
	功能：将视频转为字符画并显示
	*****************************************/
	void show(int n) {
		char asciiStrength[] = { 'M','N','H','Q','$','O','C','?','7','>','!',':','-',';','.' };
		//申请内存空间
		char* charBuffer = new(nothrow)char[2 * x * y];
		BYTE* frontColorBuffer = new BYTE[2 * x * y * 3];
		BYTE* backColorBuffer = new BYTE[2 * x * y * 3];
		if (charBuffer == nullptr) {
			cout << "字符数组申请内存空间失败！" << endl;
			exit(1);
		}
		if (frontColorBuffer == nullptr) {
			cout << "字符数组申请内存空间失败！" << endl;
			exit(1);
		}
		if (backColorBuffer == nullptr) {
			cout << "字符数组申请内存空间失败！" << endl;
			exit(1);
		}
		//设置前景色为黑色，背景色为白色
		memset(frontColorBuffer, 0, 2 * x * y * 3);
		memset(backColorBuffer, 255, 2 * x * y * 3);

		//提前计算RGB值0-255每个数乘0.299、0.587、0.114的值
		double ratio[256][3];
		for (int i = 0; i < 256; i++) {
			ratio[i][0] = i * 0.299;
			ratio[i][1] = i * 0.587;
			ratio[i][2] = i * 0.114;
		}

		//设置窗口
		FastPrinter printer(2 * y, x, 4);	
		//播放声音
		PlaySoundA("video\\1.wav", NULL, SND_FILENAME | SND_ASYNC);
		Sleep(1000);//声音播放有一定的滞后

		//遍历显示每一张图片
		for (int ii = 1; ii <= n; ii++) {
			clock_t t_start = clock();
			char filename[] = { 'v','i','d','e','o','/','1',' ',char(ii / 100) + '0',char((ii % 100) / 10) + '0',char(ii % 10) + '0','.','j','p','g','\0' };
			imread.readPic(filename);
			rgb_data = imread.getData();

			//计算灰度值
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

			//灰度值转换为ASCII字符
			for (size_t i = 0; i < x; i++) {
				for (size_t j = 0; j < y; j++) {
					size_t iscale = i * x_scale;
					size_t jscale = j * y_scale;
					size_t index = 6 * (i * y + j);
					unsigned char asciiIndex = (BYTE)rgb_data[iscale][jscale][3] / 18;
					charBuffer[2 * (i * y + j)] = charBuffer[2 * (i * y + j) + 1] = asciiStrength[asciiIndex];
				}
			}
			//显示图片
			printer.cleanSrceen();
			printer.setData(charBuffer, frontColorBuffer, backColorBuffer);
			printer.draw(0);

			//计算耗时
			size_t t = clock() - t_start;
			//如果耗时大于40ms，则进行跳过若干帧，使音画同步
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
	cout << "-----欢迎使用图像转ASCII小程序-----" << endl;
	cout << "--------0.显示无色ASCII图像--------" << endl;
	cout << "------1.显示带前景色ASCII图像------" << endl;
	cout << "--2.显示带前景色和背景色ASCII图像--" << endl;
	cout << "-----------------------------------" << endl;	
	cout << "请输入选择：";
	cin >> choice;
	if (choice < 0 || choice>2) {
		cout << "输入错误！" << endl;
		return 0;
	}
	char c = getchar();
	//显示图片
	for (int i = 0; i < 11; i++) {
		Picture p(fname[i]);
		p.show(choice);
	}
	
	Video v(videoname);
	v.show(389);
	return 0;
}