#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <opencv2\core\core.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

// 对给定的图像，去水印并返回二值图像
Mat First_Process(Mat &srcImage);
// 判断图像第一列与最后一列像素情况
string begin_end_white_condition(Mat Binary_srcImage);
//拼接函数，输入的是待拼接的文件名的容器，输出的是拼接后的图像
Mat stitch_seg_bag(vector<string> Stitch_images);

int main()
{
	string srcImages_path = "F:\\AppData\\PuShi\\Images\\";
	string save_result_path = "F:\\AppData\\PuShi\\Stitch_Seg_Bag\\Stitch_Seg_Bag\\result_Image\\";

	Directory dir;
	vector<string> filenames_path = dir.GetListFiles(srcImages_path, "*.jpg", true);
	vector<string> filenames_no_path = dir.GetListFiles(srcImages_path, "*.jpg", false);

	// 对文件名进行反排操作
	reverse(filenames_path.begin(), filenames_path.end());
	reverse(filenames_no_path.begin(), filenames_no_path.end());

	vector<string> stitch_images_names;		// 要拼接的图像名的容器
	// 遍历每张图像
	for (size_t i = 4; i < filenames_path.size(); i++)
	{
		Mat srcImage = imread(filenames_path[i]);
		Mat src_Binary = First_Process(srcImage);
		// 如果最后一列均为255，抛弃这张图像，直接进行下一张
		string temp = begin_end_white_condition(src_Binary);
		if (temp[1] == '1' && (src_Binary.cols < 185))
			continue;
		// 否则，继续对图像进行处理
		else
		{
			// 如果两侧都有白边出现，直接进行保存
			if (temp == "11")
			{
				imwrite(save_result_path + filenames_no_path[i], srcImage);
				continue;
			}
			// 否则，一直找到 “01” 的情况出现，并将这中间的所有图像名称进行保存
			// 再执行拼接函数
			else
			{
				// 建立用于拼接的图像名字的容器
				vector<string> stitch_filenames_path;		// 包含路径的名字容器
				vector<string> stitch_filenames_no_path;	// 不包含路径的名字容器
				string temp1 = temp;
				Mat tempImage, Binary_TempImage;
				int j = i;
				while (temp1 != "01")
				{
					stitch_filenames_path.push_back(filenames_path[j]);
					stitch_filenames_no_path.push_back(filenames_no_path[j]);
					tempImage = imread(filenames_path[j]);
					Binary_TempImage = First_Process(tempImage);
					temp1 = begin_end_white_condition(Binary_TempImage);
					j++;
				}
				// 将存下来的文件名进行拼接处理
				Mat stitch_result;
				// stitch_result = stitch_seg_bag(stitch_filenames_path);
				// imwrite(save_result_path + stitch_filenames_path[j], stitch_result);
				i = j-1;
				continue;
			}
		}



		int a = 0;
	}


	waitKey();
	return 0;
}

// 对给定的图像，去水印并返回二值图像
Mat First_Process(Mat &srcImage)
{
	Mat gray_srcImage;
	cvtColor(srcImage, gray_srcImage, CV_BGR2GRAY);
	int width_thresh = 185;	// 要去除的水印的宽度
	int height_thresh = 55;	// 要去除的水印的高度
	if (srcImage.cols < width_thresh)
		width_thresh = gray_srcImage.cols;
	// 还可以更精细一些
	rectangle(gray_srcImage, Rect(0, 0, width_thresh, 55), Scalar(255), CV_FILLED);
	rectangle(srcImage, Rect(0, 0, width_thresh, 55), Scalar(255, 255, 255), CV_FILLED);
	// 进行二值化
	Mat src_Binary;
	threshold(gray_srcImage, src_Binary, 200, 255, THRESH_BINARY);
	return src_Binary;
}

// 输入一张图像，判断第一列与最后一列是否均为白色
// 若第一列与最后一列均为白色，返回 '11'
// 若第一列不为白色，最后一列为白色，返回'01'
// 若第一列为白色，最后一列不为白色，返回'10'
// 若第一列与最后一列均不为白色，返回'00'
string begin_end_white_condition(Mat Binary_srcImage)
{
	string result;
	int sum_begin = 0;
	int sum_end = 0;
	for (int r = 0; r < Binary_srcImage.rows; r++)
	{
		sum_begin += Binary_srcImage.at<uchar>(r, 1);
		sum_end += Binary_srcImage.at<uchar>(r, Binary_srcImage.cols - 1);
	}
	bool beginn = 0;
	bool endd = 0;
	if (sum_begin == 255 * Binary_srcImage.rows)
		beginn = 1;
	if (sum_end == 255 * Binary_srcImage.rows)
		endd = 1;
	if (beginn && endd)
		return "11";
	if (beginn == 0 && endd == 0)
		return "00";
	if (beginn == 0 && endd == 1)
		return "01";
	else
		return "10";
}

//拼接函数，输入的是待拼接的文件名的容器，输出的是拼接后的图像
//Mat stitch_seg_bag(vector<string> Stitch_images)
//{
//
//}




























