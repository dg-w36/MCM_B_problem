//
// Created by 王浩强 on 17/1/20.
//
#include "model.h"
#include <ctime>
#include <cstdlib>
#include <stdio.h>`


double p = 0.1;
const double p2 = 0.1;
int min_delta_time[100] = {3};
const int v_max = 10;
int delay_time_min,delay_time_max;
int delta_time[100];
int flux = 0;
int boom = 0;
int input = 0;
int out = 0;
int total_time = 0;
int change_times = 0;
int length_tollbooth[100] = {25,50,LENGTH,LENGTH,LENGTH,LENGTH,50,25};
int delay_time_car[30];
#define WIDTH 300
#define LEN 1600
#define min(x,y) (x > y ? y : x)

void get_v_and_pos(Cell tollbooth[][LENGTH], int i, int j, int B, int L, int length1, int max_gap, int dir, int& v2, int& pos2){
    pos2 = j+1;
    v2 = 10000;
    if(dir == 1) { // 找前方最近的车
//        if (i >= 0 && i < L) { // 合并后的车道
            while (pos2 < length_tollbooth[i]) {
                if (pos2 - j > max_gap) { // 大于最大安全距离,直接加速
                    pos2 = LENGTH;
                    return;
                }
                if (tollbooth[i][pos2].status == 1 && tollbooth[i][pos2].change == 0) {
                    //当前道上车
                    if(v2 > tollbooth[i][pos2].v + tollbooth[i][pos2].accelerate )
                        v2 = tollbooth[i][pos2].v + tollbooth[i][pos2].accelerate;
                }
                if (check_out(i - 1, pos2, B, L, length1) && tollbooth[i - 1][pos2].status == 1 &&
                    tollbooth[i - 1][pos2].change == 1) {
                    //左侧变道而来
                    if(v2 > tollbooth[i-1][pos2].v + tollbooth[i-1][pos2].accelerate )
                    v2 = tollbooth[i-1][pos2].v + tollbooth[i-1][pos2].accelerate;
                }
                if (check_out(i + 1, pos2, B, L, length1) && tollbooth[i + 1][pos2].status == 1 &&
                    tollbooth[i + 1][pos2].change == -1) {
                    //右侧变道而来
                    if(v2 > tollbooth[i+1][pos2].v + tollbooth[i+1][pos2].accelerate )
                    v2 = tollbooth[i+1][pos2].v + tollbooth[i+1][pos2].accelerate;
                }
                if(v2!= 10000)
                    return;
                pos2++;
            }
    }
    else if(dir == 0){ //向后进行判断
        pos2 = j-1;
        v2 = 0;
        while (pos2 >= 0 ) {
            if (tollbooth[i][pos2].status == 1 && tollbooth[i][pos2].change == 0) {
                //当前道上车
                v2 = tollbooth[i][pos2].v;
                return;
            }
            pos2--;
        }
    }
    v2 = 0;
}

void make_choice(Cell tollbooth[][LENGTH], int B, int L, int length1) {
    // 车手根据情况进行决策,顺序为从前到后
    int max_gap,gap_now,gap_left_f,gap_right_f,gap_left_b,gap_right_b,tmp_gap,tmp_gap2,tmp_gap3,v_now;
    //  最大gap确定最大范围,now为前方gap,并计算左侧和右侧的前后的gap
    int v2,pos2;
    int if_choice = 0;
    if(CLASS == 0){ //中心齐模型
        for(int j = LENGTH-1; j >= 0 ;j--){
            for(int i = B/2-1; i >= 0; i--) {
                if (tollbooth[i][j].status == 1) {
                    if(1.0*rand()/RAND_MAX < p2 && tollbooth[i][j].v>0 && tollbooth[i][j].is_auto == 0){
                        tollbooth[i][j].v--;

                    }
                    v_now = tollbooth[i][j].v;
                    max_gap = get_gap(v_now + 1, 0);
                    max_gap = 1000;
                    // 判断前方
                    get_v_and_pos(tollbooth, i, j, B, L, length1, max_gap, 1, v2, pos2);
                    if (pos2 == LENGTH) {   // 前方没车 则能加速则加速
                        if (v_now < v_max) {
                            tollbooth[i][j].accelerate = 1;
                            continue;
                        }
                    } else {
                        gap_now = pos2 - j;
                        tmp_gap = get_gap(v_now, v2, tollbooth[i][j].is_auto);
                        tmp_gap2 = get_gap(v_now + 1, v2, tollbooth[i][j].is_auto);
                        if (gap_now >= tmp_gap2) { // 当前距离大于安全距离
                            if (v_now < v_max) {
                                tollbooth[i][j].accelerate = 1;
                                continue;
                            }
                        } else if (gap_now >= tmp_gap && gap_now <= tmp_gap2) { // 等于安全距离,保持不动,或者变道加速
                            if (v_now == v_max) { continue; } // 已经最高速则不变道,保持不变
                            if (tollbooth[i][j].is_auto == 1){ continue;}
                            // 判断右车道
                            if (check_out(i + 1, j, B, L, length1)) {
                                get_v_and_pos(tollbooth, i + 1, j-1, B, L, length1, max_gap, 1, v2, pos2);
                                if (pos2 == LENGTH) {   // 前方没车 则能加速则加速
                                    tollbooth[i][j].change = 1;
                                    tollbooth[i][j].accelerate = 1;
//                                    continue;
                                } else {
                                    gap_right_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now + 1, v2);
                                    if (gap_right_f >= tmp_gap2) { // 当前距离大于安全距离
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    } else if (gap_right_f >= tmp_gap && gap_right_f < tmp_gap2 &&
                                               i  < (B - L) / 2) {
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                    // 变道只有能加速才有意义
                                }
                                if (tollbooth[i][j].change == 1) {
                                    get_v_and_pos(tollbooth, i + 1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_right_b = j - pos2;
                                    tmp_gap = get_gap(max(v2-2,0), v_now + tollbooth[i][j].accelerate);
                                    if (gap_right_b < tmp_gap) {
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = 0;
                                    }else{
                                        continue;
                                    }
                                }
                            }
                            // 判断左变道
                            if (check_out(i - 1, j, B, L, length1)) {
                                get_v_and_pos(tollbooth, i - 1, j - 1, B, L, length1, max_gap, 1, v2, pos2);
                                if (pos2 == LENGTH) {   // 前方没车 则能加速则加速
                                    tollbooth[i][j].change = -1;
                                    tollbooth[i][j].accelerate = 1;
//                                    continue;
                                } else {
                                    gap_left_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now + 1, v2);
                                    if (gap_left_f >= tmp_gap2) { // 当前距离大于安全距离
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    } else if (gap_left_f >= tmp_gap && gap_left_f < tmp_gap2 && i >= (B + L) / 2) {
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
//                                    // 变道只有能加速才有意义
                                }

                                if (tollbooth[i][j].change == -1) {
                                    get_v_and_pos(tollbooth, i - 1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_left_b = j - pos2;
                                    tmp_gap = get_gap(max(v2-2,0) , v_now + tollbooth[i][j].accelerate);
                                    if (gap_left_b < tmp_gap) {
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = 0;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }

                        } else { // 当前道需要减速
                            tollbooth[i][j].accelerate = -2; // 默认结果
                            if (tollbooth[i][j].is_auto == 1){ continue;}
//                            if(j < length_tollbooth[i]-2 && (i < (B-L)/2 || i >= (B+L)/2)){
//                                continue;
//                            }
                            // 判断右车道
                            if (check_out(i + 1, j , B, L, length1)) {
                                get_v_and_pos(tollbooth, i + 1, j - 1, B, L, length1, max_gap, 1, v2, pos2);
                                if (pos2 == LENGTH) {   // 前方没车 则能加速则加速
                                    if (v_now < v_max) {
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    } else {
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                } else {
                                    gap_right_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now + 1, v2);
                                    tmp_gap3 = get_gap(max(v_now - 2,0), v2);
                                    if (gap_right_f >= tmp_gap2) { // 当前距离大于安全距离
                                        if (v_now < v_max) {
                                            tollbooth[i][j].change = 1;
                                            tollbooth[i][j].accelerate = 1;
//                                            continue;
                                        } else {
                                            tollbooth[i][j].change = 1;
                                            tollbooth[i][j].accelerate = 0;
//                                            continue;
                                        }
                                    } else if (gap_right_f >= tmp_gap && gap_right_f < tmp_gap2) {
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    } else if (gap_right_f >= tmp_gap3){
                                        if (i < (B - L) / 2) {
                                            tollbooth[i][j].change = 1;
                                            tollbooth[i][j].accelerate = -2;
//                                            continue;
                                        }
                                    }
                                }
                                // 变道只有能加速才有意义
                                if (tollbooth[i][j].v + tollbooth[i][j].accelerate < 0) {
//                                tollbooth[i][j].v = 0;
                                    tollbooth[i][j].accelerate = -tollbooth[i][j].v;
//                                printf("fuck3\n");
                                }
                                if (tollbooth[i][j].change == 1) {
                                    get_v_and_pos(tollbooth, i + 1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_right_b = j - pos2;
                                    tmp_gap = get_gap(max(v2-2,0), v_now + tollbooth[i][j].accelerate);
                                    if (gap_right_b < tmp_gap) {
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = -2;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }
                            if (tollbooth[i][j].v + tollbooth[i][j].accelerate < 0) {
//                                tollbooth[i][j].v = 0;
                                tollbooth[i][j].accelerate = -tollbooth[i][j].v;
//                                printf("fuck3\n");
                            }
                            // 判断左变道
                            if (check_out(i - 1, j, B, L, length1)) {
                                get_v_and_pos(tollbooth, i - 1, j - 1, B, L, length1, max_gap, 1, v2, pos2);
                                if (pos2 == LENGTH) {   // 前方没车 则能加速则加速
                                    if (v_now < v_max) {
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    } else {
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                } else {
                                    gap_left_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now + 1, v2);
                                    tmp_gap3 = get_gap(max(v_now - 2,0), v2);
                                    if (gap_left_f >= tmp_gap2) { // 当前距离大于安全距离
                                        if (v_now < v_max) {
                                            tollbooth[i][j].change = -1;
                                            tollbooth[i][j].accelerate = 1;
//                                            continue;
                                        } else {
                                            tollbooth[i][j].change = -1;
                                            tollbooth[i][j].accelerate = 0;
//                                            continue;
                                        }
                                    } else if (gap_left_f >= tmp_gap && gap_left_f < tmp_gap2) {
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    } else if (gap_left_f >= tmp_gap3){
                                        if (i  >= (B + L) / 2) {
                                            tollbooth[i][j].change = -1;
                                            tollbooth[i][j].accelerate = -2;
//                                            continue;
                                        }
                                    }
                                    // 变道只有能加速才有意义
                                }
                                if (tollbooth[i][j].v + tollbooth[i][j].accelerate < 0) {
//                                tollbooth[i][j].v = 0;
                                    tollbooth[i][j].accelerate = -tollbooth[i][j].v;
//                                printf("fuck3\n");
                                }

                                if (tollbooth[i][j].change == -1) {
                                    get_v_and_pos(tollbooth, i - 1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_left_b = j - pos2;

                                    tmp_gap = get_gap(max(v2-2,0), v_now + tollbooth[i][j].accelerate);
                                    if (gap_left_b < tmp_gap) {
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = -2;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }

                        }

                    }
                }
            }
            for(int i = B/2; i<B; i++){
                if(tollbooth[i][j].status == 1){
                    if(1.0*rand()/RAND_MAX < p2 && tollbooth[i][j].v>0 && tollbooth[i][j].is_auto == 0){
                        tollbooth[i][j].v--;

                    }
                    v_now = tollbooth[i][j].v;
                    max_gap = get_gap(v_now+1,0);
                    max_gap = 1000;
                    // 判断前方
                    get_v_and_pos(tollbooth,i,j,B,L,length1,max_gap,1,v2,pos2);
                    if(pos2 == LENGTH){   // 前方没车 则能加速则加速
                        if(v_now < v_max){
                            tollbooth[i][j].accelerate = 1;
                            continue;
                        }
                    }
                    else{
                        gap_now = pos2 - j;
                        tmp_gap = get_gap(v_now, v2, tollbooth[i][j].is_auto);
                        tmp_gap2 = get_gap(v_now+1, v2, tollbooth[i][j].is_auto);
                        if(gap_now >= tmp_gap2){ // 当前距离大于安全距离
                            if(v_now < v_max){
                                tollbooth[i][j].accelerate = 1;
                                continue;
                            }
                        }
                        else if(gap_now >= tmp_gap && gap_now <= tmp_gap2){ // 等于安全距离,保持不动,或者变道加速
                            if(v_now == v_max){ continue; } // 已经最高速则不变道,保持不变
                            if (tollbooth[i][j].is_auto == 1){ continue;}
//                            if(j < length_tollbooth[i]-2){
//                                continue;
//                            }
//                            if(j < length_tollbooth[i]-2 && (i < (B-L)/2 || i >= (B+L)/2)){
//                                continue;
//                            }
                            // 判断左变道
                            if(check_out(i-1,j,B,L,length1)) {
                                get_v_and_pos(tollbooth, i - 1, j-1, B, L, length1, max_gap, 1, v2, pos2);
                                if(pos2 == LENGTH){   // 前方没车 则能加速则加速
                                    tollbooth[i][j].change = -1;
                                    tollbooth[i][j].accelerate = 1;
//                                    continue;
                                }
                                else{
                                    gap_left_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now+1, v2);
                                    if(gap_left_f >= tmp_gap2){ // 当前距离大于安全距离
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    }
                                    else if(gap_left_f >= tmp_gap && gap_left_f < tmp_gap2 && i >= (B+L)/2) {
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
//                                    // 变道只有能加速才有意义
                                }

                                if(tollbooth[i][j].change == -1){
                                    get_v_and_pos(tollbooth, i - 1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_left_b = j - pos2;
                                    tmp_gap = get_gap(max(v2-2,0), v_now+tollbooth[i][j].accelerate);
                                    if(gap_left_b < tmp_gap){
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = 0;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }
                            // 判断右车道
                            if(check_out(i+1,j,B,L,length1)) {
                                get_v_and_pos(tollbooth, i + 1, j-1, B, L, length1, max_gap, 1, v2, pos2);
                                if(pos2 == LENGTH){   // 前方没车 则能加速则加速
                                    tollbooth[i][j].change = 1;
                                    tollbooth[i][j].accelerate = 1;
//                                    continue;
                                }
                                else{
                                    gap_right_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now+1, v2);
                                    if(gap_right_f >= tmp_gap2){ // 当前距离大于安全距离
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    }
                                    else if(gap_right_f >= tmp_gap && gap_right_f < tmp_gap2 && i < (B-L)/2) {
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                    // 变道只有能加速才有意义
                                }
                                if(tollbooth[i][j].change == 1){
                                    get_v_and_pos(tollbooth, i+1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_right_b = j - pos2;
                                    tmp_gap = get_gap(max(v2-2,0), v_now+tollbooth[i][j].accelerate);
                                    if(gap_right_b < tmp_gap){
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = 0;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }
                        }
                        else{ // 当前道需要减速
                            tollbooth[i][j].accelerate = -2; // 默认结果
                            if (tollbooth[i][j].is_auto == 1){ continue;}
                            // 判断左变道
                            if(check_out(i-1,j,B,L,length1)) {
                                get_v_and_pos(tollbooth, i - 1, j-1, B, L, length1, max_gap, 1, v2, pos2);
                                if(pos2 == LENGTH){   // 前方没车 则能加速则加速
                                    if(v_now < v_max) {
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    }
                                    else{
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                }
                                else{
                                    gap_left_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now+1, v2);
                                    tmp_gap3 = get_gap(max(v_now - 2, 0), v2);
                                    if(gap_left_f >= tmp_gap2){ // 当前距离大于安全距离
                                        if(v_now < v_max) {
                                            tollbooth[i][j].change = -1;
                                            tollbooth[i][j].accelerate = 1;
//                                            continue;
                                        }
                                        else{
                                            tollbooth[i][j].change = -1;
                                            tollbooth[i][j].accelerate = 0;
//                                            continue;
                                        }
                                    }
                                    else if(gap_left_f >= tmp_gap && gap_left_f < tmp_gap2){
                                        tollbooth[i][j].change = -1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                    else if(gap_left_f >= tmp_gap3){
                                        if ( i >= (B+L)/2 ) {
                                            tollbooth[i][j].change = -1;
                                            tollbooth[i][j].accelerate = -2;
//                                            continue;
                                        }
                                    }
                                    // 变道只有能加速才有意义
                                }
                                if(tollbooth[i][j].v + tollbooth[i][j].accelerate < 0){
//                                tollbooth[i][j].v = 0;
                                    tollbooth[i][j].accelerate = -tollbooth[i][j].v;
//                                printf("fuck3\n");
                                }

                                if(tollbooth[i][j].change == -1){
                                    get_v_and_pos(tollbooth, i-1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_left_b = j - pos2;

                                    tmp_gap = get_gap(max(v2-2,0), v_now+tollbooth[i][j].accelerate);
                                    if(gap_left_b < tmp_gap){
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = -2;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }
                            // 判断右车道
                            if(check_out(i+1,j,B,L,length1)) {
                                get_v_and_pos(tollbooth, i + 1, j-1, B, L, length1, max_gap, 1, v2, pos2);
                                if(pos2 == LENGTH){   // 前方没车 则能加速则加速
                                    if(v_now < v_max) {
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 1;
//                                        continue;
                                    }
                                    else{
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                }
                                else{
                                    gap_right_f = pos2 - j;
                                    tmp_gap = get_gap(v_now, v2);
                                    tmp_gap2 = get_gap(v_now+1, v2);
                                    tmp_gap3 = get_gap(max(v_now - 2,0), v2);
                                    if(gap_right_f >= tmp_gap2){ // 当前距离大于安全距离
                                        if(v_now < v_max) {
                                            tollbooth[i][j].change = 1;
                                            tollbooth[i][j].accelerate = 1;
//                                            continue;
                                        }
                                        else{
                                            tollbooth[i][j].change = 1;
                                            tollbooth[i][j].accelerate = 0;
//                                            continue;
                                        }
                                    }
                                    else if(gap_right_f >= tmp_gap && gap_right_f < tmp_gap2 ){
                                        tollbooth[i][j].change = 1;
                                        tollbooth[i][j].accelerate = 0;
//                                        continue;
                                    }
                                    else if(gap_right_f >= tmp_gap3){
                                        if ( i < (B-L)/2 ) {
                                            tollbooth[i][j].change = 1;
                                            tollbooth[i][j].accelerate = -2;
//                                            continue;
                                        }
                                    }
                                    // 变道只有能加速才有意义
                                }
                                if(tollbooth[i][j].v + tollbooth[i][j].accelerate < 0){
//                                tollbooth[i][j].v = 0;
                                    tollbooth[i][j].accelerate = -tollbooth[i][j].v;
//                                printf("fuck3\n");
                                }
                                if(tollbooth[i][j].change == 1){
                                    get_v_and_pos(tollbooth, i+1, j, B, L, length1, max_gap, 0, v2, pos2);
                                    gap_right_b = j - pos2;
                                    tmp_gap = get_gap(max(v2-2,0), v_now+tollbooth[i][j].accelerate);
                                    if(gap_right_b < tmp_gap){
                                        tollbooth[i][j].change = 0;
                                        tollbooth[i][j].accelerate = -2;
                                    }
                                    else{
                                        continue;
                                    }
                                }
                            }
                            if(tollbooth[i][j].v + tollbooth[i][j].accelerate < 0){
//                                tollbooth[i][j].v = 0;
                                tollbooth[i][j].accelerate = -tollbooth[i][j].v;
//                                printf("fuck3\n");
                            }
                        }

                    }

                }
            }
        }
    }
    else{
        return;
    }
    return;
}

int check_out(int x, int y, int B, int L, int length1) {
    // 检测当前位置是否合法
    if((x>=0 && x < B) && y < length_tollbooth[x]){
        return 1;
    }

    if(y >= LENGTH && length_tollbooth[x]==LENGTH){
        return -1;
    }
    return 0;
}

void make_sync2(Cell tollbooth[][LENGTH], int B, int L, int length1, Mat pic, Mat init) {
    // 根据当前的决策情况更新汽车移动, 初始化汽车决策数据
    int delta_length,delta_width;
    delta_length = LEN/LENGTH; //获取宽度于长度间隔
    delta_width = WIDTH/B;
    int tmp_x,tmp_y;
    Point2f p1,p2;
    for (int j = LENGTH-1; j >= 0; j--) {
        if(j > length1){
            for(int i = B/2-1; i >= (B-L)/2; i--){
                if(tollbooth[i][j].status == 0) continue;

                circle(pic, Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(0,0,255),3);
                p1 = Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2);
                tollbooth[i][j].v += tollbooth[i][j].accelerate;
                if(tollbooth[i][j].v < 0){
                    tollbooth[i][j].v = 0;
                }
                tmp_y = j + tollbooth[i][j].v;          //前进新位置
                tmp_x = i + tollbooth[i][j].change;     //变道到新道
                tollbooth[i][j].time++;
                if(tmp_x != i){
                    change_times++;
                }
                if(check_out(tmp_x, tmp_y, B, L, length1) == 1){ // 如果新位置合法,移动然后对状态清零

                    if(tollbooth[tmp_x][tmp_y].status == 1 && (tollbooth[i][j].v!=0 || (tollbooth[i][j].v == 0 && tollbooth[i][j].change != 0))){
                        boom++;
                        circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2+1)+5,Scalar(0,255,0),3);
                    }
                    tollbooth[tmp_x][tmp_y] = tollbooth[i][j];
                    tollbooth[tmp_x][tmp_y].change = 0;
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    tollbooth[i][j].status = 0;
                    circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(255,0,0),3);
                    p2 = Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2);
                    line(pic, p1, p2, Scalar(0, 0, 0));
//                    line(init, p1, p2, Scalar(255, 0, 0));
                }
                else if(check_out(tmp_x, tmp_y, B, L, length1) == -1){
                    flux++;
                    // tollbooth[i][j].time++;
                    tollbooth[tmp_x][tmp_y].change = 0;
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    tollbooth[i][j].status = 0;
                    total_time += tollbooth[i][j].time;
                    delay_time_min = min(delay_time_min, tollbooth[i][j].time);
                    delay_time_max = max(delay_time_max, tollbooth[i][j].time);
                    delay_time_car[tollbooth[i][j].time]++;
                }
                else{
                    printf("fuck!\n");
                    out++;
                }
            }
            for(int i = B/2; i < (B+L)/2; i++){
                if(tollbooth[i][j].status == 0) continue;

                circle(pic, Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(0,0,255),3);
                p1 = Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2);
                tollbooth[i][j].v += tollbooth[i][j].accelerate;
                if(tollbooth[i][j].v < 0){
                    tollbooth[i][j].v = 0;
                }
                tmp_y = j + tollbooth[i][j].v;          //前进新位置
                tmp_x = i + tollbooth[i][j].change;     //变道到新道
                tollbooth[i][j].time++;
                if(tmp_x != i){
                    change_times++;
                }
                if(check_out(tmp_x, tmp_y, B, L, length1) == 1){ // 如果新位置合法,移动然后对状态清零

                    if(tollbooth[tmp_x][tmp_y].status == 1 && (tollbooth[i][j].v!=0 || (tollbooth[i][j].v == 0 && tollbooth[i][j].change != 0))){
                        boom++;
                        circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2+1)+5,Scalar(0,255,0),3);
                    }

                    tollbooth[tmp_x][tmp_y] = tollbooth[i][j];
                    tollbooth[tmp_x][tmp_y].change = 0;
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    tollbooth[i][j].status = 0;
                    circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(255,0,0),3);
                    p2 = Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2);
                    line(pic, p1, p2, Scalar(0, 0, 0));
//                    line(init, p1, p2, Scalar(255, 0, 0));
                }
                else if(check_out(tmp_x, tmp_y, B, L, length1) == -1){
                    flux++;
                    tollbooth[i][j].time++;
                    tollbooth[tmp_x][tmp_y].change = 0;
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    tollbooth[i][j].status = 0;
                    total_time += tollbooth[i][j].time;
                    delay_time_min = min(delay_time_min, tollbooth[i][j].time);
                    delay_time_max = max(delay_time_max, tollbooth[i][j].time);
                    delay_time_car[tollbooth[i][j].time]++;
                }
                else{
                    printf("fuck!\n");
                    out++;
                }
            }
        }
        else{
            for(int i = B/2-1; i >= 0 ; i--) {
                if(tollbooth[i][j].status == 0) continue;
                circle(pic, Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(0,0,255),3);
                p1 = Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2);

                tollbooth[i][j].v += tollbooth[i][j].accelerate;
                if(tollbooth[i][j].v < 0){
                    tollbooth[i][j].v = 0;
                }

                tmp_y = j + tollbooth[i][j].v;          //前进新位置
                tmp_x = i + tollbooth[i][j].change;     //变道到新道
                tollbooth[i][j].time++;
                if(tmp_x != i){
                    change_times++;
                }

                if(check_out(tmp_x, tmp_y, B, L, length1) == 1){ // 如果新位置合法,移动然后对状态清零

                    if(tollbooth[tmp_x][tmp_y].status == 1 && (tollbooth[i][j].v!=0 || (tollbooth[i][j].v == 0 && tollbooth[i][j].change != 0))){
                        boom++;
                        circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2+2)+5,Scalar(0,255,0),3);
                    }
                    tollbooth[tmp_x][tmp_y] = tollbooth[i][j];
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    if(tollbooth[tmp_x][tmp_y].v == 0 && tollbooth[tmp_x][tmp_y].change == 0){continue;}
                    tollbooth[i][j].status = 0;
                    tollbooth[tmp_x][tmp_y].change = 0;
                    circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(255,0,0),3);
                    p2 = Point2f(tmp_y*delta_length+delta_length/2+10, tmp_x*delta_width+delta_width/2);
                    line(pic, p1, p2, Scalar(0, 0, 0));
//                    line(init, p1, p2, Scalar(255, 0, 0));
                }
                else if(check_out(tmp_x, tmp_y, B, L, length1) == -1){
                    flux++;
                    tollbooth[i][j].time++;
                    tollbooth[tmp_x][tmp_y].change = 0;
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    tollbooth[i][j].status = 0;
                    total_time += tollbooth[i][j].time;
                    delay_time_min = min(delay_time_min, tollbooth[i][j].time);
                    delay_time_max = max(delay_time_max, tollbooth[i][j].time);
                    delay_time_car[tollbooth[i][j].time]++;
                }
                else{
                    printf("fuck 2!\n");
                    out++;
                }
            }
            for(int i = B/2; i < B; i++){
                if(tollbooth[i][j].status == 0) continue;
                circle(pic, Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(0,0,255),3);
                p1 = Point2f(j*delta_length+delta_length/2, i*delta_width+delta_width/2);

                tollbooth[i][j].v += tollbooth[i][j].accelerate;
                if(tollbooth[i][j].v < 0){
                    tollbooth[i][j].v = 0;
                }

                tmp_y = j + tollbooth[i][j].v;          //前进新位置
                tmp_x = i + tollbooth[i][j].change;     //变道到新道
                tollbooth[i][j].time++;
                if(tmp_x != i){
                    change_times++;
                }

                if(check_out(tmp_x, tmp_y, B, L, length1) == 1){ // 如果新位置合法,移动然后对状态清零

                    if(tollbooth[tmp_x][tmp_y].status == 1 && (tollbooth[i][j].v!=0 || (tollbooth[i][j].v == 0 && tollbooth[i][j].change != 0))){
                        boom++;
                        circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2+2)+5,Scalar(0,255,0),3);
                    }
                    tollbooth[tmp_x][tmp_y] = tollbooth[i][j];
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    if(tollbooth[tmp_x][tmp_y].v == 0 && tollbooth[tmp_x][tmp_y].change == 0){continue;}
                    tollbooth[i][j].status = 0;
                    tollbooth[tmp_x][tmp_y].change = 0;
                    circle(pic, Point2f(tmp_y*delta_length+delta_length/2, tmp_x*delta_width+delta_width/2),min(delta_length/2-2 , delta_width/2-1),Scalar(255,0,0),3);
                    p2 = Point2f(tmp_y*delta_length+delta_length/2+10, tmp_x*delta_width+delta_width/2);
                    line(pic, p1, p2, Scalar(0, 0, 0));
//                    line(init, p1, p2, Scalar(255, 0, 0));
                }
                else if(check_out(tmp_x, tmp_y, B, L, length1) == -1){
                    flux++;
                    tollbooth[i][j].time++;
                    tollbooth[tmp_x][tmp_y].change = 0;
                    tollbooth[tmp_x][tmp_y].accelerate = 0;
                    tollbooth[i][j].status = 0;
                    total_time += tollbooth[i][j].time;
                    delay_time_min = min(delay_time_min, tollbooth[i][j].time);
                    delay_time_max = max(delay_time_max, tollbooth[i][j].time);
                    delay_time_car[tollbooth[i][j].time]++;
                }
                else{
                    printf("fuck 2!\n");
                    out++;
                }
            }
        }
    }
}

void rand_init(Cell tollbooth[][LENGTH], int B, int L, int length1){
    flux = 0;
    boom = 0;
    input = 0;
    out = 0;
    total_time = 0;
    change_times = 0;
    delay_time_max = 0;
    delay_time_min = 1000;
    double x,y;
    x = rand();
    for(int i=0; i<30; i++){
        delay_time_car[i] = 0;
    }

    for(int i=0; i<B; i++){
        delta_time[i] = 0;
        for(int j=0; j<LENGTH; j++){
            tollbooth[i][j].status = 0;
        }
    }
//    tollbooth[1][19].status = 1;
//    tollbooth[1][19].v = 2;
//    tollbooth[1][19].change = 0;
//    tollbooth[1][19].accelerate = 0;
//
//    tollbooth[5][14].status = 1;
//    tollbooth[5][14].v = 2;
//    tollbooth[5][14].change = 0;
//    tollbooth[5][14].accelerate = 0;
//
//    tollbooth[4][14].status = 1;
//    tollbooth[4][14].v = 2;
//    tollbooth[4][14].change = 0;
//    tollbooth[4][14].accelerate = 0;
//
//    tollbooth[3][19].status = 1;
//    tollbooth[3][19].v = 2;
//    tollbooth[3][19].change = 0;
//    tollbooth[3][19].accelerate = 0;
//
//    tollbooth[2][19].status = 1;
//    tollbooth[2][19].v = 2;
//    tollbooth[2][19].change = 0;
//    tollbooth[2][19].accelerate = 0;
//    for(int i=7;i<B;i++){
//        tollbooth[i][0].status = 1;
//        tollbooth[i][0].v = 0;
//        tollbooth[i][0].change = 0;
//        tollbooth[i][0].accelerate = 0;
//        tollbooth[i][0].time = 0;
//    }
}

void get_in_equal(Cell tollbooth[][LENGTH], int B, int L,FILE *fp){
    // 产生从入口进入的汽车 模型1中B个车道进入的车为等概率分布,初始为0速度

    int status[B];
    int n = 0,tmp;
    double tmp3;
    for(int i=0;i<B;i++) { // 判断那些口可以出车,距离上次出车必须一定时间以上
        if(delta_time[i] >= min_delta_time[i]) {
            status[i] = 1;
            n++;
        }
        else{
            status[i] = 0;
        }
        delta_time[i] += 1; // 等待时间增加1s
    }
    while( n>0 ) {
        tmp = rand()%B; // 随机判断出口
        if(status[tmp] == 0){
            continue;
        }
        status[tmp] = 0;
        n--;

        fscanf(fp,"%lf",&tmp3); //随机数判断是否可以出车
        if(tmp3 > p && tollbooth[tmp][0].status == 0) {
//            printf("rand %d\n",rand());
            input++;
            tollbooth[tmp][0].status = 1;
            tollbooth[tmp][0].v = 1;
            tollbooth[tmp][0].accelerate = 0;
            tollbooth[tmp][0].change = 0;
            tollbooth[tmp][0].time = 0;
            tollbooth[tmp][0].is_auto = 0;
            delta_time[tmp] = 0; // 出车窗口等待时间清0
            if(tmp >= (B-L)/2 && tmp < (B+L)/2) {
                if(1.0*rand()/RAND_MAX <= 0.3)
                tollbooth[tmp][0].is_auto = 1;
            }
        }
    }
}

int get_flux(){
    return(flux);
}

int get_gap(int v1, int v2, int is_auto){
    // 根据给定速度计算gap值
    // GAP = 1 + (V_1^2-V_2^2)/(2*da) + 2*gama * V_1;
    double gama = 0.4; // 公式中的系数
    int da = 2; // 减速加速度
    double gap = 1 + (v1*v1 - v2*v2)/(2*da) + 2 * gama * v1 + 0.5;
    if(is_auto){
        gap -= 2 * gama * v1;
    }
    return (int)gap + v1-v2;
}

int get_boom(){
    return boom;
}

int get_in(){
    return input;
}

int get_out(){
    return out;
}

int get_total_time(){
    return total_time;
}

int get_change_times(){
    return change_times;
}

void get_shape(FILE *fp,int B,int L,int length1){
    double tmp;
    for(int i=0; i<B; i++){
        fscanf(fp, "%lf", &tmp);
        if(tmp == -1){
            length_tollbooth[i] = LENGTH;
        }
        else{
            length_tollbooth[i] = (int)1.0*tmp*length1;
        }
    }
}

void GetInitMat(int B, int L, int length1, Mat& pic_tollbooth){
    //获得初始化表格图片,用于表示车道
    Mat tmp(WIDTH, LEN, CV_8UC3,Scalar::all(255));
    pic_tollbooth = tmp;
    int delta_length = LEN/LENGTH;
    int delta_width = WIDTH/B;
    for(int i=0; i<=LENGTH; i++){
        if(i <= length1) {
            line(pic_tollbooth, Point2f(i * delta_length, 0), Point2f(i * delta_length, WIDTH), Scalar(0, 0, 0));
        }
        else{
            line(pic_tollbooth, Point2f(i * delta_length, delta_width*(B-L)/2), Point2f(i * delta_length, delta_width*(B+L)/2), Scalar(0, 0, 0));
        }
    }

    for(int j=0; j<=B; j++){
        line(pic_tollbooth, Point2f(0, j * delta_width), Point2f(length_tollbooth[j]*delta_length, j * delta_width), Scalar(0, 0, 0));

    }

}

int get_delay_time_min(){
    return delay_time_min;
}
int get_delay_time_max(){
    return delay_time_max;
}
void output_delay_status(FILE *fp){
    for(int i=delay_time_min; i<=delay_time_max; i++){
        fprintf(fp, "%d, ",delay_time_car[i]);
    }
}

void get_min_delay_time(FILE *fp, int B){
    for(int i=0; i<B; i++){
        fscanf(fp,"%d",&min_delta_time[i]);
    }
    fclose(fp);
}
