#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<cstdio>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

//Neuron Struct will represent each node i.e neuron of each layer
struct neuron{
	float val;  // this will store the value that is obtained through activation function in case of output layer and hidden layer and input layer will have value equal to those of feature vector
	float delta; 
	float* cum_delta;
	
//weight vector will be used to keep the weight of the coneections going from one neuron to other	
	float* weight;
//this will store the previous weight i.e weight before weight adjustment
	float* prev_weight;
//constructor for a neuron
	neuron(){
		val = 0;
		delta =0;
		weight = NULL;
	}
// this will set weight between neurons
	void setweight(int n){
		weight = new float[n];   // n will be the no. of neuron in next layer
		prev_weight = new float[n];
		for (int i=0; i<n;i++){
			weight[i] = ((( (float)rand() / ((float)(RAND_MAX)+(float)(1)) )) - 0.5); // initially giving random weights between 0.5 to -0.5 as between these values neural network works optimally
		prev_weight[i] = weight[i];
		} 
		
	}
};


// main class neural in which network will be described
class neural{
	public:
	int inputN , outputN , hiddenN;  // will keep the no. of neurons in each layer
	int *target; // it is the target vector
	int epoc; // it will keep the count about how much time will the training set will run
	void user_input(string f); // to take the input of training file...this will be used after training is donse and then to finally read weights from file .
	float error; // stores the error after each epoc 
	float threshold; //using this threshold , each time program will save a file with weights
	float leasterror; //error reach leasterror , training will stop
	int testcaseN,batchsize; // stores no. of file in training set, and batch size for offlinne training
	int current_testcase; // stores which file is going for the training
	float learningrate , momentum; // stores the learning rate and momentum
	neuron *input_neurons , *output_neurons , *hidden_neurons;  //these are the layer vectors contining neurons
	neural(int in ,int test,int batch,float leasterr, int out , int hidden , float learningr , float momen ); //constructor
	neural(int in ,int out, int hidden); //constructor used when one need to predict a character
	void forward_propogation(); // function for forward propogation
	void get_input(); // takes the input from a training file from a training dataset
	void set_zero(); // this will initialize the values of all neuron to zero before taking in another file from training set
	void back_propogation(); //for backward propogation
	void weight_adjust(); // will adjust the weight after each epoc
	void offline_propogation(); // final call for all propogation
	void final(string filename); //produces  the final answer
};
