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

void calc(vector <vector<struc>> &sixteen, Mat frame, Mat out, int T, double alpha, int vars, int times, Mat avg);////��training
void sorts(vector <vector<struc>> &one, int x, int y, int times);//�N�V�m���᪺codebook ��o
void update(Mat frame, vector <vector<struc>> &one, int x, int y, double alpha, int vars);//�קK���ɶ��Q�P�_�I���ӨS��s��pixelbase �b16*16�Q�P�_���I�����ɭԧ�spixel_base
void move(Mat mmove, Mat frame, vector <vector<struc>> &sixteen, int x, int y, int vars);////�ʺA��t�{�ɭ�
void ero(Mat in);//�I�k����
void fillhole(Mat in);//�ɬ}
void saveBG(vector <vector<struc>> &one, int x, int y,PATH path);//�s�I��
void drawrect(Mat black, PATH path,bigrecord &bigrec);//�e����ΰ�X�e��
void collectobject(bigrecord &bigrec, int currentframe, PATH path, fstream &log);//����txt�һݸ�T�ÿ�X
void  initialfolder(PATH path);//��l�Ƹ�Ƨ�
void savebin(bigrecord &bigrec, Mat  frame, Mat  out1, PATH path, fstream &bin);//�sbin
void countavgfirst(Mat avg16, Mat avg8, Mat avg4, Mat frame);
int CB();//Codebook�D�{��
//***
void indata(vector<group> &g);//�NtxtŪ�^
void findsublabelx(vector<group> &g, Mat out, int groupcount, int convertcount);//��Xgroup����group
void findsublabely(vector<group> &g1, Mat out, int groupcount, int convertcount);//��Xgroup����group
bool findxyandsort(vector<vector<objectuni>> &tempg);//labeling �����X�n�Χ�Xxt �٬Oyt �M�᭫�s�ƦC xsort flase ysort T
void getnewgroup(vector<group> &gold, vector<vector<objectuni>> &gnew);// ���s�ƦC
int compress(vector<vector<objectuni>> &tempg);
int compressy(vector<vector<objectuni>> &tempg);
void ReadInJpgtoMat(long st, long ed, cv::Mat &img, fstream &f_bin, int color);
void show(vector<vector<objectuni>> &tempg,int end);