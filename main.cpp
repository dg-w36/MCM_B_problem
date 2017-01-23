#include <iostream>
#include <stdio.h>
#include "model.h"
#include <opencv2/opencv.hpp>

#define WIDTH 300
#define LEN 1600
#define min(x,y) (x > y ? y : x)

using namespace cv;
using namespace std;

int get_remine(Cell tollbooth[][LENGTH], int B, int L, int length1){
    int remine = 0;
    for(int j=0 ; j<LENGTH ;j++){
        if(j > length1){
            for(int i = 0; i < L ;i++){
                if(tollbooth[i][j].status == 1){
                    remine++;
                }
            }
        }
        else{
            for(int i=0; i<B; i++){
                if(tollbooth[i][j].status == 1){
                    remine++;
                }
            }
        }
    }
    return remine;
}
int main() {
    FILE *fp = fopen("rand.txt","r");
    FILE *fp_data = fopen("data.txt","r");
    FILE *fp_shape, *fp_out;
    FILE *fp_p ;

    srand(unsigned(time(0)));

    int B, L, length1,n;
    B = 8;
    L = 4;
    length1 = 20;

    Mat init;
    Mat pic_tollbooth;
    int time, boom, flux, in, out,boom2;
    char filename1[100],filename2[100],picname[100];

    fscanf(fp_data,"%d", &n);
    for(int i=0;i<n;i++){
        printf(" model %d:  ",i);
        fscanf(fp_data, "%d %d %d" ,&B, &L, &length1);
        fscanf(fp_data, "%s", filename1); // for curve
        fscanf(fp_data, "%s", filename2); //output name
        fp_shape = fopen(filename1,"r");
        fp_p = fopen("pfile.txt","r");
        get_min_delay_time(fp_p, B);
        get_shape(fp_shape, B, L, length1);
        GetInitMat(B, L, length1, init);
        Cell tollbooth[B][LENGTH], bk_tollbooth[B][LENGTH];
        rand_init(tollbooth, B, L, length1);
        time = 0;

        while(time < 2000){
            imshow("white",init);

            init.copyTo(pic_tollbooth);
            make_choice(tollbooth, B, L, length1);
            make_sync2(tollbooth, B, L, length1,pic_tollbooth,init);
            get_in_equal(tollbooth, B, L, fp);

            imshow("white", pic_tollbooth);
            if (waitKey(10) == 27)
                break;
            time += 2;
        }
        sprintf(picname, "pic%d.jpg", i);
        imwrite(picname, init);
        printf(" done !!\n");
        fp_out = fopen(filename2, "w");
        boom = get_boom();
        flux = get_flux();
        in = get_in();
        out = get_out();

        fprintf(fp_out,"B = %d L = %d length1 = %d\n",B, L, length1);
        fprintf(fp_out,"shape_file = %s\n",filename1);
        fprintf(fp_out,"flux_real = %d\n", flux);
        fprintf(fp_out,"flux = %lf\nboom = %d\ntime = %d\nin = %d\n", 1.0*flux/in, boom, time, in);
        fprintf(fp_out,"out_sp = %d\n", out);
        fprintf(fp_out,"remain = %d\n out = %d\n", get_remine(tollbooth, B,L,length1), get_remine(tollbooth, B,L,length1)+get_boom()+get_flux());
        fprintf(fp_out,"delay_time = %lf\n", 1.0*get_total_time()/flux);
        fprintf(fp_out,"change time = %d\n", get_change_times());
        fprintf(fp_out,"delay_time_min = %d\n", get_delay_time_min());
        fprintf(fp_out,"delay_time_max = %d\n", get_delay_time_max());
        output_delay_status(fp_out);
        fclose(fp_out);
    }
    return 0;
}