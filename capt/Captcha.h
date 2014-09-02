#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
using namespace std;

#include "CImg.h"
using namespace cimg_library;
#include "cluster.h"
#include "neural.h"

class Captcha{
	CImg<unsigned char> captcha;			//image object to be processed
	
	bool** mark;					//marker array to mark pixels as visited while clustering
	

	char *refer;	//reference array -- maps characters to their class number
	
//closeenough checks if all 3 components of two pixels differ by less than "close"
	bool closeenough(CImg<unsigned char> & sourceImage,int i1,int j1,int i2,int j2,int close);
	
//nbrchk checks if given pixel has atleast 2 neighbours closeenough to it
	bool nbrchk(CImg<unsigned char>& sourceImage,int i,int j,int close);
	
//lake function moves over pixels of similar color and pushes them into a cluster
	void lake(int i, int j, cluster& c,CImg<unsigned char>& sourceImage,int close);
	
//fix sets value of all components of a pixel to "c"
	void fix(CImg<unsigned char>& sourceImage,int i,int j,int c);
	
//network is the neural network that identifies characters
	neural* network;
	
	public:
	int n_comp;	//no of characters in the image

	Captcha(CImg<unsigned char>& image);//constructor function 
	void filter(int med,float bilat,int bilate,int sharp);
	void decompose(int close);		//this function separates out the characters
	void convert(int i,CImg<unsigned char> & sourceImage );	//digitises the image into a text	image
	void crop();	//crops the image and resizes it
        string prediction();	//identifies the characters
};
	
