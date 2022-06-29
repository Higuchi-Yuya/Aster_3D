#include "Aster.h"

//�m�[�h


int CostTable[MapHeight][MapWidth] =
{
	//0  1  2  3  4
	{ 0, 0, 0, 0 }, // 0
	{ 0, 0, 0, 0 }, // 1
	{ 0, 0, 0, 0 }, // 2
	{ 0, 0, 0, 0 }, // 3
	{ 0, 0, 0, 0 }, // 4
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
};

//�O���t
Node Map[MapHeight][MapWidth];

//�����Ǘ��p�֐�
bool Less(Node* a, Node* b)
{
	if (a->TotalCost < b->TotalCost)
	{
		return true;
	}

	return false;
}

//�Z���͈̔͂��`�F�b�N����֐�
bool IsCellWithinTheRange(int x, int y)
{
	if (x >= 0 &&
		x < MapWidth &&
		y >= 0 &&
		y < MapHeight)
	{
		return true;
	}

	return false;
}

//�m�[�h�쐬
void CreateMap()
{
	for (int y = 0; y < MapHeight; y++)
	{
		for (int x = 0; x < MapWidth; x++)
		{
			Map[y][x].Position.X = x;
			Map[y][x].Position.Y = y;

			Cell adjacent_cells[] =
			{
				//�㉺�E������
				Cell(x, y - 1),
				Cell(x - 1, y),
				Cell(x + 1, y),
				Cell(x, y + 1),

				//�΂ߕ�����ǉ�
				Cell(x - 1,y - 1),
				Cell(x - 1,y + 1),
				Cell(x + 1,y - 1),
				Cell(x + 1,y + 1),
			};

			// �אڃm�[�h�̒ǉ�
			for (const Cell& cell : adjacent_cells)
			{
				if (IsCellWithinTheRange(cell.X, cell.Y) == true && CostTable[cell.Y][cell.X] == 0)
				{
					Map[y][x].AdjacentNodes.push_back(&Map[cell.Y][cell.X]);
				}
			}
		}
	}
}

// �R�X�g������
void InitCost(int heuristic_cost, int total_cost)
{
	for (int y = 0; y < MapHeight; y++)
	{
		for (int x = 0; x < MapWidth; x++)
		{
			Map[y][x].HeuristicCost = heuristic_cost;
			Map[y][x].TotalCost = total_cost;
		}
	}
}

// �q���[���X�e�B�b�N�R�X�g�v�Z(�m�[�h�ƃS�[���܂ł̋�����Ԃ��Ă���)
float CalculateHeuristic(const Node* node, const Node* Goal, WorldTransform Map_[][4])
{
	float x = fabsf(Goal->Position.X - node->Position.X);
	float y = fabsf(Map_[Goal->Position.Y][Goal->Position.X].translation_.y - Map_[node->Position.Y][node->Position.X].translation_.y);
	float z = fabsf(Goal->Position.Y - node->Position.Y);
	return sqrtf(x * x + y * y + z * z);
}

// �Z����r
bool IsEqualCell(const Cell& a, const Cell& b)
{
	if (a.X == b.X &&
		a.Y == b.Y)
	{
		return true;
	}

	return false;
}

EraseResult EraseNode(std::list<Node*>& list, Node* new_node, float new_cost)
{
	// �N���[�Y���X�g�`�F�b�N
	for (auto itr = list.begin(); itr != list.end(); itr++)
	{
		// �m�[�h�Ɠ����Z�������邩���ׂ�
		if (IsEqualCell(new_node->Position, (*itr)->Position) == true)
		{
			// �R�X�g�̔�r
			if (new_cost < (*itr)->TotalCost)
			{
				list.erase(itr);
				return EraseResult::Erased;
			}
			else
			{
				return EraseResult::CouldntErased;
			}
		}
	}

	return EraseResult::NotFound;
}



// �I�[�v�����X�g�ɒǉ�
bool AddAdjacentNode(std::list<Node*>& open_list, std::list<Node*>& close_list, Node* adjacent_node, float cost)
{
	bool can_update = true;

	std::list<Node*> node_list[] =
	{
		close_list,
		open_list
	};
	//std::list<Node*> node_list;
	//node_list.push_back(close_list);
	//node_list.push_back(open_list);

	for (std::list<Node*>& list : node_list)
	{
		// ���X�g�ɓ����m�[�h�������ă��X�g�̕��̃R�X�g�������Ȃ�폜
		if (EraseNode(list, adjacent_node, cost) == EraseResult::CouldntErased)
		{
			can_update = false;
		}
	}

	if (can_update == true)
	{
		open_list.push_back(adjacent_node);
		return true;
	}

	return false;
}

void AStar(Cell &start, Cell& goal, int* cell_x, int* cell_y, int& Number_of_searches, WorldTransform Map_[][4])
{
	std::list<Node*> open_list;
	std::list<Node*> close_list;

	//const Node* start_node = &Map[start.Y][start.X];
	const Node* goal_node = &Map[goal.Y][goal.X];

	// �X�V�����m�[�h�ʒu�ۑ��p
	Cell last_update_cells[MapHeight][MapWidth];

	// �O���t�̏�����
	InitCost(Infinity, 0);

	// �X�^�[�g�m�[�h�̎w��
	open_list.push_back(&Map[start.Y][start.X]);

	// �o�H�T����
	int count = 0;

	//���[�g�m�ۂ�����̃S�[���ɂ��ǂ蒅���܂ł̉�
	int goal_count = 0;

	// �I�[�v�����X�g���Ȃ��Ȃ�܂ŉ�
	while (open_list.empty() == false)
	{
		count++;

		Node* search_node = (*open_list.begin());

		// �T�����X�g���珜�O
		open_list.erase(open_list.begin());

		// �S�[���ɓ��B������I���
		if (IsEqualCell(search_node->Position, goal) == true)
		{
			// �N���[�Y���X�g�ɍŌ�̃m�[�h��ǉ�����
			close_list.push_back(search_node);
			break;
		}

		for (Node* adjacent_node : search_node->AdjacentNodes)
		{
			// �v�Z���s���Ă��Ȃ������ꍇ�����v�Z
			if (adjacent_node->HeuristicCost == Infinity)
			{
				// �q���[���X�e�B�N�X�R�X�g�v�Z
				adjacent_node->HeuristicCost = CalculateHeuristic(adjacent_node, goal_node, Map_);
			}

			// �m�[�h�ԃR�X�g
			float edge_cost = CostTable[adjacent_node->Position.Y][adjacent_node->Position.X];
			// �擾�m�[�h�̃g�[�^���R�X�g
			float node_cost = search_node->TotalCost;

			//�����ɂ��R�X�g�̕ω�------------------------------------------------�ǉ�-------------//
			//��������̏��
			float block_saize = 2.0f;
			//�����̃R�X�g
			float height_cost = 0;

			//���̃}�X�ڂ̍����Ɨאڂ̃}�X�ڂ̍������ׁA��{�ɂ��ēo���Ȃ�R�X�g�̕ω��Ȃ�
			if (Map_[search_node->Position.Y][search_node->Position.X].translation_.y + block_saize == 
				Map_[adjacent_node->Position.Y][adjacent_node->Position.X].translation_.y)
			{
				height_cost = 0;
			}
			//��ׂē�{�ɂ��ēo��Ȃ��ꍇ�ɃR�X�g�ɖ����̒l����
			else if (Map_[search_node->Position.Y][search_node->Position.X].translation_.y + block_saize <
				Map_[adjacent_node->Position.Y][adjacent_node->Position.X].translation_.y)
			{
				height_cost = Infinity;
			}
			//------------------------------------------------------------------------------------------//
			/*
				�g�[�^���R�X�g�Z�o
					�m�[�h�ԃR�X�g + �q���[���X�e�B�b�N�R�X�g + �擾�m�[�h�̃g�[�^���R�X�g
			*/
			float total_cost = edge_cost + adjacent_node->HeuristicCost + node_cost + height_cost;

			// �m�[�h�ǉ�
			if (AddAdjacentNode(open_list, close_list, adjacent_node,total_cost) == true)
			{
				// �g�[�^���R�X�g���X�V
				adjacent_node->TotalCost = total_cost;

				if (adjacent_node->Position.Y == 0 && adjacent_node->Position.X == 2)
				{
					int xx = 0;
					xx = 100;
				}

				// �o�H���X�V�����Z����ۑ�
				last_update_cells[adjacent_node->Position.Y][adjacent_node->Position.X] = search_node->Position;
			}
		}

		bool is_add_close = true;

		// �N���[�Y���X�g�`�F�b�N
		for (auto itr = close_list.begin(); itr != close_list.end(); itr++)
		{
			// �m�[�h�Ɠ����Z�������邩���ׂ�
			if (IsEqualCell(search_node->Position, (*itr)->Position) == true)
			{
				is_add_close = false;
				break;
			}
		}

		// �����m�[�h�����������̂Œǉ�
		if (is_add_close == true)
		{
			// ���̃m�[�h�̒T���I��
			close_list.push_back(search_node);
		}

		// �����\�[�g
		open_list.sort(Less);
	}

	// �o�H����

	std::list<Cell> route_list;

	// �S�[���Z�����畜������
	route_list.push_back(goal);
	while (route_list.empty() == false)
	{
		Cell route = route_list.front();

		// �X�^�[�g�Z���Ȃ�I��
		if (IsEqualCell(route, start) == true)
		{
			// ���������o�H��\��
			for (Cell& cell : route_list)
			{
				goal_count++;
			}

			//�ύX����----------------------------//
			for (int i = 0; i < goal_count;)
			{
				for (Cell& cell : route_list)
				{
					cell_x[i] = cell.X;

					i++;
				}
			}

			for (int i = 0; i < goal_count;)
			{
				for (Cell& cell : route_list)
				{
					cell_y[i] = cell.Y;
					i++;
				}
			}
			//------------------------------------//

			break;
		}
		else
		{
			if (IsCellWithinTheRange(route.X, route.Y) == true)
			{
				// �ǉ�
				route_list.push_front(last_update_cells[route.Y][route.X]);
			}
			else
			{
				//printf("�o�H�͌�����Ȃ�����\n");
				break;
			}
		}
	}
	Number_of_searches = goal_count;


	//-------------------------------------------//

	//printf("�T���� = %d\n", count);
}

