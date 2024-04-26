#pragma once

void Initialize(void);
void Reset(void);
void MovePlayer(void);
void MoveMissile(void);
void MoveEnemy(void);
//vectorは(x, y, z) = (-, +, +)の頂点のものを指定。ビットで各頂点の値を出しやすくするため
void CreateBox(VECTOR vector, float distance);