#include "function.h"
void calc(vector <vector<struc>> &sixteen, Mat frame, Mat out, int T, double alpha, int vars, int times,Mat avg)//做training
{
	bool similar = false; int tempx = 0; int tempy = 0;
	float totalR = 0, totalG = 0, totalB = 0; float weight = (1. / T);
	int x = frame.rows>>times;
	int y = frame.cols>>times;
	vector<struc>::iterator iter;
	struc s;
	int temp = 0;

	//for (int i = 0, temp = 0; i < x; i += times)
	//{
	//	for (int j = 0; j < y; j += times, temp++)
	//	{
	//		similar = false;
	//		tempx = i; tempy = j; totalR = 0; totalG = 0; totalB = 0;
	//		for (int m = tempx; m < tempx + times; m++)//家總要取平均
	//		{
	//			for (int n = tempy; n < tempy + times; n++)
	//			{
	//				totalB += frame.ptr<uchar>(m, n)[0];
	//				totalG += frame.ptr<uchar>(m, n)[1];
	//				totalR += frame.ptr<uchar>(m, n)[2];
	//			}
	//		}
	//		s.B = totalB / (times*times);
	//		s.G = totalG / (times*times);
	//		s.R = totalR / (times*times);//取出當前像素平均值 
	//		iter = sixteen[temp].begin();
	//		for (iter; iter != sixteen[temp].end(); iter++)//掃codeword
	//		{
	//			if (((s.R - iter->R)*(s.R - iter->R) + (s.G - iter->G)*(s.G - iter->G) + (s.B - iter->B)*(s.B - iter->B)) < 3 * vars)//自訂閥值
	//			{//若相似 更新
	//				similar = true;
	//				iter->R = s.R*alpha + iter->R*(1 - alpha);
	//				iter->G = s.G*alpha + iter->G*(1 - alpha);
	//				iter->B = s.B*alpha + iter->B*(1 - alpha);
	//				iter->weight += weight;
	//				break;
	//			}
	//		}
	//		if (!similar)//沒有相似的 新增
	//		{
	//			s.weight = weight;
	//			sixteen[temp].push_back(s);
	//			out.ptr<uchar>(i / times)[j / times] = 255;
	//		}
	//	}
	//}
	for (int i = 0, temp = 0; i < x ; ++i)//將剛剛算好的以不同大小的區塊取平均值的圖 與現有模板做比較訓練
	{
		for (int j = 0; j < y; ++j,++temp)
		{		
			similar = false;
			s.R = avg.ptr<uchar>(i, j)[2]; s.G = avg.ptr<uchar>(i, j)[1]; s.B = avg.ptr<uchar>(i, j)[0];//取出剛剛算好的平均值的模板平均值
			iter = sixteen[temp].begin();
			for (iter; iter != sixteen[temp].end(); iter++)//掃codeword 與codebook內所有可能的平均值做比對
			{
				if (((s.R - iter->R)*(s.R - iter->R) + (s.G - iter->G)*(s.G - iter->G) + (s.B - iter->B)*(s.B - iter->B)) < 3 * vars)//自訂閥值
				{//若相似 更新很像的那顆codeword
					similar = true;
					iter->R = s.R*alpha + iter->R*(1 - alpha);
					iter->G = s.G*alpha + iter->G*(1 - alpha);
					iter->B = s.B*alpha + iter->B*(1 - alpha);
					iter->weight += weight;
					break;
				}
			}
			if (!similar)//沒有相似的 新增一顆codeword
			{
				s.weight = weight;
				sixteen[temp].push_back(s);
				out.ptr<uchar>(i)[j] = 255;
			}
		}
	}
	/*for (int i = 0; i < x / times; i++)//檢查用
	{
	for (int j = 0; j < y / times; j++)
	{
	cout << sixteen[y / times * i + j].size();

	}cout << endl;
	}*/
}
void sorts(vector <vector<struc>> &one, int x, int y, int times)
{
	vector<struc>::iterator iters;
	int temp = 0;
	for (int i = 0, temp = 0; i < x; i += times)//codebook 精緻化(重新排列之後只取出線機率前70%的背景 濾除可能誤判之前景)
	{
		for (int j = 0; j < y; j += times, temp++)
		{
			if (one[temp].size() != 1)
			{
				sort(one[temp].begin(), one[temp].end());//vector sort 將codebook內依照weight 排序
				iters = one[temp].begin();
				float add = 0;
				//
				for (iters; iters != one[temp].end();)//codeword精緻化
				{
					add += iters->weight;
					if (add < 0.7)
					{
						iters++;
					}
					else
					{
						one[temp].erase(iters + 1, one[temp].end()); break;//刪掉加總>0.7之後的
					}
				}

			}
		}
	}

}
void update(Mat frame, vector <vector<struc>> &one, int x, int y, double alpha, int vars)//在16*16被判斷成背景的時候更新pixel_base
{
	int temp = 0; struc s; int realy = frame.cols;
	vector<struc>::iterator iter;
	for (int i = x; i < x + 16; i++)
	{
		for (int j = y; j < y + 16; j++)
		{
			temp = realy*i + j;
			s.B = frame.ptr<uchar>(i, j)[0];
			s.G = frame.ptr<uchar>(i, j)[1];
			s.R = frame.ptr<uchar>(i, j)[2];
			iter = one[temp].begin();
			for (iter; iter != one[temp].end(); iter++)
			{
				if (((s.R - iter->R)*(s.R - iter->R) + (s.G - iter->G)*(s.G - iter->G) + (s.B - iter->B)*(s.B - iter->B)) < vars * 3)
				{//若相似就更新
					iter->R = s.R*alpha + iter->R*(1 - alpha);
					iter->G = s.G*alpha + iter->G*(1 - alpha);
					iter->B = s.B*alpha + iter->B*(1 - alpha);
					break;
				}
			}
		}
	}
}
void move(Mat mmove, Mat frame, vector <vector<struc>> &sixteen, int x, int y, int vars)//動態邊緣臨界值
{
	mmove.setTo(0);
	int temp = 0; float totalB = 0, totalG = 0, totalR = 0;
	vector<struc>::iterator iter;
	for (int i = 0; i < x; i += 16)
	{
		for (int j = 0; j < y; j += 16)
		{
			temp = (y  * i >> 8) + (j >> 4);
			totalB = 0, totalG = 0, totalR = 0;
			for (int m = i; m < i + 16; m++)
			{
				for (int n = j; n < j + 16; n++)
				{
					totalB += frame.ptr<uchar>(m, n)[0];
					totalG += frame.ptr<uchar>(m, n)[1];
					totalR += frame.ptr<uchar>(m, n)[2];
				}
			}
			totalB /= 256; totalG /= 256; totalR /= 256;
			iter = sixteen[temp].begin();
			for (iter; iter != sixteen[temp].end(); iter++)//掃codeword
			{
				if (((totalR - iter->R)*(totalR - iter->R) + (totalG - iter->G)* (totalG - iter->G) + (totalB - iter->B)*(totalB - iter->B)) > 3 * vars)//自訂閥值
				{//若"""不"""相似		為前景
					mmove.ptr<uchar>(i >> 4)[j >> 4] = 255; break;
				}
			}
		}
	}
	for (int i = 1; i < (x >> 4) - 1; i++)
	{
		for (int j = 1; j < (y >> 4) - 1; j++)
		{
			if (mmove.ptr<uchar>(i)[j] == 255)
			{
				if (mmove.ptr<uchar>(i - 1)[j] == 0){ mmove.ptr<uchar>(i - 1)[j] = 128; }
				if (mmove.ptr<uchar>(i + 1)[j] == 0){ mmove.ptr<uchar>(i + 1)[j] = 128; }
				if (mmove.ptr<uchar>(i)[j + 1] == 0){ mmove.ptr<uchar>(i)[j + 1] = 128; }
				if (mmove.ptr<uchar>(i)[j - 1] == 0){ mmove.ptr<uchar>(i)[j - 1] = 128; }
			}
		}
	}
	//接下來在進入正式判斷的時候檢查Mat值來決定閥值
}
void ero(Mat in)
{
	Mat temp;
	dilate(in, temp, Mat(), Point(-1, -1), 1, 1, 1);
	erode(temp, in, Mat(), Point(-1, -1), 1, 1, 1);

}
void fillhole(Mat in)//沒用]
{
	Mat th;
	threshold(in, th, 150, 255, THRESH_BINARY);
	Mat im_floodfill = th.clone();//copy
	floodFill(im_floodfill, cv::Point(0, 0), Scalar(255));
	bitwise_not(im_floodfill, in);
	in = (th | in);
}
void saveBG(vector <vector<struc>> &one, int x, int y, PATH path)
{
	Mat mtemp(x, y, CV_8UC3, Scalar(0));
	int temp = 0;
	vector<struc>::iterator iter;
	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			temp = y*i + j;
			mtemp.ptr<uchar>(i, j)[0] = one[temp][0].B;
			mtemp.ptr<uchar>(i, j)[1] = one[temp][0].G;
			mtemp.ptr<uchar>(i, j)[2] = one[temp][0].R;
		}
	}
	imwrite(path.BGpath, mtemp);
}
void drawrect(Mat black, PATH path, bigrecord &bigrec)
{
	Mat blackcopy = black.clone();
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	bigrec.rec.clear();

	//labeling
	findContours(blackcopy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//input 二值化影像 輸出所有邊緣輪廓 以及各個輪廓的點 
	bigrec.rec.resize(contours.size());

	////
	int cnt = 0;
	//抓邊框
	vector<Point>::iterator iter2;
	for (int i = 0; i < contours.size(); i++){
		if (contourArea(contours[i], false) < 300) continue;//面積過濾 false=>不論輪廓方向都返回正的面積值 如果設為ture的話除了面積還會記錄方向

		iter2 = contours[i].begin();
		bigrec.rec[cnt].minx = iter2->x;
		bigrec.rec[cnt].miny = iter2->y;
		for (iter2; iter2 != contours[i].end(); iter2++)
		{
			if (bigrec.rec[cnt].maxx < iter2->x)
			{
				bigrec.rec[cnt].maxx = iter2->x;
			}
			else if (bigrec.rec[cnt].minx > iter2->x)
			{
				bigrec.rec[cnt].minx = iter2->x;
			}
			if (bigrec.rec[cnt].maxy < iter2->y)
			{
				bigrec.rec[cnt].maxy = iter2->y;
			}
			else if (bigrec.rec[cnt].miny > iter2->y)
			{
				bigrec.rec[cnt].miny = iter2->y;
			}
		}
		cnt++;
	}
	bigrec.rec.resize(cnt);
	//畫邊框
	for (int i = 0; i < bigrec.rec.size(); i++){
		rectangle(black, Point(bigrec.rec[i].minx, bigrec.rec[i].miny), Point(bigrec.rec[i].maxx, bigrec.rec[i].maxy), rng.uniform(0, 255), 1, 4);
	}

}
void collectobject(bigrecord &bigrec, int currentframe, PATH path, fstream &log)
{//接下來收集參數並輸出

	if (bigrec.rec.size() == 0){
		bigrec.isset = false;	bigrec.start = 0;
		bigrec.yn = 0;
	}
	else bigrec.moveframe++;//是第幾個"前景"

	vector<record>::iterator iter2 = (bigrec.rec).begin();
	for (iter2; iter2 != bigrec.rec.end(); iter2++)
	{
		if (!bigrec.isset){
			if (bigrec.yn == 0 && bigrec.rec.size() > 0)
			{ //符合新增時間點
				bigrec.yn = 1;
				bigrec.start = currentframe;
				bigrec.isset = true;
			}

		}
		else
		{ //還在時間點內
			bigrec.yn = 0;
			bigrec.start = 0;
		}

		//--------------------印
		iter2->bcnum = 1;
		bigrec.currentframe = currentframe;
		log << bigrec.yn << "\t"
			<< bigrec.start << "\t"
			<< iter2->bcnum << "\t"
			<< iter2->minx << "\t"
			<< iter2->miny << "\t"
			<< iter2->maxx << "\t"
			<< iter2->maxy << "\t"
			<< bigrec.currentframe << "\t"
			<< bigrec.moveframe << "\t"
			<< iter2->colorLength[0] << "\t"
			<< iter2->colorLength[1] << "\t"
			<< iter2->boolLength[0] << "\t"
			<< iter2->boolLength[1] << "\n";
	}

}
void savebin(bigrecord &bigrec, Mat frame, Mat black, PATH path, fstream &bin)
{

	vector<record> ::iterator iter2;
	iter2 = bigrec.rec.begin();
	for (iter2; iter2 != bigrec.rec.end(); iter2++)
	{
		//fstream bin(path.binpath, ios::in | ios::out | ios::binary);
		bin.seekp(0, ios::end);
		iter2->colorLength[0] = bin.tellp().seekpos();

		Mat temp = frame(Rect(iter2->minx, iter2->miny, (iter2->maxx - iter2->minx + 1), (iter2->maxy - iter2->miny + 1)));

		vector<unsigned char> buff;
		imencode(".jpg", temp, buff);
		bin.write((char*)&buff[0], buff.size()*sizeof(unsigned char));

		iter2->colorLength[1] = bin.tellp().seekpos();
		bin.seekp(0, ios::end);
		iter2->boolLength[0] = bin.tellp().seekpos();
		buff.clear();

		temp = black(Rect(iter2->minx, iter2->miny, (iter2->maxx - iter2->minx + 1), (iter2->maxy - iter2->miny + 1)));
		imencode(".jpg", temp, buff);
		bin.write((char*)&buff[0], buff.size()*sizeof(unsigned char));

		iter2->boolLength[1] = bin.tellp().seekpos();

		//bin.close();
	}

}
void  initialfolder(PATH path)
{
	if (_mkdir(path.folderpath) == 0)	printf("create  folder sucess \n");
	else printf("Data folder already exist \n");

	fstream log(path.txtpath, ios::in | ios::out | ios::trunc);
	log.close();
	fstream bin(path.binpath, ios::in | ios::out | ios::trunc);
	bin.close();
}
void countavgfirst(Mat avg16, Mat avg8, Mat avg4, Mat frame)
{
	for (int i = 0; i < frame.rows; i+=4)//將進入的frame 取區塊平均
	{
		for (int j = 0; j < frame.cols; j+=4)
		{
			float total4R = 0,total4G=0,total4B=0;
			for (int m = 0; m < 4; ++m)
			{
				for (int n = 0; n < 4; ++n)
				{
					total4B += frame.ptr<uchar>(i + m, j + n)[0];
					total4G += frame.ptr<uchar>(i + m, j + n)[1];
					total4R += frame.ptr<uchar>(i + m, j + n)[2];
				}
			}
			total4R *= 0.0625;total4G *= 0.0625;total4B *= 0.0625;
			avg4.ptr<uchar>(i >> 2, j >> 2)[2] = total4R;
			avg4.ptr<uchar>(i >> 2, j >> 2)[1] = total4G;
			avg4.ptr<uchar>(i >> 2, j >> 2)[0] = total4B;
		}
	}
	for (int i = 0; i < frame.rows*0.25; i+=2)
	{
		for (int j = 0; j < frame.cols*0.25; j+=2)
		{
			avg8.ptr<uchar>(i >> 1, j >> 1)[2] = (avg4.ptr<uchar>(i, j)[0] + avg4.ptr<uchar>(i + 1, j)[0] + avg4.ptr<uchar>(i, j + 1)[0] + avg4.ptr<uchar>(i + 1, j + 1)[0])*0.25;
			avg8.ptr<uchar>(i >> 1, j >> 1)[1] = (avg4.ptr<uchar>(i, j)[1] + avg4.ptr<uchar>(i + 1, j)[1] + avg4.ptr<uchar>(i, j + 1)[1] + avg4.ptr<uchar>(i + 1, j + 1)[1])*0.25;
			avg8.ptr<uchar>(i >> 1, j >> 1)[0] = (avg4.ptr<uchar>(i, j)[2] + avg4.ptr<uchar>(i + 1, j)[2] + avg4.ptr<uchar>(i, j + 1)[2] + avg4.ptr<uchar>(i + 1, j + 1)[2])*0.25;
		}
	}
	for (int i = 0; i < frame.rows*0.125; i += 2)
	{
		for (int j = 0; j < frame.cols*0.125; j += 2)
		{
			avg16.ptr<uchar>(i >> 1, j >> 1)[0] = ( avg8.ptr<uchar>(i, j)[0] + avg8.ptr<uchar>(i + 1, j)[0] + avg8.ptr<uchar>(i, j + 1)[0] + avg8.ptr<uchar>(i + 1, j + 1)[0])*0.25;
			avg16.ptr<uchar>(i >> 1, j >> 1)[1] = (avg8.ptr<uchar>(i, j)[1] + avg8.ptr<uchar>(i + 1, j)[1] + avg8.ptr<uchar>(i, j + 1)[1] + avg8.ptr<uchar>(i + 1, j + 1)[1])*0.25;
			avg16.ptr<uchar>(i >>1, j >> 1)[2] = (avg8.ptr<uchar>(i, j)[2] + avg8.ptr<uchar>(i + 1, j)[2] + avg8.ptr<uchar>(i, j + 1)[2] + avg8.ptr<uchar>(i + 1, j + 1)[2])*0.25;
		}
	}
}
int CB()
{// 16*16 8*8 4*4 1*1 計算平均要先算好 (現在是沒先算好 計算會很慢 /******

	clock_t c1, c2; c1 = clock();
	const int T = 90; const int vars = 25; float alpha = 0.05;//T 訓練frame數 vars 閥值 alpha 學習率
	VideoCapture c("Square.avi");
	const int x = c.get(CV_CAP_PROP_FRAME_HEIGHT);
	const int y = c.get(CV_CAP_PROP_FRAME_WIDTH);

	Mat frame;//不同大小的板模(印出來看的
	Mat out16(x >> 4, y >> 4, CV_8UC1, Scalar(0));
	Mat out8(x >> 3, y >> 3, CV_8UC1, Scalar(0));
	Mat out4(x >> 2, y >> 2, CV_8UC1, Scalar(0));
	Mat out1(x, y, CV_8UC1, Scalar(0));
	Mat mmove(x >> 4, y >> 4, CV_8UC1, Scalar(0));

	//namedWindow("my_window");
	//namedWindow("my_window16");
	//namedWindow("my_window8");
	//namedWindow("my_window4");
	namedWindow("my_window1");

	vector <vector<struc>> one(x * y);//建立四個背景板模
	vector <vector<struc>> four((x * y) >> 4);
	vector <vector<struc>> eight((x* y) >> 6);
	vector <vector<struc>> sixteen((x * y) >> 8);

	vector<struc>::iterator iter16;
	vector<struc>::iterator iter8;
	vector<struc>::iterator iter4;
	vector<struc>::iterator iter1;

	

	Mat avg16(x >> 4, y >> 4, CV_8UC3, Scalar(0));
	Mat avg8(x >> 3, y >> 3, CV_8UC3, Scalar(0));
	Mat avg4(x >> 2, y >> 2, CV_8UC3, Scalar(0));

	//vector<record> tmprec;
	bigrecord bigrec;

	struc s;

	PATH path;
	strcpy_s(path.folderpath, "..//Data\\");
	strcpy_s(path.BGpath, "..//Data\\BG.jpg");
	strcpy_s(path.txtpath, "..//Data\\Object.txt");
	strcpy_s(path.binpath, "..//Data\\ImageBin.bin");
	initialfolder(path);

	fstream bin(path.binpath, ios::in | ios::out | ios::binary);//開啟bin 輸出串流

	fstream log(path.txtpath, ios::in | ios::out | ios::app);//開啟txtx輸出串流
	if (!log){//
		cout << "Fail to open txt: " << endl;
	}

	int temp16 = 0;
	int count = 0;
	int tempvar = 0;
	int framecount = c.get(CV_CAP_PROP_FRAME_COUNT);
	//***
	if (!c.isOpened())return -1;
	for (int count = 0; count < framecount; ++count)//前置作業完成 開始讀frame
	{
		c >> frame;
		if (frame.empty()) break;

		cout << count << " " << framecount << endl;;
		out16.setTo(0); out8.setTo(0); out4.setTo(0); out1.setTo(0);
		

		if (count < T)//training 四個model各自更新
		{
			countavgfirst(avg16, avg8, avg4, frame);
			calc(sixteen, frame, out16, T, alpha, vars,4,avg16);
			calc(eight, frame, out8, T, alpha, vars,3, avg8);
			calc(four, frame, out4, T, alpha, vars,2, avg4);
			calc(one, frame, out1, T, alpha, vars, 0, frame);
			
		}
		else if (count == T){//==90 做精緻化
			sorts(sixteen, x, y, 16);
			sorts(eight, x, y, 8);
			sorts(four, x, y, 4);
			sorts(one, x, y, 1);
			//initialfolder(path);//初始化資料夾 
			saveBG(one, x, y, path);//存背景
			c2 = clock();
			//{ cout << T / ((c2 - c1) / (double)(CLOCKS_PER_SEC)); break; }
		}
		else
		{//testing
			float total16R = 0, total16G = 0, total16B = 0; bool similar16 = false;
			move(mmove, frame, sixteen, x, y, vars);//動態邊緣 建立Mat比對是否為邊緣 
			for (int i16 = 0; i16 < x; i16 += 16)
			{
				for (int j16 = 0; j16 < y; j16 += 16)//掃描當前frame
				{
					temp16 = (y  * i16 >> 8) + (j16 >> 4);//將二維位置換算成一維位置
					total16R = 0; total16G = 0; total16B = 0;
					for (int m16 = i16; m16 < i16 + 16; ++m16)
					{
						for (int n16 = j16; n16 < j16 + 16; ++n16)//取區塊平均
						{
							total16B += frame.ptr<uchar>(m16, n16)[0];
							total16G += frame.ptr<uchar>(m16, n16)[1];
							total16R += frame.ptr<uchar>(m16, n16)[2];
						}
					}
					similar16 = false;
					total16B /= 256;
					total16G /= 256;
					total16R /= 256;//取平均值

					if (mmove.ptr<uchar>(i16 >> 4)[j16 >> 4] == 128){ tempvar = 9; }//動態邊緣臨界閥值 若是邊緣降低閥值 使更容易進入小模組進行判斷
					else  tempvar = vars;

					iter16 = sixteen[temp16].begin();
					for (iter16; iter16 != sixteen[temp16].end(); ++iter16)//掃codeword
					{
						if (((total16R - iter16->R)*(total16R - iter16->R) + (total16G - iter16->G)*(total16G - iter16->G) + (total16B - iter16->B)*(total16B - iter16->B)) < tempvar * 3)//自訂閥值
						{//若相似就更新 並且進入update(16*16的再次更新機制 為了以防一直被判斷成背景造成1*1 長時間沒有被更新到)
							similar16 = true;
							iter16->R = total16R*alpha + iter16->R * (1 - alpha);
							iter16->G = total16G*alpha + iter16->G * (1 - alpha);
							iter16->B = total16B*alpha + iter16->B * (1 - alpha);

							if (iter16->count < 9){ iter16->count += 1; }
							else{ iter16->count = 0; update(frame, one, i16, j16,  alpha, vars); }
							break;
						}
					}
					if (!similar16)//如果不是背景
					{//若不相似則往下找8*8
						//out16.ptr<uchar>(i16 / 16)[j16 / 16] = 255;
						//***
						int temp8 = 0;//換算vector的第幾格
						float total8R = 0, total8G = 0, total8B = 0; bool similar8 = false;
						for (int i8 = i16, p8 = 0; p8 <= 1; i8 += 8, ++p8)
						{
							for (int j8 = j16, q8 = 0; q8 <= 1; j8 += 8, ++q8)
							{
								temp8 = (y  * i8 >> 6) + (j8 >> 3);//換算vector的第幾格
								total8R = 0, total8G = 0, total8B = 0;
								for (int m8 = i8; m8 < i8 + 8; m8++)//算平均
								{
									for (int n8 = j8; n8 < j8 + 8; n8++)
									{
										total8B += frame.ptr<uchar>(m8, n8)[0];
										total8G += frame.ptr<uchar>(m8, n8)[1];
										total8R += frame.ptr<uchar>(m8, n8)[2];
									}
								}
								similar8 = false;
								total8B /= 64;
								total8G /= 64;
								total8R /= 64;
								iter8 = eight[temp8].begin();
								for (iter8; iter8 != eight[temp8].end(); ++iter8)
								{
									if (((total8R - iter8->R)*(total8R - iter8->R) + (total8G - iter8->G)*(total8G - iter8->G) + (total8B - iter8->B)*(total8B - iter8->B)) < vars * 3)
									{//若相似就更新
										similar8 = true;
										iter8->R = total8R*alpha + iter8->R*(1 - alpha);
										iter8->G = total8G*alpha + iter8->G*(1 - alpha);
										iter8->B = total8B*alpha + iter8->B*(1 - alpha);
										break;
									}
								}
								if (!similar8)
								{//若不相似則往下找4*4
									//out8.ptr<uchar>(i8 / 8)[j8 / 8] = 255;
									//******
									int temp4 = 0;//換算vector的第幾格
									float total4R = 0, total4G = 0, total4B = 0; bool similar4 = false;
									for (int i4 = i8, p4 = 0; p4 <= 1; i4 += 4, ++p4)
									{
										for (int j4 = j8, q4 = 0; q4 <= 1; j4 += 4, ++q4)
										{
											temp4 = (y  * i4 >> 4) + (j4 >> 2);//換算vector的第幾格

											total4R = 0, total4G = 0, total4B = 0;
											for (int m4 = i4; m4 < i4 + 4; m4++)//算平均
											{
												for (int n4 = j4; n4 < j4 + 4; n4++)
												{
													total4B += frame.ptr<uchar>(m4, n4)[0];
													total4G += frame.ptr<uchar>(m4, n4)[1];
													total4R += frame.ptr<uchar>(m4, n4)[2];
												}
											}
											similar4 = false;
											total4B /= 16;
											total4G /= 16;
											total4R /= 16;
											iter4 = four[temp4].begin();
											for (iter4; iter4 != four[temp4].end(); ++iter4)
											{
												if (((total4R - iter4->R)*(total4R - iter4->R) + (total4G - iter4->G)* (total4G - iter4->G) + (total4B - iter4->B)*(total4B - iter4->B)) < vars * 3)
												{//若相似就更新
													similar4 = true;
													iter4->R = total4R*alpha + iter4->R*(1 - alpha);
													iter4->G = total4G*alpha + iter4->G*(1 - alpha);
													iter4->B = total4B*alpha + iter4->B*(1 - alpha);
													break;
												}
											}
											if (!similar4)
											{//若不相似則往下找1*1
												//out4.ptr<uchar>(i4 / 4)[j4 / 4] = 255;
												//**
												int temp1 = 0;//換算vector的第幾格
												bool similar1 = false;
												for (int i1 = i4; i1 < i4 + 4; ++i1)
												{
													for (int j1 = j4; j1 < j4 + 4; ++j1)
													{
														temp1 = y * i1 + j1;//換算vector的第幾格

														similar1 = false;
														s.B = frame.ptr<uchar>(i1, j1)[0];
														s.G = frame.ptr<uchar>(i1, j1)[1];
														s.R = frame.ptr<uchar>(i1, j1)[2];
														iter1 = one[temp1].begin();
														for (iter1; iter1 != one[temp1].end(); ++iter1)//掃描pixelbase的codeword
														{
															if (((s.R - iter1->R)*(s.R - iter1->R) + (s.G - iter1->G)*(s.G - iter1->G) + (s.B - iter1->B)* (s.B - iter1->B)) < vars * 3)
															{//若相似就更新
																similar1 = true;
																iter1->R = s.R*alpha + iter1->R*(1 - alpha);
																iter1->G = s.G*alpha + iter1->G*(1 - alpha);
																iter1->B = s.B*alpha + iter1->B*(1 - alpha);
																break;
															}//若不相似則為真正前景 做光影模型濾除強光陰影
															float absp = sqrt(iter1->R*iter1->R + iter1->G*iter1->G + iter1->B*iter1->B);
															float absin = s.R*s.R + s.G*s.G + s.B*s.B;
															float abshadow = (s.R*iter1->R + s.G*iter1->G + s.B*iter1->B) / absp;//投影向量要界介於一定範圍
															float dist = sqrt(abs(absin - abshadow*abshadow));//dist
															float theta = dist / abshadow;//角度差異

															if (theta > 0.03 || abshadow > 1.25*absp || abshadow < 0.7*absp)//濾光影
															{
																out1.ptr<uchar>(i1)[j1] = 255;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

			}
			ero(out1);//做影像段開
			
			//fillhole(out1);//影像補洞
			threshold(out1, out1, 200, 255, THRESH_BINARY);//可不要
			drawrect(out1, path, bigrec);//抓取前景ROI
			savebin(bigrec, frame, out1, path, bin);//存前景進入bin
			collectobject(bigrec, count, path, log);//收集所需參數輸出至txt

		}
		//imshow("my_window", frame);
		//imshow("my_window16", out16);
		//imshow("my_window8", out8);
		//imshow("my_window4", out4);

		imshow("my_window1", out1);
		if (cvWaitKey(1) >= 0) break;

	}

	bin.close();
	log.close();
	return 0;
}
//***********
void indata(vector<group> &g)
{
	char buffer[100];
	int tmp[9] = { 0 };
	long tmpl[4] = { 0 };

	fstream fin;
	fin.open("..//Data\\Object.txt", fstream::in);


	objectuni obj;
	int count = 0;
	group gg;
	while (1)
	{
		fin >> buffer;
		tmp[0] = atoi(buffer);
		fin >> buffer;
		tmp[1] = atoi(buffer);
		fin >> buffer;
		tmp[2] = atoi(buffer);
		fin >> buffer;
		tmp[3] = atoi(buffer);
		fin >> buffer;
		tmp[4] = atoi(buffer);
		fin >> buffer;
		tmp[5] = atoi(buffer);
		fin >> buffer;
		tmp[6] = atoi(buffer);
		fin >> buffer;
		tmp[7] = atoi(buffer);
		fin >> buffer;
		tmp[8] = atoi(buffer);
		fin >> buffer;
		tmpl[0] = atol(buffer);
		fin >> buffer;
		tmpl[1] = atol(buffer);
		fin >> buffer;
		tmpl[2] = atol(buffer);
		fin >> buffer;
		tmpl[3] = atol(buffer);

		if (fin.eof())
		{
			break;
		}
		obj.yn = tmp[0];
		obj.start = tmp[1];
		obj.bcnum = tmp[2];
		obj.minx = tmp[3];
		obj.miny = tmp[4];
		obj.maxx = tmp[5];
		obj.maxy = tmp[6];
		obj.currentframe = tmp[7];
		obj.moveframe = tmp[8];
		obj.colorLength[0] = tmpl[0];
		obj.colorLength[1] = tmpl[1];
		obj.boolLength[0] = tmpl[2];
		obj.boolLength[1] = tmpl[3];
		if (obj.yn > 0)
		{
			g.push_back(gg); count++;
		}
		g[count - 1].vobj.push_back(obj);


		if (tmp[3] < g[count - 1].minx){ g[count - 1].minx = tmp[3]; }
		if (tmp[4] < g[count - 1].miny){ g[count - 1].miny = tmp[4]; }
		if (tmp[5] > g[count - 1].maxx){ g[count - 1].maxx = tmp[5]; }
		if (tmp[6] > g[count - 1].maxy){ g[count - 1].maxy = tmp[6]; }

	}
	fin.close();
	//g[g.size() - 1].vobj.pop_back();//清掉最後一行空白
	int x = 3;
}

void findsublabelx(vector<group> &g, Mat out, int groupcount, int convertcount)
{//找出輸入的大時間段內 有多少獨立的群
	for (int j = 0; j < g[groupcount].vobj.size(); j++)
	{
		int temp = g[groupcount].vobj[j].moveframe - convertcount;
		g[groupcount].vobj[j].whichsubgroup = out.ptr<int>(g[groupcount].vobj[j].minx - g[groupcount].minx + 1)[temp];

	}
}
void findsublabely(vector<group> &g1, Mat out, int groupcount, int convertcount)
{
	for (int j = 0; j < g1[groupcount].vobj.size(); j++)
	{
		int temp = g1[groupcount].vobj[j].moveframe - convertcount;
		g1[groupcount].vobj[j].whichsubgroup = out.ptr<uchar>(g1[groupcount].vobj[j].miny - g1[groupcount].miny + 1)[temp];

	}
}
void getnewgroup(vector<group> &gold, vector<vector<objectuni>> &gnew)
{//將大群包小群重新整理成一堆群
	int whichgroup = 0, total = 0, start = -1;
	for (int i = 0; i < gold.size(); i++)
	{
		whichgroup = 0;
		for (int j = 0; j < gold[i].vobj.size(); j++)
		{
			if (gold[i].vobj[j].whichsubgroup > whichgroup)
			{
				total += (gold[i].vobj[j].whichsubgroup - whichgroup);
				whichgroup = gold[i].vobj[j].whichsubgroup;
				gnew.resize(total);// 大群
				gold[i].vobj[j].yn = 1;
				gnew[total - 1].push_back(gold[i].vobj[j]);

			}
			else
			{
				if (gnew[gold[i].vobj[j].whichsubgroup + start].size() == 0)
				{
					gold[i].vobj[j].yn = 1;
				}
				else
				{
					gold[i].vobj[j].yn = 0;
				}
				gnew[gold[i].vobj[j].whichsubgroup + start].push_back(gold[i].vobj[j]);
			}
		}
		start = (total - 1);
	}
	int stop = 2;
}
bool findxyandsort(vector<vector<objectuni>> &tempg)
{//找出xtyt 要用哪個 並將畫出來的xtyt(tmp)做標籤化 最後重組成新群
	vector<group> g, g1;//給x跟y 兩個群
	indata(g);
	g1 = g;
	int xsum = 0, ysum = 0, convertcount = 0;

	for (int i = 0; i < g.size(); i++)
	{
		g[i].timeframe = g[i].vobj[g[i].vobj.size() - 1].moveframe - g[i].vobj[0].moveframe + 1;//抓每個時間群的總frame
		g1[i].timeframe = g[i].timeframe;

		Mat Matx(g[i].maxx - g[i].minx + 3, g[i].timeframe + 2, CV_8UC1, Scalar(0));//+3+2 為了給圖形留邊 比較好看
		Mat Maty(g[i].maxy - g[i].miny + 3, g[i].timeframe + 2, CV_8UC1, Scalar(0));


		vector<objectuni>::iterator iterobj = g[i].vobj.begin();
		for (iterobj; iterobj != g[i].vobj.end(); iterobj++)
		{//畫xt yt圖
			line(Matx, Point(iterobj->moveframe - convertcount, iterobj->minx - g[i].minx + 1), Point(iterobj->moveframe - convertcount, iterobj->maxx - g[i].minx + 1), 255, 1, 4);
			line(Maty, Point(iterobj->moveframe - convertcount, iterobj->miny - g[i].miny + 1), Point(iterobj->moveframe - convertcount, iterobj->maxy - g[i].miny + 1), 255, 1, 4);
		}

		Mat tmp(Matx.rows, Matx.cols, CV_8UC1, Scalar(0));
		xsum += (connectedComponents(Matx, tmp, 8, 4) - 1);//邊labeling 邊紀錄
		findsublabelx(g, tmp, i, convertcount);

		Mat tmp1(Maty.rows, Maty.cols, CV_8UC1, Scalar(0));
		ysum += (connectedComponents(Maty, tmp1, 8, 4) - 1);//邊labeling 邊紀錄
		findsublabely(g1, tmp1, i, convertcount);

		convertcount += g[i].timeframe;
	}
	cout << xsum << endl; cout << ysum  << endl;
	if (xsum > ysum)
	{
		getnewgroup(g, tempg); cout << "X;;"; return false;
	}
	else { getnewgroup(g1, tempg); cout << "Y;;";  return true; }

}
int compressy(vector<vector<objectuni>> &tempg)
{

	VideoCapture c("Square.avi");
	int scale = c.get(CV_CAP_PROP_FRAME_COUNT);
	Mat xy(c.get(CV_CAP_PROP_FRAME_HEIGHT), c.get(CV_CAP_PROP_FRAME_COUNT), CV_8UC1, Scalar(0));
	Mat mat1(c.get(CV_CAP_PROP_FRAME_HEIGHT), c.get(CV_CAP_PROP_FRAME_COUNT), CV_8UC1, Scalar(0));

	for (int i = 0; i < tempg.size(); i++)
	{
		int temptime = tempg[i][0].currentframe;
		for (int k = 0; k < c.get(CV_CAP_PROP_FRAME_COUNT); k++)
		{
			mat1.setTo(0);
			bool isoverlap = false;
			for (int j = 0; j < tempg[i].size(); j++)
			{
				line(mat1, Point(tempg[i][j].currentframe - temptime + k, tempg[i][j].miny), Point(tempg[i][j].currentframe - temptime + k, tempg[i][j].maxy), 1, 1, 4);
				tempg[i][j].adjstartframe = tempg[i][j].currentframe - temptime + k;
			}
			add(mat1, xy, mat1);
			for (int m = 0; m < mat1.rows; m++)
			{
				for (int n = 0; n < mat1.cols; n++)
				{
					if (mat1.ptr<uchar>(m)[n] > 1)
					{
						isoverlap = true; break;
					}
				}
				if (isoverlap) { break; }
			}
			if (!isoverlap)
			{
				xy = mat1.clone();
				break;
			}
		}
	}
	return 0;
	
}
int compress(vector<vector<objectuni>> &tempg)
{

	VideoCapture c("Square.avi");
	int scale = c.get(CV_CAP_PROP_FRAME_COUNT);
	Mat xy(c.get(CV_CAP_PROP_FRAME_WIDTH), c.get(CV_CAP_PROP_FRAME_COUNT), CV_8UC1, Scalar(0));
	Mat mat1(c.get(CV_CAP_PROP_FRAME_WIDTH), c.get(CV_CAP_PROP_FRAME_COUNT), CV_8UC1, Scalar(0));
	//Mat tmp1;
	for (int z = 0; z < tempg.size(); z++)//跑前景
	{
		int temptime = tempg[z][0].currentframe;
		for (int k = 0; k < c.get(CV_CAP_PROP_FRAME_COUNT); k++)//\總frame
		{
			mat1.setTo(0);
			bool isoverlap = false;
			for (int j = 0; j < tempg[z].size(); j++)
			{
				line(mat1, Point(tempg[z][j].currentframe - temptime + k, tempg[z][j].minx), Point(tempg[z][j].currentframe - temptime + k, tempg[z][j].maxx), 1, 1, 4);
				tempg[z][j].adjstartframe = tempg[z][j].currentframe - temptime + k;
			}

			//Mat temX; temX = mat1.clone();
			//for (int i = 0; i < mat1.rows; i++)
			//{
			//	for (int j = 0; j < mat1.cols; j++)
			//	{
			//		if (mat1.ptr<uchar>(i)[j] > 0){ temX.ptr<uchar>(i)[j] =255; }
			//	}
			//}
			add(mat1, xy, mat1);
			/*	Mat tem1; tem1 = mat1.clone();
				for (int i = 0; i < mat1.rows; i++)
				{
				for (int j = 0; j < mat1.cols; j++)
				{
				if (mat1.ptr<uchar>(i)[j] >0){ tem1.ptr<uchar>(i)[j] *= 100; }
				}
				}*/

			for (int m = 0; m < mat1.rows; m++)
			{
				for (int n = 0; n <mat1.cols; n++)
				{
					if (mat1.ptr<uchar>(m)[n] > 1)
					{
						isoverlap = true; break;
					}
				}
				if (isoverlap) { break; }
			}
			if (!isoverlap)
			{
				xy = mat1.clone();
				break;
			}
		}
	}
	/*Mat tem = mat1.clone();
	for (int i = 0; i < mat1.rows; i++)
	{
		for (int j = 0; j < mat1.cols; j++)
		{
			if (mat1.ptr<uchar>(i)[j] >0){ tem.ptr<uchar>(i)[j] *= 100; }
		}
	}*/
	int i;
	for ( i = 0; i < xy.cols; i++)
	{
		bool isbreak = false;
		for (int j = 0; j < xy.rows; j++)
		{
			if (xy.ptr<uchar>(j)[i] > 0){ isbreak = true; break; }
		}
		if (!isbreak){ cout <<"? "<< i << endl; break; }
	}
	return i;
	
}
void ReadInJpgtoMat(long st, long ed, cv::Mat &img, fstream &f_bin, int color)
{
	//開圖
	uchar *Buf1 = (uchar *)malloc((ed - st)*sizeof(uchar));
	f_bin.seekg(st);
	f_bin.read((char*)Buf1, (ed - st)*sizeof(uchar));
	vector<uchar> DeBuf1(Buf1, Buf1 + (ed - st));
	img = cv::imdecode(DeBuf1, color);
	free(Buf1);
}
void show(vector<vector<objectuni>> &tempg,int end)
{
	int tow = 100;
	vector<int> LT(tempg.size());
	vector<float>W(tempg.size());
	vector<int> S(tempg.size());
	vector<vector<objectuni>> ::iterator iter;
	for (int i = 0; i < tempg.size(); i++)
	{

		LT[i] = tempg[i].back().adjstartframe - tempg[i][0].adjstartframe + 1;
		W[i] = (float)(tempg[i][0].adjstartframe) / (end - LT[i] + 1);
		S[i] = ceil((tow - LT[i])* W[i]);
	}
	///****
	for (int i = 0; i < tempg.size(); i++)
	{
		int conv = tempg[i][0].adjstartframe - S[i];
		for (int j = 0; j < tempg[i].size(); j++)
		{
			tempg[i][j].adjstartframe -= conv;
		}
	}

	//***
	namedWindow("!");
	Mat realbg = imread("../Data/BG.jpg", CV_LOAD_IMAGE_ANYCOLOR);
	fstream bin("../Data/ImageBin.bin", ios::in | ios::out | ios::binary);
	for (int count = 0; count <=end; count++)
	{
		
		Mat bigpaste(realbg.rows, realbg.cols, CV_16UC3, Scalar(0, 0, 0));//
		Mat blackbg(realbg.rows, realbg.cols, CV_8UC1, Scalar(0));
		for (int i = 0; i < tempg.size(); i++)//讀黑白圖 並且記錄重疊數
		{
			for (int j = 0; j < tempg[i].size(); j++)
			{
				if (tempg[i][j].adjstartframe == count)
				{
					//record.push_back(Point(i,j));
					Mat paste1;//讀入黑白圖
					ReadInJpgtoMat(tempg[i][j].boolLength[0], tempg[i][j].boolLength[1], paste1, bin, 0);
					Mat roi = blackbg(Rect(tempg[i][j].minx, tempg[i][j].miny, tempg[i][j].maxx - tempg[i][j].minx + 1, tempg[i][j].maxy - tempg[i][j].miny + 1));
					threshold(paste1, paste1, 200, 1, THRESH_BINARY);//
					roi += paste1;

					Mat paste;
					ReadInJpgtoMat(tempg[i][j].colorLength[0], tempg[i][j].colorLength[1], paste, bin, 1);
					Mat rop = bigpaste(Rect(tempg[i][j].minx, tempg[i][j].miny, tempg[i][j].maxx - tempg[i][j].minx + 1, tempg[i][j].maxy - tempg[i][j].miny + 1));//貼上用

					for (int u = 0; u < paste.rows; u++)
					{
						for (int v = 0; v < paste.cols; v++)
						{
							if (paste1.ptr<uchar>(u)[v] > 0)
							{
								rop.ptr<ushort>(u, v)[0] += paste.ptr<uchar>(u, v)[0];
								rop.ptr<ushort>(u, v)[1] += paste.ptr<uchar>(u, v)[1];
								rop.ptr<ushort>(u, v)[2] += paste.ptr<uchar>(u, v)[2];
							}

						}
					}
				}
			}
		}
		Mat tempout = realbg.clone();
		for (int i = 0; i < blackbg.rows; i++)
		{
			for (int j = 0; j < blackbg.cols; j++)
			{
				int x = blackbg.ptr<uchar>(i)[j];
				if (x > 0)
				{
					tempout.ptr<uchar>(i, j)[0] = bigpaste.ptr<ushort>(i, j)[0] / x;
					tempout.ptr<uchar>(i, j)[1] = bigpaste.ptr<ushort>(i, j)[1] / x;
					tempout.ptr<uchar>(i, j)[2] = bigpaste.ptr<ushort>(i, j)[2] / x;
				}
			}
		}
		imshow("!", tempout);
		if (cvWaitKey(1) >= 0) break;

	}
	bin.close();
}
