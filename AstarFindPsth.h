#pragma once
#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;

struct ANode
{
	vector<vector<int>> cutnums;
	int G;
	int H;
	int F;
	int white;
	ANode* father;
	int step;

	ANode(vector<vector<int>>& _arr, int _G, int _H, int _white, ANode* _father, int _step)
		: cutnums(_arr)
		, G(_G)
		, H(_H)
		, F(G + H)
		, white(_white)
		, father(_father)
		, step(_step)
	{
	}
};

class A_start
{
public:
	A_start(vector<vector<int>>& _startNums);
	ANode* search();
	static bool IsNormal;
	static int wpos;
	static int wrow;
	static int wcol;
private:
	int _abs(int num);
	int getH(vector<vector<int>>& _cutnums);
	void getWRC(int& white, int& rows, int& cols);
	int exist(ANode* node);
	int OpenSearch(int x);
	void Insert(ANode* newNode);
	void ExistAndOperate(ANode* newNode);
	vector<vector<int>> targetNums = { {1,2,3},{4,5,6}, {7,8,9} };
	vector<ANode*> openlist;
	vector<ANode*> closelist;
};