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
				stitch_result = stitch_seg_bag(stitch_filenames_path);
				imwrite(save_result_path + stitch_filenames_path[j], stitch_result);
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

// 不去水印，将图像变为二值图像进行返回
Mat color_2_Binary(Mat srcImage)
{
	Mat gray_srcImage;
	cvtColor(srcImage, gray_srcImage, CV_BGR2GRAY);
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

// 两张图像的拼接函数
Mat Stitch_2_Images(Mat srcImage1, Mat srcImage2)
{
	srcImage2 = srcImage2(Rect(1, 0, srcImage2.cols - 1, srcImage2.rows));
	// 进行灰度化处理
	Mat grayImage1, grayImage2;
	cvtColor(srcImage1, grayImage1, CV_BGR2GRAY);
	cvtColor(srcImage2, grayImage2, CV_BGR2GRAY);

	// 对灰度图像进行特殊的阈值处理
	// 大于 230，不考虑，找到第一个小于230的像素
	int pos1 = 0;	// 记住第一张图里最后一列第一个小于230的像素
	for (int i = 0; i < grayImage1.rows; i++)
	{
		if (grayImage1.at<uchar>(i, grayImage1.cols - 1) < 230)
		{
			pos1 = i;
			break;
		}
		else
			continue;
	}
	int pos2 = 0;
	for (int j = 0; j < grayImage2.rows; j++)
	{
		if (grayImage2.at<uchar>(j, 0) < 230)
		{
			pos2 = j;
			break;
		}
		else
			continue;
	}

	// 利用这两个相对位置进行拼接，将这两个位置进行做差计算，求取拼接位置
	// 用第一张图像的 pos1 减去 第二张图像 的 pos2
	// 如果这个差值大于0，令第二张图像向下平移相对位移再进行拼接
	// 如果这个差值小于0，令第二张图像向上平移相对位移再进行拼接
	int diff = pos1 - pos2;
	// 首先求出总宽度
	int result_width = srcImage1.cols + srcImage2.cols;
	int result_height = 3000;	// 程序预设值
	Mat result_Image;
	result_Image.create(Size(result_width, result_height), CV_8UC3);
	if (diff >= 0)
	{
		// 将第一张图像的最左侧中心与result_Image 最左侧中心进行对齐
		// 然后再进行拼接操作
		
		// 确定高度的开始位置
		int start_row = 1500 - srcImage1.rows / 2;
		// 首先将第一张图复制到最终图像上
		for (int i = 0; i < srcImage1.rows; i++)
		{
			for (int j = 0; j < srcImage1.cols; j++)
			{
				result_Image.at<Vec3b>(start_row + i, j)[0] = srcImage1.at<Vec3b>(i, j)[0];
				result_Image.at<Vec3b>(start_row + i, j)[1] = srcImage1.at<Vec3b>(i, j)[1];
				result_Image.at<Vec3b>(start_row + i, j)[2] = srcImage1.at<Vec3b>(i, j)[2];
			}
		}
		// 再讲第二张图像根据相对位移复制到最终图像上
		for (int i = 0; i < srcImage2.rows; i++)
		{
			for (int j = 0; j < srcImage2.cols; j++)
			{

			}
		}

	}
	else
	{

	}



}







//拼接函数，输入的是待拼接的文件名的容器，输出的是拼接后的图像
Mat stitch_seg_bag(vector<string> Stitch_images)
{
	Mat result_Mat;
	int result_width = 0;	// 最后生成图像的总宽度
	for (size_t i = 0; i < Stitch_images.size(); i++)
	{
		Mat Temp_Image = imread(Stitch_images.at(i));
		result_width += Temp_Image.cols;
	}
	// 因为除了第一张图像外，之后的图像第一列均为白色
	result_width -= (Stitch_images.size() - 1);
	Mat Temp_Result = imread(Stitch_images[0]);
	//循环进行拼接操作
	for (size_t s = 1; s < Stitch_images.size(); s++)
	{
		Mat this_Image = imread(Stitch_images[s]);
		Temp_Result = Stitch_2_Images(Temp_Result, this_Image);
	}
	return Temp_Result;
}



























