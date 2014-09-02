#include<stack>
#include<iostream>
using namespace std;
class cluster
{
	public:
	stack< pair<int,int> > s; //stack to keep all the cordinates
	int minx,miny,maxx,maxy; //these will keep the maximum and munimum x and y cordinates in a cluster
	void insert(int x,int y);
	cluster();
};
//comparision operator of cluster class on basis of size used in priority_queue
class comparison1{
	public:
	bool operator() (const cluster& x1 , const cluster& x2){
		return (x1.s.size() < x2.s.size());}
	};
//comparision operator of cluster class on basis of leftmost x-cordinate used in priority_queue
class comparison2{
	public:
	bool operator() (const cluster& x1 , const cluster& x2){
		return (x1.minx > x2.minx);}
	};
