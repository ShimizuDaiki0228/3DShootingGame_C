#define PI 3.1416
#include "DxLib.h"
#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//定数の定義
const int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 720;

int _playerModel;
int _missileModel;
int _enemyModel[2];

//自機の変数
float _playerPosX = 0.0f, _playerPosY = 0.0f, _playerPosZ = 0.0f; // 座標
int _playerZa = 0; // 機体の傾き
bool _oldSpaceKey = false; //1フレーム前のスペースキーの状態


//弾用の変数
const int MISSILE_MAX = 20; //弾の最大数
float _missilePosX[MISSILE_MAX], _missilePosY[MISSILE_MAX], _missilePosZ[MISSILE_MAX]; //弾の座標
bool _missileFlg[MISSILE_MAX]; //存在するかのフラグ

//敵機用の変数
const int ENEMY_MAX = 10; // 敵の最大数
float _enemyPosX[ENEMY_MAX], _enemyPosY[ENEMY_MAX], _enemyPosZ[ENEMY_MAX]; //敵機の座標




//テスト用
VERTEX3D vertex[8];
unsigned short indexData[36] = { // インデックスデータ（各面の頂点を指定）
	0, 1, 2, 1, 3, 2, // 前面
	1, 5, 3, 5, 7, 3, // 右面
	5, 4, 7, 4, 6, 7, // 背面
	4, 0, 6, 0, 2, 6, // 左面
	4, 5, 0, 5, 1, 0, // 上面
	2, 3, 6, 3, 7, 6  // 下面
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("3Dシューティングゲーム");
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	int timer = 0;
	float camX = 0.0f, camY = 100.0f, camZ = -600.0f;
	float objX = 0.0f, objY = 0.0f, objZ = 1000.0f;
	int model = MV1LoadModel("model02/model.FBX"); //モデルデータの読み込み
	ChangeLightTypeDir(VGet(1.0f, -1.0f, 0.5f)); //ディレクショナルライトの向きを設定

	//初期化
	Initialize();
	//リセット
	Reset();

	//カメラ位置及び注視点
	VECTOR camPos = VGet(camX, camY, camZ);
	VECTOR camTar = VGet(objX, objY, objZ);
	//カメラ位置と注視点をセット
	SetCameraPositionAndTarget_UpVecY(camPos, camTar);

	VECTOR box = VGet(0, 0, -300);
	float distance = 100;
	CreateBox(box, distance);

	while (1)
	{
		ClearDrawScreen();


		// 各面のポリゴンを描画
		for (int i = 0; i < 12; i++) {
			DrawPolygon3D(&vertex[indexData[i * 3]], 1, -1, TRUE);
		}

		timer++;
		DrawFormatString(0, 0, 0xffff00, "%d", timer);

		//線分を描く命令で宇宙の線を表現
		for (int i = 0; i < 20; i++)
		{
			float z = 800.0f * i - (timer % 20) * 40; // z座標
			int col = (30 - i) * 6; // 色, 遠くに行くにつれて黒くなるように
			DrawLine3D(VGet(-400.0f, -240.0f, z), VGet(400.0f, -240.0f, z), GetColor(col, col, col));
		}

		//自機の移動
		MovePlayer();

		//弾の処理
		MoveMissile();

		//敵機の移動
		MoveEnemy();

		//DrawSphere3D(box, distance, 4, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);


		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}

/// <summary>
/// 初期化
/// </summary>
void Initialize()
{
	_playerModel = MV1LoadModel("model/fighter.mqoz");
	_missileModel = MV1LoadModel("model/missile.mqoz");
	//敵機の画像の読み込み
	/*for (int i = 0; i < 2; i++)
	{
		char file[] = "model/object*.mqoz";
		file[12] = (char)('0' + i);
		_enemyModel[i] = MV1LoadModel(file);
	}*/
	_enemyModel[0] = MV1LoadModel("model/object0.mqoz");
	_enemyModel[1] = MV1LoadModel("model/object1.mqoz");

}

/// <summary>
/// リセット
/// </summary>
void Reset()
{
	//弾の存在するかのフラグの初期化
	for (int i = 0; i < MISSILE_MAX; i++) _missileFlg[i] = false;

	//敵機の位置の初期化
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		_enemyPosX[i] = -300.0f + rand() % 600;
		_enemyPosY[i] = -200.0f + rand() % 400;
		_enemyPosZ[i] = 10000.0f;
	}
}

/// <summary>
/// 自機を操作する
/// </summary>
/// <param name=""></param>
void MovePlayer(void)
{
	if (CheckHitKey(KEY_INPUT_UP) && _playerPosY < 200) _playerPosY += 8;
	if (CheckHitKey(KEY_INPUT_DOWN) && _playerPosY > -200) _playerPosY -= 8;
	if (CheckHitKey(KEY_INPUT_LEFT))
	{
		if (_playerPosX > -300) _playerPosX -= 12;
		if (_playerZa < 30) _playerZa += 10;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		if (_playerPosX < 300) _playerPosX += 12;
		if (_playerZa > -30) _playerZa -= 10;
	}
	else
	{
		_playerZa /= 2; //キー操作が無ければ傾きを戻していく
	}

	MV1SetRotationXYZ(_playerModel, VGet(0.0f, 0.0f, PI * _playerZa / 180)); // 自機の回転角を設定
	MV1SetPosition(_playerModel, VGet(_playerPosX, _playerPosY, _playerPosZ)); // 自機を三次元空間に配置
	MV1DrawModel(_playerModel);
}

/// <summary>
/// 弾の処理
/// </summary>
/// <param name=""></param>
void MoveMissile(void)
{
	//弾の装填
	if (CheckHitKey(KEY_INPUT_SPACE) && !_oldSpaceKey)
	{
		for (int i = 0; i < MISSILE_MAX; i++)
		{
			if (_missileFlg[i]) continue;
			_missilePosX[i] = _playerPosX;
			_missilePosY[i] = _playerPosY;
			_missilePosZ[i] = _playerPosZ;
			_missileFlg[i] = 1; //存在している状態にする
			break;
		}
	}

	_oldSpaceKey = CheckHitKey(KEY_INPUT_SPACE);

	//弾の発射
	for (int i = 0; i < MISSILE_MAX; i++)
	{
		if (!_missileFlg[i]) continue;
		_missilePosZ[i] += 200; // 画面の奥に向かって飛ばす
		MV1SetPosition(_missileModel, VGet(_missilePosX[i], _missilePosY[i], _missilePosZ[i])); //弾の配置
		MV1DrawModel(_missileModel); //弾の描画
		if (_missilePosZ[i] > 10000) _missileFlg[i] = false; // 画面奥まで飛んだら弾を削除する
	}
}

/// <summary>
/// 敵機の処理
/// </summary>
/// <param name=""></param>
void MoveEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		_enemyPosZ[i] = _enemyPosZ[i] - 10 - i * 2;
		if (_enemyPosZ[i] < -200) _enemyPosZ[i] = 10000; // 手前まで来たら奥に移動させ常に表示させるように
		int modelTypeCount = i % 2; // 2つのモデルを交互に使う
		MV1SetPosition(_enemyModel[modelTypeCount], VGet(_enemyPosX[i], _enemyPosY[i], _enemyPosZ[i])); // 敵機を配置
		MV1DrawModel(_enemyModel[modelTypeCount]);

		//弾とのヒットチェック
		for (int j = 0; j < MISSILE_MAX; j++)
		{
			if (!_missileFlg[j]) continue;
			int dx = abs((int)(_enemyPosX[i] - _missilePosX[j]));
			int dy = abs((int)(_enemyPosY[i] - _missilePosY[j]));
			int dz = abs((int)(_enemyPosZ[i] - _missilePosZ[j]));
			if(dx < 100 && dy < 100 && dz < 120)
			{
				_enemyPosZ[i] = -200;
				_missileFlg[j] = false;
			}
		}
	}
}

/// <summary>
/// 3Dの立方体を作る関数
/// </summary>
void CreateBox(VECTOR vector, float distance)
{

	//各頂点を出し、配列に入れていく
	for (int i = 0; i < 7; i++)
	{
		vertex[i] = {
			VGet(
				vector.x + (i & 1 ? distance : 0),
				vector.y - (i & 2 ? distance : 0),
				vector.z - (i & 4 ? distance : 0)
			),
			VGet(0.0f, 1.0f, 1.0f),
			GetColorU8(255, 255, 255, 255),
			GetColorU8(255, 255, 255, 255),
			1.0f,
			0.0f
		};
	}

	

	//return DrawPolygon3D(vertex, 2, DX_NONE_GRAPH, FALSE);
}