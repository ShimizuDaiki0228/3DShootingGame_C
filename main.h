#pragma once

void Initialize(void);
void Reset(void);
void MovePlayer(void);
void MoveMissile(void);
void MoveEnemy(void);
//vector��(x, y, z) = (-, +, +)�̒��_�̂��̂��w��B�r�b�g�Ŋe���_�̒l���o���₷�����邽��
void CreateBox(VECTOR vector, float distance);