#include"AstarFindPsth.h"

bool A_start::IsNormal = 0;
int A_start::wpos = 9;
int A_start::wrow = 2;
int A_start::wcol = 2;

A_start::A_start(vector<vector<int>>& _startNums)
{
	ANode* start = new ANode(_startNums, 0, getH(_startNums), wpos, nullptr, 0);
	openlist.push_back(start);
}
ANode* A_start::search()
{
	int i, j, temp, rows, cols;
	vector<vector<int>> tempNums;
	ANode* checkNode = nullptr;
	ANode* tempNode = nullptr;
	while (1)
	{
		checkNode = openlist[0];
		if (checkNode->H == 0)
		{
			break;
		}

		closelist.push_back(checkNode);
		vector<ANode*>::iterator  it = find(openlist.begin(), openlist.end(), checkNode);
		openlist.erase(it);
		if ((checkNode->white - 1) % 3 >= 1)
		{
			tempNums = checkNode->cutnums;
			getWRC(checkNode->white, rows, cols);
			temp = tempNums[rows][cols - 1];
			tempNums[rows][cols - 1] = 9;
			tempNums[rows][cols] = temp;
			tempNode = new ANode(tempNums, checkNode->G + 1, getH(tempNums), checkNode->white - 1, checkNode, 1);
			ExistAndOperate(tempNode);
		}
		if ((checkNode->white - 1) % 3 <= 1)
		{
			tempNums = checkNode->cutnums;
			getWRC(checkNode->white, rows, cols);
			temp = tempNums[rows][cols + 1];
			tempNums[rows][cols + 1] = 9;
			tempNums[rows][cols] = temp;
			tempNode = new ANode(tempNums, checkNode->G + 1, getH(tempNums), checkNode->white + 1, checkNode, 2);
			ExistAndOperate(tempNode);
		}
		if (checkNode->white > 3)
		{
			tempNums = checkNode->cutnums;
			getWRC(checkNode->white, rows, cols);
			temp = tempNums[rows - 1][cols];
			tempNums[rows - 1][cols] = 9;
			tempNums[rows][cols] = temp;
			tempNode = new ANode(tempNums, checkNode->G + 1, getH(tempNums), checkNode->white - 3, checkNode, 3);
			ExistAndOperate(tempNode);
		}
		if (checkNode->white < 7)
		{
			tempNums = checkNode->cutnums;
			getWRC(checkNode->white, rows, cols);
			temp = tempNums[rows + 1][cols];
			tempNums[rows + 1][cols] = 9;
			tempNums[rows][cols] = temp;
			tempNode = new ANode(tempNums, checkNode->G + 1, getH(tempNums), checkNode->white + 3, checkNode, 4);
			ExistAndOperate(tempNode);
		}

		if (openlist.empty() == 1)
		{
			return nullptr;
		}
	}
	return checkNode;
}

int A_start::_abs(int num)
{
	if (num < 0) return (-num);
    return num;
}
int A_start::getH(vector<vector<int>>& _cutnums)
{
    int H = 0;
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
			if (_cutnums[i][j] == 9) continue;
            H += _abs((_cutnums[i][j] - 1) / 3 - i) + _abs((_cutnums[i][j] - 1) % 3 - j);
        }
    }
    return H;
}
void A_start::getWRC(int& white, int& wrows, int& wcols)
{
	wrows = (white - 1) / 3;
	wcols = (white - 1) % 3;
}
int A_start::exist(ANode* node)
{
	int i, j, k;
	int temp = 0;
	for (i = 0; i < openlist.size(); i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (node->cutnums[j][k] != openlist[i]->cutnums[j][k])
				{
					temp++;
				}
			}	
		}
		if (temp == 0)
		{
			return i+1;
		}
		temp = 0;
	}

	for (i = 0; i < closelist.size(); i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (node->cutnums[j][k] != closelist[i]->cutnums[j][k])
				{
					temp++;
				}
			}
		}
		if (temp == 0)
		{
			return (-i-1);
		}
		temp = 0;
	}

	return 0;
}
int A_start::OpenSearch(int x)
{
	int mid;
	int low = 0, high = openlist.size() - 1;
	while (low <= high)
	{
		mid = (low + high) / 2;
		if (openlist[mid]->F == x)
			return mid;
		if (openlist[mid]->F < x)
			low = mid + 1;
		if (openlist[mid]->F > x)
			high = mid - 1;
	}
	return low;
}
void A_start::Insert(ANode* newNode)
{
	int index = OpenSearch(newNode->F);
	auto it = openlist.begin();
	openlist.insert(it + index, newNode);
}
void A_start::ExistAndOperate(ANode* newNode)
{
	int i;
	int inList = exist(newNode);
	if (newNode->G == 1)
	{
		Insert(newNode);
		return;
	}

	if (inList == 0)
	{
		Insert(newNode);
	}
	else if (inList > 0)
	{
		if (openlist[inList - 1]->F > newNode->F)
		{
			auto it = openlist.begin();
			openlist.erase(it + inList - 1);
			Insert(newNode);
		}
	}
}