#define PI 3.1416
#include "DxLib.h"
#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//�萔�̒�`
const int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 720;

int _playerModel;
int _missileModel;
int _enemyModel[2];

//���@�̕ϐ�
float _playerPosX = 0.0f, _playerPosY = 0.0f, _playerPosZ = 0.0f; // ���W
int _playerZa = 0; // �@�̂̌X��
bool _oldSpaceKey = false; //1�t���[���O�̃X�y�[�X�L�[�̏��


//�e�p�̕ϐ�
const int MISSILE_MAX = 20; //�e�̍ő吔
float _missilePosX[MISSILE_MAX], _missilePosY[MISSILE_MAX], _missilePosZ[MISSILE_MAX]; //�e�̍��W
bool _missileFlg[MISSILE_MAX]; //���݂��邩�̃t���O

//�G�@�p�̕ϐ�
const int ENEMY_MAX = 10; // �G�̍ő吔
float _enemyPosX[ENEMY_MAX], _enemyPosY[ENEMY_MAX], _enemyPosZ[ENEMY_MAX]; //�G�@�̍��W




//�e�X�g�p
VERTEX3D vertex[8];
unsigned short indexData[36] = { // �C���f�b�N�X�f�[�^�i�e�ʂ̒��_���w��j
	0, 1, 2, 1, 3, 2, // �O��
	1, 5, 3, 5, 7, 3, // �E��
	5, 4, 7, 4, 6, 7, // �w��
	4, 0, 6, 0, 2, 6, // ����
	4, 5, 0, 5, 1, 0, // ���
	2, 3, 6, 3, 7, 6  // ����
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("3D�V���[�e�B���O�Q�[��");
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	int timer = 0;
	float camX = 0.0f, camY = 100.0f, camZ = -600.0f;
	float objX = 0.0f, objY = 0.0f, objZ = 1000.0f;
	int model = MV1LoadModel("model02/model.FBX"); //���f���f�[�^�̓ǂݍ���
	ChangeLightTypeDir(VGet(1.0f, -1.0f, 0.5f)); //�f�B���N�V���i�����C�g�̌�����ݒ�

	//������
	Initialize();
	//���Z�b�g
	Reset();

	//�J�����ʒu�y�ђ����_
	VECTOR camPos = VGet(camX, camY, camZ);
	VECTOR camTar = VGet(objX, objY, objZ);
	//�J�����ʒu�ƒ����_���Z�b�g
	SetCameraPositionAndTarget_UpVecY(camPos, camTar);

	VECTOR box = VGet(0, 0, -300);
	float distance = 100;
	CreateBox(box, distance);

	while (1)
	{
		ClearDrawScreen();


		// �e�ʂ̃|���S����`��
		for (int i = 0; i < 12; i++) {
			DrawPolygon3D(&vertex[indexData[i * 3]], 1, -1, TRUE);
		}

		timer++;
		DrawFormatString(0, 0, 0xffff00, "%d", timer);

		//������`�����߂ŉF���̐���\��
		for (int i = 0; i < 20; i++)
		{
			float z = 800.0f * i - (timer % 20) * 40; // z���W
			int col = (30 - i) * 6; // �F, �����ɍs���ɂ�č����Ȃ�悤��
			DrawLine3D(VGet(-400.0f, -240.0f, z), VGet(400.0f, -240.0f, z), GetColor(col, col, col));
		}

		//���@�̈ړ�
		MovePlayer();

		//�e�̏���
		MoveMissile();

		//�G�@�̈ړ�
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
/// ������
/// </summary>
void Initialize()
{
	_playerModel = MV1LoadModel("model/fighter.mqoz");
	_missileModel = MV1LoadModel("model/missile.mqoz");
	//�G�@�̉摜�̓ǂݍ���
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
/// ���Z�b�g
/// </summary>
void Reset()
{
	//�e�̑��݂��邩�̃t���O�̏�����
	for (int i = 0; i < MISSILE_MAX; i++) _missileFlg[i] = false;

	//�G�@�̈ʒu�̏�����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		_enemyPosX[i] = -300.0f + rand() % 600;
		_enemyPosY[i] = -200.0f + rand() % 400;
		_enemyPosZ[i] = 10000.0f;
	}
}

/// <summary>
/// ���@�𑀍삷��
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
		_playerZa /= 2; //�L�[���삪������ΌX����߂��Ă���
	}

	MV1SetRotationXYZ(_playerModel, VGet(0.0f, 0.0f, PI * _playerZa / 180)); // ���@�̉�]�p��ݒ�
	MV1SetPosition(_playerModel, VGet(_playerPosX, _playerPosY, _playerPosZ)); // ���@���O������Ԃɔz�u
	MV1DrawModel(_playerModel);
}

/// <summary>
/// �e�̏���
/// </summary>
/// <param name=""></param>
void MoveMissile(void)
{
	//�e�̑��U
	if (CheckHitKey(KEY_INPUT_SPACE) && !_oldSpaceKey)
	{
		for (int i = 0; i < MISSILE_MAX; i++)
		{
			if (_missileFlg[i]) continue;
			_missilePosX[i] = _playerPosX;
			_missilePosY[i] = _playerPosY;
			_missilePosZ[i] = _playerPosZ;
			_missileFlg[i] = 1; //���݂��Ă����Ԃɂ���
			break;
		}
	}

	_oldSpaceKey = CheckHitKey(KEY_INPUT_SPACE);

	//�e�̔���
	for (int i = 0; i < MISSILE_MAX; i++)
	{
		if (!_missileFlg[i]) continue;
		_missilePosZ[i] += 200; // ��ʂ̉��Ɍ������Ĕ�΂�
		MV1SetPosition(_missileModel, VGet(_missilePosX[i], _missilePosY[i], _missilePosZ[i])); //�e�̔z�u
		MV1DrawModel(_missileModel); //�e�̕`��
		if (_missilePosZ[i] > 10000) _missileFlg[i] = false; // ��ʉ��܂Ŕ�񂾂�e���폜����
	}
}

/// <summary>
/// �G�@�̏���
/// </summary>
/// <param name=""></param>
void MoveEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		_enemyPosZ[i] = _enemyPosZ[i] - 10 - i * 2;
		if (_enemyPosZ[i] < -200) _enemyPosZ[i] = 10000; // ��O�܂ŗ����牜�Ɉړ�������ɕ\��������悤��
		int modelTypeCount = i % 2; // 2�̃��f�������݂Ɏg��
		MV1SetPosition(_enemyModel[modelTypeCount], VGet(_enemyPosX[i], _enemyPosY[i], _enemyPosZ[i])); // �G�@��z�u
		MV1DrawModel(_enemyModel[modelTypeCount]);

		//�e�Ƃ̃q�b�g�`�F�b�N
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
/// 3D�̗����̂����֐�
/// </summary>
void CreateBox(VECTOR vector, float distance)
{

	//�e���_���o���A�z��ɓ���Ă���
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