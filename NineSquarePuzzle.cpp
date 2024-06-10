#include"NineSquarePuzzle.h"


Mat ImgPuzzles::img = Mat();
Mat ImgPuzzles::PuzzlesMat = Mat();
vector<vector<CutMat*>> ImgPuzzles::CutImgs = vector<vector<CutMat*>>();
vector<vector<Point>> ImgPuzzles::CutContours = vector<vector<Point>>();
CutMat* ImgPuzzles::finalCutMat=nullptr;
int ImgPuzzles::cols=3;
int ImgPuzzles::rows=3;
int ImgPuzzles::rectwidth=0;
int ImgPuzzles::rectheight=0;
int ImgPuzzles::state=0;
double ImgPuzzles::PuzzleUseTime=0;
vector<int> ImgPuzzles::restorPath=vector<int>();
vector<int> CalcReverserNum::tempvct;


void ImgPuzzles::SplitMats(Mat& _img, int _cols, int _rows)
{
	assert(cols > 0);
	assert(rows > 0);
	img = _img; cols = _cols; rows = _rows;
	rectwidth = img.cols / cols; rectheight = img.rows / rows;
	state = 0;

	int px, py;
	vector<int> nums = GetVctPos(cols, rows);
	vector<int> reverseNums;
	CutImgs = vector<vector<CutMat*>>(rows, vector<CutMat*>(cols, nullptr));

	for (size_t i = 0; i < rows; i++)
	{
		py = i * rectheight;
		for (size_t j = 0; j < cols; j++)
		{
			px = j * rectwidth;
			//切片
			Mat tempMat = Mat(img, Rect(Point(px, py), Size(rectwidth, rectheight)));
			CutMat* tempCutMat = new CutMat(tempMat, i * 3 + j + 1);
			//根据随机数将CutMat存入二维切片图形组
			if (i == rows - 1 && j == cols - 1)
			{
				CutImgs[i][j] = tempCutMat;
				continue;
			}
			tempCutMat->curposition = GetRandvct(nums);
			int rcol = (tempCutMat->curposition - 1) % cols;
			int rrow = (tempCutMat->curposition - 1) / cols;
			CutImgs[rrow][rcol] = tempCutMat;
		}
	}
	//将此时随机序号存入数组reverseNums检验
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			reverseNums.push_back(CutImgs[i][j]->position);
		}
	}
	reverseNums.pop_back();
	int count = CalcReverserNum::MergeSort(reverseNums, 0, reverseNums.size() - 1);
	if (count % 2 != 0)
	{
		Mat* mat1 = &CutImgs[rows - 2][0]->data;
		Mat* mat2 = &CutImgs[rows - 1][0]->data;
		size_t* pos1 = &CutImgs[rows - 2][0]->position;
		size_t* pos2 = &CutImgs[rows - 1][0]->position;
		std::swap(*mat1, *mat2);
		std::swap(*pos1, *pos2);
	}
}
void ImgPuzzles::CreatePuzzlesMat()
{
	PuzzlesMat = Mat(Size(cols * rectwidth + 20, (rows + 1) * rectheight + 20), CV_8UC3, Scalar(240, 240, 240));
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			Rect rect = Rect(10 + j * rectwidth, 10 + i * rectheight, rectwidth, rectheight);
			CutImgs[i][j]->data.copyTo(PuzzlesMat(rect));
			InsertContours(rect);
		}
	}
	Rect finalrect = Rect(10 + (cols - 1) * rectwidth, 10 + rows * rectheight, rectwidth, rectheight);
	Mat finalMat = Mat(Size(rectwidth, rectheight), CV_8UC3, Scalar(255, 255, 255));
	finalCutMat = new CutMat(finalMat, CutContours.size() + 1);
	finalCutMat->data.copyTo(PuzzlesMat(finalrect));
	InsertContours(finalrect);

	imshow("ImgPuzzles", PuzzlesMat);
	setMouseCallback("ImgPuzzles", OnMouse);
}
void ImgPuzzles::RestoreGame()
{
	PuzzleUseTime = getTickCount();
	vector<vector<int>> tempNums = vector<vector<int>>(rows, vector<int>(cols, 0));
	ANode* targetNode;
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			tempNums[i][j] = CutImgs[i][j]->position;
			if (tempNums[i][j] == 10)
			{
				A_start::IsNormal = 1;
				A_start::wpos = i * 3 + j + 1;
				A_start::wrow = i;
				A_start::wcol = j;
			}
		}
	}
	A_start Astar(tempNums);
	targetNode = Astar.search();

	getPath(targetNode);
	restor();
}


vector<int> ImgPuzzles::GetVctPos(int cols, int rows)
{
	vector<int> tempvct;
	size_t total = cols * rows;
	for (size_t i = 1; i < total; i++)
	{
		tempvct.push_back(i);
	}
	return tempvct;
}
int ImgPuzzles::GetRandvct(vector<int>& nums)
{
	srand((unsigned)time(NULL));
	int index = nums.size() == 1 ? 0 : rand() % (nums.size() - 1);
	int rand = nums[index];
	nums.erase(nums.begin() + index);
	return rand;
}
void ImgPuzzles::InsertContours(Rect& rect)
{
	vector<cv::Point> contour;
	contour.push_back(Point(rect.x, rect.y));
	contour.push_back(Point(rect.x + rect.width, rect.y));
	contour.push_back(Point(rect.x + rect.width, rect.y + rect.height));
	contour.push_back(Point(rect.x, rect.y + rect.height));
	CutContours.push_back(contour);
}
void ImgPuzzles::OnMouse(int event, int x, int y, int flags, void* userdata)
{
	static int mousePos1=0, mouseRows1=0, mouseCols1=0;
	static int mousePos2 = 0, mouseRows2 = 0, mouseCols2 = 0;
	if (event == EVENT_LBUTTONDOWN)
	{
		Point mousePoint(x, y);
		GetMousePostion(mousePoint, mouseRows1, mouseCols1, mousePos1);
	}
	else if (event == EVENT_LBUTTONUP)
	{
		Point mousePoint(x, y);
		GetMousePostion(mousePoint, mouseRows2, mouseCols2, mousePos2);
		if (mousePos1 == mousePos2) return;

		if (state == 0)
		{
			if (mousePos1 + mousePos2 == CutContours.size() * 2 - 1)
			{
				state = 1;
				PuzzleUseTime = getTickCount();
				ImageMove(mousePos1, mouseRows1, mouseCols1, mousePos2, mouseRows2, mouseCols2);
			}
			else
			{
				cout << "请从右下角开始！！！" << endl;
			}
		}
		else if (state == 1)
		{
			ImageMove(mousePos1, mouseRows1, mouseCols1, mousePos2, mouseRows2, mouseCols2);
			if (CheckFinished(CutImgs))
			{
				state = 2;
				cout << "恭喜完成Puzzles!!!" << endl;
				cout << "用时:" << ((double)getTickCount() - PuzzleUseTime) / getTickFrequency() << "秒！" << endl;
			}
		}
		
	}
	else if (event == EVENT_RBUTTONDBLCLK)
	{
		SplitMats(img);
		CreatePuzzlesMat();
	}
	else if (event == EVENT_MBUTTONUP)
	{
		RestoreGame();
	}
}
void ImgPuzzles::ImageMove(int curposition, int rows1, int cols1, int newposition, int rows2, int cols2)
{
	if(curposition-newposition==1||newposition-curposition==1||newposition-curposition==3||curposition-newposition==3)
	{
		if (curposition == cols * rows + 1 || newposition == cols * rows + 1)
		{
			if (finalCutMat->position == CutContours.size() || CutImgs[rows - 1][cols - 1]->position == CutContours.size())
			{
				Mat* mat1 = &CutImgs[rows - 1][cols - 1]->data;
				Mat* mat2 = &finalCutMat->data;
				size_t* pos1 = &CutImgs[rows - 1][cols - 1]->position;
				size_t* pos2 = &finalCutMat->position;
				swap(*mat1, *mat2);
				swap(*pos1, *pos2);
				DrawPuzzleMat(curposition, newposition);
			}
		}
		else
		{
			if (CutImgs[rows1][cols1]->position == CutContours.size() || CutImgs[rows2][cols2]->position == CutContours.size())
			{
				Mat* mat1 = &CutImgs[rows1][cols1]->data;
				Mat* mat2 = &CutImgs[rows2][cols2]->data;
				size_t* pos1 = &CutImgs[rows1][cols1]->position;
				size_t* pos2 = &CutImgs[rows2][cols2]->position;
				swap(*mat1, *mat2);
				swap(*pos1, *pos2);
				DrawPuzzleMat(curposition, newposition);
			}
		}
	}
}
void ImgPuzzles::DrawPuzzleMat(int& curposition, int& newposition)
{
	vector<Point> tempContour1 = CutContours[curposition - 1];
	Rect tempRect1 = Rect(tempContour1[0], tempContour1[2]);
	Mat tempMat1 = PuzzlesMat(tempRect1);
	Mat temp;
	tempMat1.copyTo(temp);

	vector<Point> tempContour2 = CutContours[newposition - 1];
	Rect tempRect2 = Rect(tempContour2[0], tempContour2[2]);
	Mat tempMat2 = PuzzlesMat(tempRect2);

	tempMat2.copyTo(PuzzlesMat(tempRect1));
	temp.copyTo(PuzzlesMat(tempRect2));
	
	imshow("ImgPuzzles", PuzzlesMat);
}
void ImgPuzzles::GetMousePostion(Point& pt, int& _rows, int& _cols, int& curposition)
{
	for (int i = 0; i < CutContours.size(); i++)
	{
		if (pointPolygonTest(CutContours[i], pt,true) > 0)
		{
			if (i == CutContours.size() - 1)
			{
				_rows = rows + 1;
				_cols = cols;
				curposition = CutContours.size();
			}
			else
			{
				_rows = i / CutImgs[0].size();
				_cols = i % CutImgs[0].size();
				curposition = i + 1;
				break;
			}
		}
	}
}
bool ImgPuzzles::CheckFinished(vector<vector<CutMat*>>& _CutImgs)
{
	int target = 1;
	for (int _rows = 0; _rows < rows; ++_rows) {
		for (int _cols = 0; _cols < cols; ++_cols)
		{
			if (_CutImgs[_rows][_cols]->position != target) return false;
			target++;
			if (target == CutContours.size())
			{
				return true;
			}
		}
	}
}
void ImgPuzzles::getPath(ANode* targetNode)
{
	while (targetNode)
	{
		restorPath.push_back(targetNode->step);
		targetNode = targetNode->father;
	}
}
void ImgPuzzles::restor()
{
	int step, wposition = A_start::wpos, wrows = A_start::wrow, wcols = A_start::wcol;
	if (!A_start::IsNormal) ImageMove(wposition, wrows, wcols, 10, 3, 2);
	for (int i = restorPath.size() - 1; i >= 0; i--)
	{
		step = restorPath[i];
		if (step == 1)
		{
			ImageMove(wposition, wrows, wcols, wposition - 1, wrows, wcols - 1);
			wposition--; wcols--;
			waitKey(50);
		}
		else if (step == 2)
		{
			ImageMove(wposition, wrows, wcols, wposition + 1, wrows, wcols + 1);
			wposition++; wcols++;
			waitKey(50);
		}
		else if (step == 3)
		{
			ImageMove(wposition, wrows, wcols, wposition - 3, wrows - 1, wcols);
			wposition -= 3; wrows--;
			waitKey(50);
		}
		else if (step == 4)
		{
			ImageMove(wposition, wrows, wcols, wposition + 3, wrows + 1, wcols);
			wposition += 3; wrows++;
			waitKey(50);
		}
	}
	ImageMove(wposition, wrows, wcols, 10, 3, 2);
	if (CheckFinished(CutImgs))
	{
		cout << "恭喜完成Puzzles!!!" << endl;
		cout << "用时:" << ((double)getTickCount() - PuzzleUseTime) / getTickFrequency() << "秒！" << endl;
	}
}

int CalcReverserNum::MergeSort(vector<int>& vct, size_t left, size_t right)
{
	if (left >= right) return 0;
	//"分"
	int mid = left + (right - left) / 2;
	int leftnum = MergeSort(vct, left, mid);
	int rigthnum = MergeSort(vct, mid + 1, right);
	//"治"
	int nownum = merge(vct, left, mid, right);
	return leftnum + rigthnum + nownum;
}
int CalcReverserNum::merge(vector<int>& vct, int left, int mid, int right)
{
	tempvct.resize(vct.size());
	for (size_t i = left; i <= right; i++)
	{
		tempvct[i] = vct[i];
	}
	int i = left, j = mid + 1, k = left;
	int count = 0;
	while (i <= mid && j <= right)
	{
		if (tempvct[i] <= tempvct[j])
		{
			vct[k++] = tempvct[i++];
		}
		else
		{
			vct[k++] = tempvct[j++];
			count += mid - i + 1;
		}
	}
	while (i <= mid)
	{
		vct[k++] = tempvct[i++];
	}
	while (j<=right)
	{
		vct[k++] = tempvct[j++];
	}
	return count;
}

bool ReadImg(string FilePaths, vector<Mat>& imgs)
{
	vector<cv::String> file_mats;
	glob(FilePaths, file_mats);

	if (file_mats.size() <= 0)
	{
		cout << "找不到图片文件" << endl;
		waitKey(0);
		return false;
	}

	vector<cv::String>::iterator it;
	for (it = file_mats.begin(); it < file_mats.end(); it++)
	{
		imgs.push_back(imread(*it));
	}
	return true;
}
void ReSize(vector<Mat>& imgs, size_t cols, size_t rows)
{
	vector<Mat>::iterator it;
	for (it = imgs.begin(); it < imgs.end(); it++)
	{
		resize(*it, *it, Size(cols, rows));
	}
}