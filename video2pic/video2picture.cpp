#include <iostream>
#include "cv.h"
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;


/*
* video2picture
* 
* created by yanxudong on 2018/3/1
*/

class video2pic
{
public:
	void videoPic()
	{
		// 获取视频文件  
		VideoCapture cap("F:\\CVproject\\100.avi");

		// 获取视频总帧数  
		long totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
		cout << "total frames: " << totalFrameNumber << endl;

		Mat frame;
		bool flags = true;
		long currentFrame = 0;
		int num = 0;

		while (flags) {
			// 读取视频每一帧  
			cap.read(frame);

			stringstream str;
			str << setfill('0') << setw(4) << num << ".jpg";
			cout << "正在处理第" << currentFrame << "帧" << endl;
			printf("\n");

			// 设置每30帧获取一次帧  
			if (currentFrame % 1 == 0) {
				// 将帧转成图片输出  
				imwrite("F:\\CVproject\\output\\" + str.str(), frame);
				num++;
			}
			// 结束条件  
			if (currentFrame >= totalFrameNumber) {
				flags = false;
			}
			currentFrame++;
		}

		//system("pause");
	}
};
/*
void main() {
	video2pic v;
	v.videoPic();
}
*/