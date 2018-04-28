
#include <cmath>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <queue>
#include <vector>
#include <time.h>
#include "myfft.h"
#include <sstream>

/*
* object detection by frequency
*
* created by yanxudong on 2018/4/1
*/

using namespace cv;
using namespace std;
#define row 8
#define col 10
#define Frame 20

class gray {

public:

	struct MyStruct
	{
		int num;

	};
	vector<vector<float>> rec;

	void storage(Mat pic) {
		int nr = pic.rows;
		int nc = pic.cols;
		nr = int(nr / row) * row;
		nc = int(nc / col) * col;
		int dr = nr / row;
		int dc = nc / col;

		resize(pic, pic, Size(nc, nr), 0, 0, CV_INTER_LINEAR);
		Mat pic1 = pic.clone();
		vector<float> r;
		vector<double> a;

		for (int i = 0; i <= nr - dr; i += dr) {
			for (int j = 0; j <= nc - dc; j += dc) {
				//Rect rect = Rect(j, i, j + dc, i + dr);
				//rectangle(pic, rect, Scalar(84, 255, 159), 1, 8, 0);
				float eve = grayPic(pic1, i, i + dr, j, j + dc);
				r.push_back(eve);
				//cout << eve << "\t";
			}
			//cout << endl;
		}
		//cout << endl;
		if (rec.size() >= Frame) {
			rec.erase(rec.begin());
		}
		rec.push_back(r);
		//todo 优化
		if (rec.size() == Frame) {
			a = fft();      //fft
		}
		if (a.size() == row*col)
			showPropab(pic, a);

		namedWindow("hh");
		imshow("hh", pic);
		waitKey(1);
	}

	void showPropab(Mat pic, vector<double> a) {
		
		int nr = pic.rows;
		int nc = pic.cols;
		nr = int(nr / row) * row;
		nc = int(nc / col) * col;
		int dr = nr / row;
		int dc = nc / col;

		double sumPro = 0;
		int max = 0;
		double maxnum = 0;
		
		for (int j = 0; j < row*col; j++) {
			sumPro += a[j];
			if (a[j] > maxnum){
				maxnum = a[j];
				max = j + 1;
			}
		}
		cout << max << endl;
		double pro = maxnum / sumPro;
		int c = max % col;
		int r = max / col + 1;
		int i = (r - 1) * dr;
		int j = (c - 1) * dc;
		paint(pic, pro, i, j);
		Rect rect = Rect(j, i, dc, dr);
		rectangle(pic, rect, Scalar(84, 255, 159), 1, 8, 0);
		//int count = 0;
		//double pro = 0;
		//for (int i = 0; i <= nr - dr; i += dr) {
		//	for (int j = 0; j <= nc - dc; j += dc) {
		//		if (a[count] == 0) {
		//			count++;
		//			continue;
		//		}
		//		pro = a[count] / sumPro;
		//		count++;
		//		stringstream ss;
		//		ss << setprecision(2) << pro;
		//		string s = ss.str();
		//		char *p = const_cast<char *>(s.c_str());
		//		cvPutText(pBinary, p, cvPoint(j, i+14), &font, CV_RGB(255, 0, 0));//在图片中输出字符 
		//	}
		//}
	}

	void paint(Mat pic, double pro, int i, int j) {
		CvFont font;
		double hScale = 0.5;
		double vScale = 0.5;
		int lineWidth = 1;// 相当于写字的线条

		IplImage* pBinary = &IplImage(pic);
		// 初始化字体   
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC,
			hScale, vScale, 0, lineWidth);//初始化字体，准备写到图片上的

		stringstream ss;
		ss << setprecision(2) << pro;
		string s = ss.str();
		char *p = const_cast<char *>(s.c_str());
		cvPutText(pBinary, p, cvPoint(j, i + 14), &font, CV_RGB(255, 0, 0));//在图片中输出字符 
	}

	vector<double> fft() {
		my_complex * in = new my_complex[Frame];
		my_complex * out = new my_complex[Frame];
		vector<double> propab;
		int hh = 0;
		for (int i = 0; i < row*col; i++) {
			float zero = 0.0;
			
			for (int j = 0; j < Frame; j++) {
				in[j].r = rec[j][i];
				in[j].i = 0;
				out[j].r = 0;
				out[j].i = 0;
				zero += rec[j][i];
			}
			//cout << zero << endl;
			if (zero != 0) {
				myfft_by_define_c2c_1d(in, Frame, out, Frame, 1, 0);
				double sum = 0;
				for (int count = 1; count < Frame; count++) {
					double magnitude = sqrt(pow(out[count].r, 2) + pow(out[count].i, 2));
					sum += magnitude;
				}
				propab.push_back(sum);
			}
			else {
				propab.push_back(0);
			}
			
		}
		//vector<double>::iterator h;
		//for (h = propab.begin(); h != propab.end(); h++) {
		//	float q = *h;
		//	//cout << q << endl;
		//}
		
		return propab;
	}

	void video2Pic()
	{
		// 获取视频文件  
		VideoCapture cap("F:\\CVproject\\300.avi");

		// 获取视频总帧数  
		long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
		double rate = cap.get(CV_CAP_PROP_FPS);
		//cout << "total frames: " << totalFrameNumber << endl;

		Mat frame;
		bool flags = true;
		long currentFrame = 0;
		int num = 0;
		float time = 0;
		int ff = 0;
		ofstream out("F:\\CVproject\\300.txt", ios::trunc);

		while (flags) {	
			// 结束条件  
			if (currentFrame >= totalFrameNumber) {
				break;
			}
			// 读取视频每一帧  
			cap.read(frame);
			if (ff >= 6) {
				ff = 0;
				//cvtColor(frame, frame, CV_BGR2GRAY);
				long start_time = GetTickCount();
				storage(frame);
				long end_time = GetTickCount();
				cout << "程序段运行时间：" << (end_time - start_time) << "ms!" << endl;
				//canny(frame);
				//int eve = grayPic(frame);
				//fstream(eve, time);
				time += 0.01;
				currentFrame++;
				continue;
			}
			ff++;
			
		}
		printf("rate=%f", rate);
		system("pause");
	}

	float grayPic(Mat src_gray, int r1, int r2, int c1, int c2)
	{
		int len = 0;
		int greySum = 0;
		cvtColor(src_gray, src_gray, CV_BGR2GRAY);

		for (int j = r1; j <= r2 - 1; j++) {
			uchar *data = src_gray.ptr<uchar>(j);
			for (int i = c1; i < c2 - 1; i++) {
				if (data[i] > 80){
					len++;
					greySum += data[i];
					//data[i] = 255;
				}
			}
		}
		float eve = greySum / (len+1);
		//printf("len=%d,Sum=%d,eve=%.5f,", len, greySum,eve);
		//printf("row=%d,column=%d,channel=%d", nr, nc,channel);
		//namedWindow("hh");
		//imshow("hh", src_gray);
		//waitKey();
		//return eve;
		return float(len);
	}

	void fstream(int eve, float time) {
		//ofstream out("F:\\CVproject\\fuqiang\\grey.txt", ios::trunc);
		ofstream outfile("F:\\CVproject\\fuqiang\\300.txt", ofstream::app);
		outfile << eve << "\t" << time << endl;
	}
	
};

void main() {
	gray g;
	g.video2Pic();
	system("pause");
}
