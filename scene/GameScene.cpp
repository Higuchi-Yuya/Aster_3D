#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <math.h>

float round_n(float number, double n)
{
	number = number * pow(10, n - 1); //四捨五入したい値を10の(n-1)乗倍する。
	number = round(number); //小数点以下を四捨五入する。
	number /= pow(10, n - 1); //10の(n-1)乗で割る。
	return number;
}

GameScene::GameScene() {}

GameScene::~GameScene()
{
	
	delete model_;
	
	delete debugCamera_;
}

void GameScene::Initialize() {

	Affine_trans* trans = nullptr;

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("block.png");
	pet_textureHandle_ = TextureManager::Load("inu.png");
	goal_textureHandle_ = TextureManager::Load("goal.png");


	// 3Dモデルの生成
	model_ = Model::Create();

	//ワールドトランスフォームの初期化
	for (int z = 0; z < 4; z++)
	{
		for (int x = 0; x < 4; x++)
		{
			worldTransforms_[z][x].Initialize();
			worldTransforms_[z][x].translation_ = { 0.0f + (x * 2.0f),0.0f,0.0f + (z * 2.0f) };

			trans->translation(worldTransforms_[z][x].matWorld_, worldTransforms_[z][x].translation_);
			worldTransforms_[z][x].TransferMatrix();
		}
	}
	int y = 0;
	for (int x = 0; x < 4; x++)
	{
		worldTransforms_[4][x].Initialize();
		worldTransforms_[4][x].translation_ = { 0.0f + (x * 2.0f),0.0f + (y * 2.0f),8.0f};

		trans->translation(worldTransforms_[4][x].matWorld_, worldTransforms_[4][x].translation_);
		worldTransforms_[4][x].TransferMatrix();
		y++;
	}
	int f = 0;
	for (int z = 5; z < 9; z++)
	{
		
		f++;
		for (int x = 0; x < 4; x++)
		{
			worldTransforms_[z][x].Initialize();
			worldTransforms_[z][x].translation_ = { 0.0f + (x * 2.0f),6.0f,0.0f + ((z - (2 * f)) * 2.0f) };

			trans->translation(worldTransforms_[z][x].matWorld_, worldTransforms_[z][x].translation_);
			worldTransforms_[z][x].TransferMatrix();
			
		}
	
	}
	/*for (int x = 0; x < 3; x++)
	{
		map[5][x] = 1;
	}*/

	//隣接ノードの作成
	CreateMap();

	//スタートとゴールのセット
	Cell start = Cell(0, 0);
	Cell goal = Cell(3, 7);

	//A*の処理
	AStar(start, goal, cell_x, cell_z, num_count, worldTransforms_);

	pet_.Initialize();
	pet_.translation_ = { 0.0f,2.0f,0.0f };
	trans->translation(pet_.matWorld_, pet_.translation_);
	pet_.TransferMatrix();

	goal_.Initialize();
	goal_.translation_ = { worldTransforms_[goal.Y][goal.X].translation_.x,worldTransforms_[goal.Y][goal.X].translation_.y + 2.0f,worldTransforms_[goal.Y][goal.X].translation_.z };
	trans->translation(goal_.matWorld_, goal_.translation_);
	goal_.TransferMatrix();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();


}

void GameScene::Update()
{
	//Asterの更新処理
	Vector3 move = { 0, 0, 0 };
	Affine_trans* trans = nullptr;
	
	if (input_->TriggerKey(DIK_SPACE))
	{
		count = 0;
		pet_.translation_.x = 0.0f;
		pet_.translation_.y = 2.0f;
		pet_.translation_.z = 0.0f;
	}

	for (count; count < num_count;)
	{
		if ((int)pet_.translation_.y < worldTransforms_[cell_z[count]][cell_x[count]].translation_.y+2.0f)
		{
			move = { 0,+pet_speed_y,0 };
			pet_.translation_.y += move.y;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.y > worldTransforms_[cell_z[count]][cell_x[count]].translation_.y + 2.0f)
		{
			move = { 0,-pet_speed_y,0 };
			pet_.translation_.y += move.y;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.x > worldTransforms_[cell_z[count]][cell_x[count]].translation_.x &&
				 (int)pet_.translation_.z > worldTransforms_[cell_z[count]][cell_x[count]].translation_.z)
		{
			move={-pet_speed_x,0,-pet_speed_z};
			pet_.translation_.x += move.x;
			pet_.translation_.z += move.z;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.x > worldTransforms_[cell_z[count]][cell_x[count]].translation_.x &&
				 (int)pet_.translation_.z < worldTransforms_[cell_z[count]][cell_x[count]].translation_.z)
		{
			move = { -pet_speed_x,0,+pet_speed_z };
			pet_.translation_.x += move.x;
			pet_.translation_.z += move.z;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.x < worldTransforms_[cell_z[count]][cell_x[count]].translation_.x &&
				 (int)pet_.translation_.z > worldTransforms_[cell_z[count]][cell_x[count]].translation_.z)
		{
			move = { +pet_speed_x,0,-pet_speed_z };
			pet_.translation_.x += move.x;
			pet_.translation_.z += move.z;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.x < worldTransforms_[cell_z[count]][cell_x[count]].translation_.x &&
				 (int)pet_.translation_.z < worldTransforms_[cell_z[count]][cell_x[count]].translation_.z)
		{
			move = { +pet_speed_x,0,+pet_speed_z };
			pet_.translation_.x += move.x;
			pet_.translation_.z += move.z;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.x < worldTransforms_[cell_z[count]][cell_x[count]].translation_.x)
		{
			move = { +pet_speed_x,0,0 };
			pet_.translation_.x += move.x;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.x > worldTransforms_[cell_z[count]][cell_x[count]].translation_.x)
		{
			move = { -pet_speed_x,0,0 };
			pet_.translation_.x += move.x;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.z < worldTransforms_[cell_z[count]][cell_x[count]].translation_.z)
		{
			move = { 0,0,+pet_speed_z };
			pet_.translation_.z += move.z;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}
		else if ((int)pet_.translation_.z > worldTransforms_[cell_z[count]][cell_x[count]].translation_.z)
		{
			move = { 0,0,-pet_speed_z };
			pet_.translation_.z += move.z;
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			break;
		}

		if ((int)pet_.translation_.x == worldTransforms_[cell_z[count]][cell_x[count]].translation_.x &&
			(int)pet_.translation_.z == worldTransforms_[cell_z[count]][cell_x[count]].translation_.z) 
		{
			
			pet_.translation_.x = floorf(pet_.translation_.x);
			pet_.translation_.y = floorf(pet_.translation_.y);
			pet_.translation_.z = floorf(pet_.translation_.z);
			trans->translation(pet_.matWorld_, pet_.translation_);
			pet_.TransferMatrix();
			count++;
		}

	}
	debugText_->SetPos(50, 50);
	debugText_->Printf("speed_z=%f", pet_speed_z);

	debugCamera_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dモデル描画
	for (int z = 0; z < 9; z++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (map_[z][x] == 0)
			{
				model_->Draw(worldTransforms_[z][x], debugCamera_->GetViewProjection(), textureHandle_);
			}
		}
	}
	model_->Draw(pet_, debugCamera_->GetViewProjection(), pet_textureHandle_);
	model_->Draw(goal_, debugCamera_->GetViewProjection(), goal_textureHandle_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
