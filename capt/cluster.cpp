#include "cluster.h"
//function to insert cordinates of a pixel in a cluster	
void cluster::insert(int x,int y)
{
	pair<int,int>* temp;
	temp = new pair<int,int>(x,y);
	if(minx > x)
	minx=x;
	if(miny > y)
	miny=y;
	if(maxx < x)
	maxx=x;
	if(maxy < y)
	maxy=y;
	s.push(*temp);
}
//constructor which is initialising maximum x and y and minimum x and y cordinates in a cluster
cluster::cluster()
	{
		minx=9000,miny=9000,maxx=0,maxy=0;
	}



