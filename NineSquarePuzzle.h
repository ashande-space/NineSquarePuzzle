#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<assert.h>
#include<cstdlib>
#include<ctime>
#include"AstarFindPsth.h"

using namespace cv;
using namespace std;

struct CutMat
{
	size_t position;
	size_t curposition;
	Mat data;

	CutMat(Mat& _img, size_t _pos)
		:data(_img)
		,position(_pos)
		,curposition(_pos)
	{

	}
};

class ImgPuzzles
{
public:
	static Mat img;
	static int state;
	static Mat PuzzlesMat;
	static vector<vector<CutMat*>> CutImgs;
	static vector<vector<Point>> CutContours;
	static double PuzzleUseTime;
	static void SplitMats(Mat& _img, int _cols = 3, int _rows = 3);
	static void CreatePuzzlesMat();
	static void RestoreGame();
private:
	
	static int cols;
	static int rows;
	static int rectwidth;
	static int rectheight;
	static CutMat* finalCutMat;
	static vector<int> restorPath;
	static vector<int> GetVctPos(int cols = 3, int rows = 3);
	static int GetRandvct(vector<int>& nums);
	static void InsertContours(Rect& rect);
	static void OnMouse(int event, int x, int y, int flags, void* param);
	static void ImageMove(int curposition, int rows1, int cols1, int newposition, int rows2, int cols2);
	static void DrawPuzzleMat(int& curposition, int& newposition);
	static void GetMousePostion(Point& pt, int& _rows, int& _cols, int& curposition);
	static bool CheckFinished(vector<vector<CutMat*>>& _CutImgs);
	static void getPath(ANode* targetNode);
	static void restor();
};

static class CalcReverserNum
{
public:
	static int MergeSort(vector<int>& vct, size_t left, size_t right);
	static int merge(vector<int>& vct, int left, int mid, int right);
private:
	static vector<int> tempvct;
};


bool ReadImg(string FilePaths, vector<Mat>& imgs);
void ReSize(vector<Mat>& imgs, size_t cols = 450, size_t rows = 450);
