#pragma once

#include <stdio.h>
#include <memory.h>
#include <list>
#include <queue>
#include <vector>
#include <math.h>
#include "WorldTransform.h"

const int MapWidth = 4;
const int MapHeight = 9;
const int Infinity = 100000;

enum EraseResult
{
	NotFound,       //未発見
	Erased,         //削除
	CouldntErased   //削除できない
};

//マップのマス目
struct Cell
{
	int X;
	int Y;

	Cell() :
		X(-1), Y(-1)
	{

	}
	Cell(int x_, int y_)
	{
		X = x_;
		Y = y_;
	}
};

struct Node
{
	Cell Position;
	std::vector<Node*> AdjacentNodes;
	float HeuristicCost;
	float TotalCost;

	Node() :
		Node(0, 0)
	{
	}
	Node(int x, int y) :
		Position(x, y),
		HeuristicCost(Infinity),
		TotalCost(0)
	{
		AdjacentNodes.clear();
	}
};

//昇順管理用関数
bool Less(Node* a, Node* b);

//セルの範囲をチェックする関数
bool IsCellWithinTheRange(int x, int y);

//ノード作成
void CreateMap();

// コスト初期化
void InitCost(int heuristic_cost, int total_cost);

// ヒューリスティックコスト計算(ノードとゴールまでの距離を返している)
float CalculateHeuristic(const Node* node, const Node* Goal, WorldTransform Map_[][4]);

// セル比較
bool IsEqualCell(const Cell& a, const Cell& b);

EraseResult EraseNode(std::list<Node*>& list, Node* new_node, float new_cost);

// オープンリストに追加
bool AddAdjacentNode(std::list<Node*>& open_list, std::list<Node*>& close_list, Node* adjacent_node,  float cost);

void AStar(Cell &start, Cell &goal, int* cell_x, int* cell_y, int& Number_of_searches, WorldTransform Map_[][4]);


