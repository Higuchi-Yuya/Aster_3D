#include "Aster.h"

//ノード


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

//グラフ
Node Map[MapHeight][MapWidth];

//昇順管理用関数
bool Less(Node* a, Node* b)
{
	if (a->TotalCost < b->TotalCost)
	{
		return true;
	}

	return false;
}

//セルの範囲をチェックする関数
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

//ノード作成
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
				//上下右左方向
				Cell(x, y - 1),
				Cell(x - 1, y),
				Cell(x + 1, y),
				Cell(x, y + 1),

				//斜め方向を追加
				Cell(x - 1,y - 1),
				Cell(x - 1,y + 1),
				Cell(x + 1,y - 1),
				Cell(x + 1,y + 1),
			};

			// 隣接ノードの追加
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

// コスト初期化
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

// ヒューリスティックコスト計算(ノードとゴールまでの距離を返している)
float CalculateHeuristic(const Node* node, const Node* Goal, WorldTransform Map_[][4])
{
	float x = fabsf(Goal->Position.X - node->Position.X);
	float y = fabsf(Map_[Goal->Position.Y][Goal->Position.X].translation_.y - Map_[node->Position.Y][node->Position.X].translation_.y);
	float z = fabsf(Goal->Position.Y - node->Position.Y);
	return sqrtf(x * x + y * y + z * z);
}

// セル比較
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
	// クローズリストチェック
	for (auto itr = list.begin(); itr != list.end(); itr++)
	{
		// ノードと同じセルがあるか調べる
		if (IsEqualCell(new_node->Position, (*itr)->Position) == true)
		{
			// コストの比較
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



// オープンリストに追加
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
		// リストに同じノードがあってリストの方のコストが高いなら削除
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

	// 更新したノード位置保存用
	Cell last_update_cells[MapHeight][MapWidth];

	// グラフの初期化
	InitCost(Infinity, 0);

	// スタートノードの指定
	open_list.push_back(&Map[start.Y][start.X]);

	// 経路探索回数
	int count = 0;

	//ルート確保した後のゴールにたどり着くまでの回数
	int goal_count = 0;

	// オープンリストがなくなるまで回す
	while (open_list.empty() == false)
	{
		count++;

		Node* search_node = (*open_list.begin());

		// 探索リストから除外
		open_list.erase(open_list.begin());

		// ゴールに到達したら終わり
		if (IsEqualCell(search_node->Position, goal) == true)
		{
			// クローズリストに最後のノードを追加する
			close_list.push_back(search_node);
			break;
		}

		for (Node* adjacent_node : search_node->AdjacentNodes)
		{
			// 計算を行っていなかった場合だけ計算
			if (adjacent_node->HeuristicCost == Infinity)
			{
				// ヒューリスティクスコスト計算
				adjacent_node->HeuristicCost = CalculateHeuristic(adjacent_node, goal_node, Map_);
			}

			// ノード間コスト
			float edge_cost = CostTable[adjacent_node->Position.Y][adjacent_node->Position.X];
			// 取得ノードのトータルコスト
			float node_cost = search_node->TotalCost;

			//高さによるコストの変化------------------------------------------------追加-------------//
			//高さ一個分の情報
			float block_saize = 2.0f;
			//高さのコスト
			float height_cost = 0;

			//今のマス目の高さと隣接のマス目の高さを比べ、二倍にして登れるならコストの変化なし
			if (Map_[search_node->Position.Y][search_node->Position.X].translation_.y + block_saize == 
				Map_[adjacent_node->Position.Y][adjacent_node->Position.X].translation_.y)
			{
				height_cost = 0;
			}
			//比べて二倍にして登れない場合にコストに無限の値を代入
			else if (Map_[search_node->Position.Y][search_node->Position.X].translation_.y + block_saize <
				Map_[adjacent_node->Position.Y][adjacent_node->Position.X].translation_.y)
			{
				height_cost = Infinity;
			}
			//------------------------------------------------------------------------------------------//
			/*
				トータルコスト算出
					ノード間コスト + ヒューリスティックコスト + 取得ノードのトータルコスト
			*/
			float total_cost = edge_cost + adjacent_node->HeuristicCost + node_cost + height_cost;

			// ノード追加
			if (AddAdjacentNode(open_list, close_list, adjacent_node,total_cost) == true)
			{
				// トータルコストを更新
				adjacent_node->TotalCost = total_cost;

				if (adjacent_node->Position.Y == 0 && adjacent_node->Position.X == 2)
				{
					int xx = 0;
					xx = 100;
				}

				// 経路を更新したセルを保存
				last_update_cells[adjacent_node->Position.Y][adjacent_node->Position.X] = search_node->Position;
			}
		}

		bool is_add_close = true;

		// クローズリストチェック
		for (auto itr = close_list.begin(); itr != close_list.end(); itr++)
		{
			// ノードと同じセルがあるか調べる
			if (IsEqualCell(search_node->Position, (*itr)->Position) == true)
			{
				is_add_close = false;
				break;
			}
		}

		// 同じノードが無かったので追加
		if (is_add_close == true)
		{
			// このノードの探索終了
			close_list.push_back(search_node);
		}

		// 昇順ソート
		open_list.sort(Less);
	}

	// 経路復元

	std::list<Cell> route_list;

	// ゴールセルから復元する
	route_list.push_back(goal);
	while (route_list.empty() == false)
	{
		Cell route = route_list.front();

		// スタートセルなら終了
		if (IsEqualCell(route, start) == true)
		{
			// 復元した経路を表示
			for (Cell& cell : route_list)
			{
				goal_count++;
			}

			//変更部分----------------------------//
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
				// 追加
				route_list.push_front(last_update_cells[route.Y][route.X]);
			}
			else
			{
				//printf("経路は見つからなかった\n");
				break;
			}
		}
	}
	Number_of_searches = goal_count;


	//-------------------------------------------//

	//printf("探索回数 = %d\n", count);
}

