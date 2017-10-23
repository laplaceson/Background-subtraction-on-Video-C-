#include <iostream>
#include <vector> 
#include<time.h>
#include<opencv2/highgui//highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <direct.h>
#include <stdio.h>
#include<fstream>
#include "type.h"
using namespace std;
using namespace cv;

void calc(vector <vector<struc>> &sixteen, Mat frame, Mat out, int T, double alpha, int vars, int times, Mat avg);////做training
void sorts(vector <vector<struc>> &one, int x, int y, int times);//將訓練之後的codebook 精緻
void update(Mat frame, vector <vector<struc>> &one, int x, int y, double alpha, int vars);//避免長時間被判斷背景而沒更新到pixelbase 在16*16被判斷成背景的時候更新pixel_base
void move(Mat mmove, Mat frame, vector <vector<struc>> &sixteen, int x, int y, int vars);////動態邊緣臨界值
void ero(Mat in);//侵蝕膨脹
void fillhole(Mat in);//補洞
void saveBG(vector <vector<struc>> &one, int x, int y,PATH path);//存背景
void drawrect(Mat black, PATH path,bigrecord &bigrec);//畫正方形圈出前景
void collectobject(bigrecord &bigrec, int currentframe, PATH path, fstream &log);//收集txt所需資訊並輸出
void  initialfolder(PATH path);//初始化資料夾
void savebin(bigrecord &bigrec, Mat  frame, Mat  out1, PATH path, fstream &bin);//存bin
void countavgfirst(Mat avg16, Mat avg8, Mat avg4, Mat frame);
int CB();//Codebook主程序
//***
void indata(vector<group> &g);//將txt讀回
void findsublabelx(vector<group> &g, Mat out, int groupcount, int convertcount);//找出group中的group
void findsublabely(vector<group> &g1, Mat out, int groupcount, int convertcount);//找出group中的group
bool findxyandsort(vector<vector<objectuni>> &tempg);//labeling 之後找出要用找出xt 還是yt 然後重新排列 xsort flase ysort T
void getnewgroup(vector<group> &gold, vector<vector<objectuni>> &gnew);// 重新排列
int compress(vector<vector<objectuni>> &tempg);
int compressy(vector<vector<objectuni>> &tempg);
void ReadInJpgtoMat(long st, long ed, cv::Mat &img, fstream &f_bin, int color);
void show(vector<vector<objectuni>> &tempg,int end);