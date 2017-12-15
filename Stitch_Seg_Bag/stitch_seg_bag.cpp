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

// �Ը�����ͼ��ȥˮӡ�����ض�ֵͼ��
Mat First_Process(Mat &srcImage);
// �ж�ͼ���һ�������һ���������
string begin_end_white_condition(Mat Binary_srcImage);
//ƴ�Ӻ�����������Ǵ�ƴ�ӵ��ļ������������������ƴ�Ӻ��ͼ��
Mat stitch_seg_bag(vector<string> Stitch_images);

int main()
{
	string srcImages_path = "F:\\AppData\\PuShi\\Images\\";
	string save_result_path = "F:\\AppData\\PuShi\\Stitch_Seg_Bag\\Stitch_Seg_Bag\\result_Image\\";

	Directory dir;
	vector<string> filenames_path = dir.GetListFiles(srcImages_path, "*.jpg", true);
	vector<string> filenames_no_path = dir.GetListFiles(srcImages_path, "*.jpg", false);

	// ���ļ������з��Ų���
	reverse(filenames_path.begin(), filenames_path.end());
	reverse(filenames_no_path.begin(), filenames_no_path.end());

	vector<string> stitch_images_names;		// Ҫƴ�ӵ�ͼ����������
	// ����ÿ��ͼ��
	for (size_t i = 4; i < filenames_path.size(); i++)
	{
		Mat srcImage = imread(filenames_path[i]);
		Mat src_Binary = First_Process(srcImage);
		// ������һ�о�Ϊ255����������ͼ��ֱ�ӽ�����һ��
		string temp = begin_end_white_condition(src_Binary);
		if (temp[1] == '1' && (src_Binary.cols < 185))
			continue;
		// ���򣬼�����ͼ����д���
		else
		{
			// ������඼�аױ߳��֣�ֱ�ӽ��б���
			if (temp == "11")
			{
				imwrite(save_result_path + filenames_no_path[i], srcImage);
				continue;
			}
			// ����һֱ�ҵ� ��01�� ��������֣��������м������ͼ�����ƽ��б���
			// ��ִ��ƴ�Ӻ���
			else
			{
				// ��������ƴ�ӵ�ͼ�����ֵ�����
				vector<string> stitch_filenames_path;		// ����·������������
				vector<string> stitch_filenames_no_path;	// ������·������������
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
				// �����������ļ�������ƴ�Ӵ���
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

// �Ը�����ͼ��ȥˮӡ�����ض�ֵͼ��
Mat First_Process(Mat &srcImage)
{
	Mat gray_srcImage;
	cvtColor(srcImage, gray_srcImage, CV_BGR2GRAY);
	int width_thresh = 185;	// Ҫȥ����ˮӡ�Ŀ��
	int height_thresh = 55;	// Ҫȥ����ˮӡ�ĸ߶�
	if (srcImage.cols < width_thresh)
		width_thresh = gray_srcImage.cols;
	// �����Ը���ϸһЩ
	rectangle(gray_srcImage, Rect(0, 0, width_thresh, 55), Scalar(255), CV_FILLED);
	rectangle(srcImage, Rect(0, 0, width_thresh, 55), Scalar(255, 255, 255), CV_FILLED);
	// ���ж�ֵ��
	Mat src_Binary;
	threshold(gray_srcImage, src_Binary, 200, 255, THRESH_BINARY);
	return src_Binary;
}

// ��ȥˮӡ����ͼ���Ϊ��ֵͼ����з���
Mat color_2_Binary(Mat srcImage)
{
	Mat gray_srcImage;
	cvtColor(srcImage, gray_srcImage, CV_BGR2GRAY);
	Mat src_Binary;
	threshold(gray_srcImage, src_Binary, 200, 255, THRESH_BINARY);
	return src_Binary;
}


// ����һ��ͼ���жϵ�һ�������һ���Ƿ��Ϊ��ɫ
// ����һ�������һ�о�Ϊ��ɫ������ '11'
// ����һ�в�Ϊ��ɫ�����һ��Ϊ��ɫ������'01'
// ����һ��Ϊ��ɫ�����һ�в�Ϊ��ɫ������'10'
// ����һ�������һ�о���Ϊ��ɫ������'00'
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

// ����ͼ���ƴ�Ӻ���
Mat Stitch_2_Images(Mat srcImage1, Mat srcImage2)
{
	srcImage2 = srcImage2(Rect(1, 0, srcImage2.cols - 1, srcImage2.rows));
	// ���лҶȻ�����
	Mat grayImage1, grayImage2;
	cvtColor(srcImage1, grayImage1, CV_BGR2GRAY);
	cvtColor(srcImage2, grayImage2, CV_BGR2GRAY);

	// �ԻҶ�ͼ������������ֵ����
	// ���� 230�������ǣ��ҵ���һ��С��230������
	int pos1 = 0;	// ��ס��һ��ͼ�����һ�е�һ��С��230������
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

	// �������������λ�ý���ƴ�ӣ���������λ�ý���������㣬��ȡƴ��λ��
	// �õ�һ��ͼ��� pos1 ��ȥ �ڶ���ͼ�� �� pos2
	// ��������ֵ����0����ڶ���ͼ������ƽ�����λ���ٽ���ƴ��
	// ��������ֵС��0����ڶ���ͼ������ƽ�����λ���ٽ���ƴ��
	int diff = pos1 - pos2;
	// ��������ܿ��
	int result_width = srcImage1.cols + srcImage2.cols;
	int result_height = 3000;	// ����Ԥ��ֵ
	Mat result_Image;
	result_Image.create(Size(result_width, result_height), CV_8UC3);
	if (diff >= 0)
	{
		// ����һ��ͼ��������������result_Image ��������Ľ��ж���
		// Ȼ���ٽ���ƴ�Ӳ���
		
		// ȷ���߶ȵĿ�ʼλ��
		int start_row = 1500 - srcImage1.rows / 2;
		// ���Ƚ���һ��ͼ���Ƶ�����ͼ����
		for (int i = 0; i < srcImage1.rows; i++)
		{
			for (int j = 0; j < srcImage1.cols; j++)
			{
				result_Image.at<Vec3b>(start_row + i, j)[0] = srcImage1.at<Vec3b>(i, j)[0];
				result_Image.at<Vec3b>(start_row + i, j)[1] = srcImage1.at<Vec3b>(i, j)[1];
				result_Image.at<Vec3b>(start_row + i, j)[2] = srcImage1.at<Vec3b>(i, j)[2];
			}
		}
		// �ٽ��ڶ���ͼ��������λ�Ƹ��Ƶ�����ͼ����
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







//ƴ�Ӻ�����������Ǵ�ƴ�ӵ��ļ������������������ƴ�Ӻ��ͼ��
Mat stitch_seg_bag(vector<string> Stitch_images)
{
	Mat result_Mat;
	int result_width = 0;	// �������ͼ����ܿ��
	for (size_t i = 0; i < Stitch_images.size(); i++)
	{
		Mat Temp_Image = imread(Stitch_images.at(i));
		result_width += Temp_Image.cols;
	}
	// ��Ϊ���˵�һ��ͼ���⣬֮���ͼ���һ�о�Ϊ��ɫ
	result_width -= (Stitch_images.size() - 1);
	Mat Temp_Result = imread(Stitch_images[0]);
	//ѭ������ƴ�Ӳ���
	for (size_t s = 1; s < Stitch_images.size(); s++)
	{
		Mat this_Image = imread(Stitch_images[s]);
		Temp_Result = Stitch_2_Images(Temp_Result, this_Image);
	}
	return Temp_Result;
}



























