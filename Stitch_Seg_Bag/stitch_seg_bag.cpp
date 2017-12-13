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

//ƴ�Ӻ�����������Ǵ�ƴ�ӵ��ļ������������������ƴ�Ӻ��ͼ��
//Mat stitch_seg_bag(vector<string> Stitch_images)
//{
//
//}




























