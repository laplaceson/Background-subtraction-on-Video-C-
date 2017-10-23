#include<vector>
struct   struc
{
	float R = 0, G = 0, B = 0; float weight = 0; int count = 0;
	bool operator< (const struc& cmp) const
	{
		return weight > cmp.weight;
	}

};
struct RGB
{
	float R,G,B;
};
struct PATH
{
	char txtpath[100] ;
	char folderpath[30] ;
	char BGpath[30];
	char binpath[100];
} ;
struct record
{
	int bcnum = 1;
	int minx, miny,maxx = 0,maxy = 0;
	std::streampos colorLength[2];
	std::streampos boolLength[2];
};
struct bigrecord
{
	int yn = 0;
	int start = 0;int currentframe = 0;
	int moveframe = 0;
	bool isset = false;//防止第一章圖就有N個前景 卻只能有一個的yn=1
	std::vector<record> rec;
};
struct objectuni
{
	int yn = 0; int whichsubgroup = 0;
	int start = 0;
	int currentframe = 0;
	int moveframe = 0;
	int bcnum = 1;
	int minx = 65535, miny = 65535, maxx = 0, maxy = 0;
	std::streampos colorLength[2];
	std::streampos boolLength[2];

	bool operator<(const objectuni& obj)const{ return whichsubgroup < obj.whichsubgroup; };
	int adjstartframe = 0;

};
struct group
{
	int timeframe = 0, maxx = 0, maxy = 0, minx = 65535, miny = 65535;
	std::vector <objectuni> vobj;

};