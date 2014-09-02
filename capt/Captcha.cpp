#include "Captcha.h"
//absolute value function
int abs (int a){
        return (a>0)?(a):(-1*a);
}
//constructor function 
//initialises neural network with obtained weights
Captcha::Captcha(CImg<unsigned char>& image){
	captcha = image;
	//count =1;
        n_comp=0;
	refer = new char[53];		//making the map from character to its class number
	for (int i=0;i<53;i++){
		if (i<10)
			refer[i] = (char)(48+i);
		else if (i<36)
			refer[i] = (char)(97+i-10);
		else if (i<38)
			refer[i] = (char)(65+i-36);
		else if (i<49)
			refer[i] = (char)(66+i-36);
		refer[49] = 'Q';
		refer[50] = 'R';
		refer[51] = 'T';
		refer[52] = 'Y';
	}
	network = new neural(225,53,130);
        ifstream myfile("../capt/training.txt");	//picking up the obtained weights
	for (int i=0;i<network->inputN;i++)				//feeding the weights into the network
		for (int j=0;j<network->hiddenN;j++)
			myfile >> network->input_neurons[i].weight[j];
	for (int i=0;i<network->hiddenN+1;i++)
		for (int j=0;j<network->outputN;j++)
			myfile >> network->hidden_neurons[i].weight[j] ;

}
//this function removes noise from the image by applying various filters
void Captcha::filter(int med,float bilat,int bilate,int sharp){
	captcha.blur_median(med);	//removes salt and pepper noise
	captcha.blur_bilateral(bilat,bilat,0,bilat,bilate,bilate,0,bilate);	//removes noise
	captcha.sharpen(sharp);//sharpens the edges
	
}
//this function separates out the characters
void Captcha::decompose(int close){
	
	priority_queue < cluster,vector<cluster>,comparison1 > myclusters;	//priority queue on basis of size
	priority_queue <cluster,vector<cluster>,comparison2> myclust;	//priority queue on basis of leftmost co-ordinate
	int imageHeight = captcha.height();
	int imageWidth = captcha.width();
	mark = new bool*[imageWidth];
	//allocating memory to marker array
	for (int i=0;i<imageWidth;i++){
		mark[i]=new bool[imageHeight];
	}
	for (int i=0;i<imageWidth;i++){
		for (int j=0;j<imageHeight;j++){
			mark[i][j]=1;
		}
	}

 //we are looking at steps of height/6 for clusters of similar color in the image
	for(int i=1;i<imageWidth-1;i++)
	{		
		for (int w=1;w<6;w++){
			int j=w*imageHeight/6;
			if (!mark[i][j]) continue;
			if(!nbrchk(captcha,i,j,close))
			{
				continue;
			}
			cluster* mycluster;
			mycluster = new cluster;
			lake(i,j,*mycluster,captcha,close);
		
			myclusters.push(*mycluster);	//putting them in size based priority queue
		}
	}

	cluster tem = myclusters.top();
	if (tem.maxx-tem.minx > imageWidth/2)	//ignoring if width > 1/2 width of image
	{
		myclusters.pop();
	}
	
	
		
	int j=0;
	int size=0;
	while (myclusters.size()>0){	//for each cluster in the priority queue
		
		cluster *temp;
		temp = new cluster;
		*temp = myclusters.top();
		myclusters.pop();
		if (temp->s.size() < size*0.5) {break;}	//ignoring rest all if size decreases drastically
		size = temp->s.size();

		
		if (temp->maxx-temp->minx >imageWidth/3) continue;
		if (temp->maxy - temp->miny <imageHeight/3) continue;		
		
		myclust.push(*temp);	//putting clusters in minx based priority queue
		j++;		//counting number of passed on clusters
	}
	
	int count = 0,prevax=0;	
	while(myclust.size()>0){
		count++;
		CImg<unsigned char>	first(imageWidth,imageHeight,1,3);	//making new image
		first.fill(255);		//filling it with white
		cluster temp1 = myclust.top();
		myclust.pop();
		if (prevax > temp1.maxx) {	//ignoring if maxx of current char is less than maxx of 	prev char
			prevax=temp1.maxx;
			j--;		//decreasing number of ignored clusters
			count--;
			continue;}
			prevax=temp1.maxx;
		
		while((temp1.s).size()>0)	//coloring the cluster pixel on new image with black
		{
			int x= (temp1.s).top().first;
			int y= (temp1.s).top().second;			
			fix(first,x,y,0);
			(temp1.s).pop();
		}
		
		first.erode(1,1);
		ostringstream oss;
		oss << count<<".jpg";	//saving the image as count.jpg i.e 1.jpg, 2.jpg
		first.save(oss.str().c_str());
		
		//myclust.pop();


	}
	n_comp=j;	//setting the number of char in images = total clusters - ignored clusters
}
//closeenough checks if all 3 components of two pixels differ by less than "close"	
bool Captcha::closeenough(CImg<unsigned char> & sourceImage,int i1,int j1,int i2,int j2,int close)
{
	int a=close;
	if(abs(sourceImage(i1,j1,0,0) - sourceImage(i2,j2,0,0)) <= a)
	{
		if(abs(sourceImage(i1,j1,0,1) - sourceImage(i2,j2,0,1)) <= a)
                {
			if(abs(sourceImage(i1,j1,0,2) - sourceImage(i2,j2,0,2)) <= a)
			return true;
		}
	}
	else 
	return false;
}	
	
//nbrchk checks if given pixel has atleast 2 neighbours closeenough to it
bool Captcha::nbrchk(CImg<unsigned char>& sourceImage,int i,int j,int close)
{	int count=0;
	for(int a=-1;a<2;a++)
	{
		for(int b=-1;b<2;b++)
		{
			if(closeenough(sourceImage,i,j,i+a,j+b,close))
			count ++;
			
		}
	}
	return (count>2);
}
//fix sets value of all components of a pixel to "c"
void Captcha::fix(CImg<unsigned char>& sourceImage,int i,int j,int c)
{
	sourceImage(i,j,0,0)=c;
	sourceImage(i,j,0,1)=c;
	sourceImage(i,j,0,2)=c;
}
//lake function moves over pixels of similar color and pushes them into a cluster
void Captcha::lake(int i, int j, cluster& c,CImg<unsigned char>& sourceImage,int close)
{
	c.insert(i,j);	//pushing in cluster
	mark[i][j]=0;	//marking the pixel
	for(int a=-1;a<2;a++){
		for(int b=-1;b<2;b++){
			if(i+a<0 || j+b <0 ||i +a >=sourceImage.width() || j+b >=sourceImage.height())continue;
			if (mark[i+a][j+b]&&closeenough(sourceImage,i,j,i+a,j+b,close)){
				lake(i+a,j+b,c,sourceImage,close);	//making the recursive call
			}
		}
	}
}
//digitises the image into a text image
void Captcha::convert(int i,CImg<unsigned char> & sourceImage )
{
	int imageHeight = sourceImage.height();
	int imageWidth = sourceImage.width();
	ostringstream oss ;
	oss<<i<<".txt";			//making files 1.txt, 2.txt,....
	ofstream myfile;
	myfile.open((oss.str()).c_str());
	for(int j=0;j<imageHeight;j++)
	{
		for(int k=0;k<imageWidth;k++)
		{
			if(sourceImage(k,j,0,0)<150)
			myfile<<1<<endl;
			else
			myfile<<0<<endl;
		}

	}
        myfile.close();
}
	
//crops the image and resizes it
void Captcha::crop(){	
	
	for(int i=1;i<n_comp+1;i++)
	{	
		ostringstream oss1,oss2 ;
		oss1<<i<<".jpg";
		CImg<unsigned char> sourceImage(oss1.str().c_str());
		int minx=0,maxx=0,miny=0,maxy=0;
	
	//next 4 for loops are for finding the bounding box around the character
		for(int a=0;a<sourceImage.height();a++)
		{
			for(int b=0;b<sourceImage.width();b++)
			{
				if(sourceImage(b,a,0,0)<150)
				miny=a;
			}
			if(miny!=0)
			break;
		}
		for(int a=sourceImage.height()-1;a>=0;a--)
		{
			for(int b=0;b<sourceImage.width();b++)
			{
				if(sourceImage(b,a,0,0)<150)
				{
					maxy=a;
				}	
			}
			if(maxy!=0)
			break;
		}

		for(int a=0;a<sourceImage.width();a++)
		{
			for(int b=0;b<sourceImage.height();b++)
			{
				if(sourceImage(a,b,0,0)<150)
				{
				minx=a;
				}
				
			}
			if(minx!=0)
			break;
		}
		for(int a=sourceImage.width()-1;a>=0;a--)
		{
			for(int b=0;b<sourceImage.height();b++)
			{
				if(sourceImage(a,b,0,0)<150)
				maxx=a;
			}
			if(maxx!=0)
			break;
		}
		//making a new image of size = max(width of cluster,height of cluster)
		int size;	
		size = maxx-minx>maxy-miny?maxx-minx+1:maxy-miny+1;
		
		CImg<unsigned char> temp2(size+2,size+2,1,3);		
		
		temp2.fill(255);
		int gapx = temp2.width()-(maxx-minx+1);
		gapx/=2;
		int gapy = temp2.height() - (maxy-miny+1);
		gapy/=2;
		
		//copying the character into the center of new image
		for(int k=minx;k<=maxx;k++)
		{
			for(int j=miny;j<=maxy;j++)
			{
				temp2(k+gapx-minx,j+gapy-miny,0,0) = sourceImage(k,j,0,0);
				temp2(k+gapx-minx,j+gapy-miny,0,1) = sourceImage(k,j,0,0);
				temp2(k+gapx-minx,j+gapy-miny,0,2) = sourceImage(k,j,0,0);
			}
		}							
		
		temp2.resize(15,15);	//resizing the image to 15 X 15
		convert(i,temp2);
		remove(oss1.str().c_str());		//removing image file that has been processed
	}	
}

string Captcha::prediction(){
    string s="";
        for (int k=1;k<n_comp+1;k++){	//for each character
                ostringstream image;
                image<<k<<".txt";		
                network->user_input(image.str());	//taking the text image as input
                remove(image.str().c_str());		//removing the text file
                float sum=0;
                network->forward_propogation();		//passing the character through network
                for (int i=0; i< network->outputN;i++){		//summing up the output values
                        sum += network->output_neurons[i].val;
                }
                float max = 0;
                int pos=0;
                max= network->output_neurons[0].val;

                for ( int j=0; j<network->outputN;j++){		//finding the class with max. probability
                        if (network->output_neurons[j].val > max ) {
                                max = network->output_neurons[j].val;
                                pos =j;
                        }
                }
                s += refer[pos];	//adding the char obtained to output string

        }
    return s;
}

