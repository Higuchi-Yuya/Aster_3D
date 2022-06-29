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
	NotFound,       //������
	Erased,         //�폜
	CouldntErased   //�폜�ł��Ȃ�
};

//�}�b�v�̃}�X��
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

//�����Ǘ��p�֐�
bool Less(Node* a, Node* b);

//�Z���͈̔͂��`�F�b�N����֐�
bool IsCellWithinTheRange(int x, int y);

//�m�[�h�쐬
void CreateMap();

// �R�X�g������
void InitCost(int heuristic_cost, int total_cost);

// �q���[���X�e�B�b�N�R�X�g�v�Z(�m�[�h�ƃS�[���܂ł̋�����Ԃ��Ă���)
float CalculateHeuristic(const Node* node, const Node* Goal, WorldTransform Map_[][4]);

// �Z����r
bool IsEqualCell(const Cell& a, const Cell& b);

EraseResult EraseNode(std::list<Node*>& list, Node* new_node, float new_cost);

// �I�[�v�����X�g�ɒǉ�
bool AddAdjacentNode(std::list<Node*>& open_list, std::list<Node*>& close_list, Node* adjacent_node,  float cost);

void AStar(Cell &start, Cell &goal, int* cell_x, int* cell_y, int& Number_of_searches, WorldTransform Map_[][4]);


