//
// Created by 王浩强 on 17/1/20.
//
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

#ifndef B_PROBLEM_MODEL_H
#define B_PROBLEM_MODEL_H

#endif //B_PROBLEM_MODEL_H

const int CLASS = 0;
const int LENGTH = 100;

typedef struct {
    int status;             //cell状态,       1 是车 0 空闲
    int v;                  //v速度,          取值0,1,2,3,4,5
    int change;             //变道,           -1 左变 0 不变 1 右边
    int accelerate;         //是否加速,        1 加速 0 不变 -1 减速
    int is_auto;            //是否是自动驾驶    1 自动 0 手动
    int time;
} Cell;

/* 元胞自动机模型,从0位置开始,0位置为通过收费站之后的状态 */

void make_choice(Cell tollbooth[][LENGTH], int B, int L, int length1);
// 车手根据情况进行决策,顺序为从前到后
/*
 * tollbooth Cell 格式数组,存储整个路的信息
 * B 表示tollbooth的车道数
 * L 表示高速路lane车道数
 * length1 合并部分的长度
 */

void make_sync(Cell tollbooth[][LENGTH], int B, int L, int length1);
// 根据当前的决策情况更新汽车移动, 初始化汽车决策数据
/*
 * tollbooth Cell 格式数组,存储整个路的信息
 * B 表示tollbooth的车道数
 * L 表示高速路lane车道数
 * length1 合并部分的长度
 */
void make_sync2(Cell tollbooth[][LENGTH], int B, int L, int length1, Mat pic, Mat init);

void get_in_equal(Cell tollbooth[][LENGTH], int B, int L,FILE *fp);
// 产生从入口进入的汽车 模型1中B个车道进入的车为等概率分布,初始为0速度
/*
 * tollbooth Cell 格式数组,存储整个路的信息
 * B 表示tollbooth的车道数
 */

void get_in_Noequal(Cell tollbooth[][LENGTH], int B);
// 产生从入口进入的汽车 模型2中B个车道进入的车为非等概率分布,初始为0速度
/*
 * tollbooth Cell 格式数组,存储整个路的信息
 * B 表示tollbooth的车道数
 */

int check_out(int x, int y, int B, int L, int length1);
// 检测当前位置是否合法
/*
 * tollbooth Cell 格式数组,存储整个路的信息
 * B 表示tollbooth的车道数
 * length1 合并部分的长度
 */

void rand_init(Cell tollbooth[][LENGTH], int B, int L, int length1); // 随即初始化所有汽车分布

void calc_flux(Cell tollbooth[][LENGTH], int B, int L, int length1);
// 统计当前在汇合出增加的流量

int get_flux(); // 获取流量

int get_gap(int v1, int v2, int is_auto = 0);

void cell_sync(Cell tollbooth[][LENGTH], int x, int y, int B,int L, int length1); //更新当前坐标

int get_boom(); // 检测碰撞

int get_in();

int get_out();

int get_total_time();

int get_change_times();

void get_shape(FILE *fp, int B, int L, int length1);

void GetInitMat(int B, int L, int length1, Mat& pic_tollbooth);

int get_delay_time_min();

int get_delay_time_max();

void output_delay_status(FILE *fp);

void get_min_delay_time(FILE *fp, int B);
