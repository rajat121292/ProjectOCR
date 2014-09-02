
#include"neural.h"
using namespace std;

//activation function
float sigmoid(float value){
	return 1 / (1 + exp(-value));
}

// derivative of sigmoid function 
float dersigmoid(float value){
	return value*(1-value);
}

//function to compute the value to be stored in each neuron , so this is a weighted sum  of each neuron with its corresponding weight going to a neuron
void linear_combi(neuron* x , neuron* y , int xsize , int ysize){
	for(int i=0;i<ysize;i++){
		for (int j=0; j<xsize ; j++){
			y[i].val += x[j].val * x[j].weight[i];
		}
	y[i].val = sigmoid(y[i].val); //activation function finally appilied on values
	}
	
}

// constructor
neural::neural(int in ,int test,int batch,float leasterr, int out , int hidden , float learningr , float momen ){
	inputN = in;    //no. of neuron in input layer
	error=30; // initialised error from 30  (random)
	outputN = out;  //no.of neuron in output layer
	hiddenN = hidden; //no. of neuron in hidden layer
	learningrate = learningr; // stores learning rate
	momentum = momen; //stores momentum
	threshold = 2; // when error will reach this files will be produced with this as a error
	epoc=1;
	testcaseN = test; 
	batchsize=batch; // baatch size for offline learning
	leasterror = leasterr;
	input_neurons = new neuron[in+1]; // providing memory to neuron in initial layer , with one extra because of bias
	output_neurons = new neuron[out]; //  providing memory to neuron in output layer
	hidden_neurons = new neuron[hidden+1]; //providing memory to neuron in hidden layer with one extra because of bias
	target = new int[out];   //memory to target vector
	hidden_neurons[hidden].val=1; //providing value of that bias to be 1
	input_neurons[in].val=1;//bias value = 1
	
	// setting weight between hidden layer and output layer
	for (int i=0;i<hidden+1;i++){
		hidden_neurons[i].setweight(out);
	}
	//setting cumulative delta for offline training
	for (int i=0;i<hidden;i++){
		hidden_neurons[i].cum_delta =new float[in];
		for (int j=0;j<in;j++)
			hidden_neurons[i].cum_delta[j] = 0;
	}
	//setting weight between hidden and input layer
	for (int i=0;i<in+1;i++){
		input_neurons[i].setweight(hidden);
	}
	//initialising delta for each neuron which is required for weight adjustment
	for (int i=0;i<out;i++){
		target[i] = 0;
		output_neurons[i].cum_delta =new float[hidden+1];
		for (int j=0;j<hidden+1;j++)
			output_neurons[i].cum_delta[j] = 0;
	}
	current_testcase =1; //initialising current test case to 1
}

//constructor called when training is done and a check is to be made about a character 
neural::neural(int in, int out, int hidden){
	inputN = in;
	outputN = out; 
	hiddenN = hidden;
	input_neurons = new neuron[in+1];
	output_neurons = new neuron[out];
	hidden_neurons = new neuron[hidden+1];
	hidden_neurons[hidden].val=1;
	input_neurons[in].val=1;
	
	for (int i=0;i<hidden+1;i++){
		hidden_neurons[i].setweight(out);
	}
	for (int i=0;i<in+1;i++){
		input_neurons[i].setweight(hidden);
	}
	
}
	
//initialising value of hidden layer and output layer to zero before taking in a file from training dataset
void neural::set_zero(){
	for (int i=0;i<hiddenN;i++){
		hidden_neurons[i].val=0;
	}
	for (int i=0;i<outputN;i++){
		output_neurons[i].val=0;
	}
}

//to give a particular file with trained weights to the neural network :called when to make a prediction of a character
void neural::user_input(string filename){
	set_zero();
	ifstream myfile(filename.c_str());
	for (int i =0;i<inputN;i++){
		myfile >> input_neurons[i].val;
	}
}
// function to take a file from training set
void neural::get_input(){
	set_zero();
	for (int i=0;i<outputN;i++)
		target[i]=0; //setting the target vector to zero
	ostringstream oss;
	oss << "training/" << current_testcase-1 <<".txt"; 
	ifstream myfile(oss.str().c_str()); //takes in a particular file
	for (int i =0;i<inputN;i++){
		myfile >> input_neurons[i].val;  //feeds the input feature vector into the input layer
	}
	int temp;
	myfile >> temp; //last line has the type of character for which system is going to be trained
	target[temp]=1; //setting target vector 1 for that character
	(current_testcase==testcaseN)?current_testcase=1:current_testcase++;
}
//forward propogation
void neural::forward_propogation(){
	linear_combi(input_neurons,hidden_neurons,inputN+1,hiddenN); //calculates value of neuron in hidden layer
	linear_combi(hidden_neurons,output_neurons,hiddenN+1,outputN);//calculate value of neuron in output layer
}

//backward propogation
void neural::back_propogation(){
	
	for (int i=0;i<outputN;i++){
		output_neurons[i].delta = (target[i] - output_neurons[i].val)*(dersigmoid(output_neurons[i].val));//calculating delta for output neurons first as it is backward propogation
		error += (target[i]-output_neurons[i].val)*(target[i]-output_neurons[i].val); // calculating error according to the target vector and value of neuron in output vector
		//accumulating delta that is used for offline training 
		for (int j=0;j<hiddenN+1;j++)
			output_neurons[i].cum_delta[j] += output_neurons[i].delta * hidden_neurons[j].val;
	}
	
	//now processing the hidden layer
	for (int i=0; i<hiddenN;i++){
		hidden_neurons[i].delta=0;
		for (int j =0 ; j< outputN;j++){
			hidden_neurons[i].delta += hidden_neurons[i].weight[j] * output_neurons[j].delta;//calculating delta of all neurons in hidden layer using the delta of output layer
		}
		hidden_neurons[i].delta *= dersigmoid(hidden_neurons[i].val); // multiplied with the derivative of sigmoid
		for (int j=0; j<inputN+1 ;j++)
			hidden_neurons[i].cum_delta[j] += hidden_neurons[i].delta * input_neurons[j].val; //calculating cum delta
	}
}

//this takes in the file name and produces a file which contains all weight between neurons i.e a final trained file
void neural::final(string filename){
	ofstream myfile;
	myfile.open(filename.c_str());
	for (int i=0;i<inputN;i++)
		for (int j=0;j<hiddenN;j++)
			myfile << input_neurons[i].weight[j] <<endl; //writing weights between input neuron an dhidden neuron to a file
	for (int i=0;i<hiddenN+1;i++)
		for (int j=0;j<outputN;j++)
			myfile << hidden_neurons[i].weight[j] <<endl;//writing weights to a file
}
	
//this will be doing the final training
void neural::offline_propogation(){
	int count=testcaseN/batchsize; //calculates how much batch are done
	while(error>leasterror){ //when error is least than least error triaing will stop
		error=0; //in each epoc initialising error to zero
		while (count!=0){ // traoins for all mini batch for offline training
			for (int i=0;i<batchsize;i++){
				get_input();//takes the file from training set as input
				forward_propogation();//do the forward propogation for every file
				back_propogation();//do backward propogation
			}
			weight_adjust();//adjsut the weights after each minibatch is done
			count--;
		}
		count=testcaseN/batchsize;
		
		error /= outputN; //taking the error to be variance
		if (error < threshold) { //if error is eless than threshhold it produces a file this is done to check we don't pick a overfitting file and thus all intermediate files are created in process
			ostringstream oss;
			oss <<"result/"<< error*100 <<".txt";//this will produce the file in a folder result4
			final(oss.str());
			threshold -=.005;
		}
		
		cout<<epoc<<" "<<error<<" "<<leasterror<<endl;
		epoc++;//updates epocs
	}
}



// adjust the weights after each epoc
void neural::weight_adjust(){
	float temp_weight =0;
	for (int i=0;i<inputN+1;i++){
		for (int j=0;j<hiddenN;j++){
			temp_weight = input_neurons[i].weight[j]; //used to keep prev weight
			input_neurons[i].weight[j] += momentum * (input_neurons[i].weight[j] - input_neurons[i].prev_weight[j]) + learningrate * hidden_neurons[j].cum_delta[i]; // calculating new weights between hidden and input layer
			input_neurons[i].prev_weight[j] = temp_weight; //setting prev weight 
		}
	}
	//adjusting weight between hidden and output layer
	for (int i=0;i<hiddenN+1;i++){
		for (int j=0;j<outputN;j++){
			temp_weight = hidden_neurons[i].weight[j];
			hidden_neurons[i].weight[j] += momentum * (hidden_neurons[i].weight[j] - hidden_neurons[i].prev_weight[j]) + learningrate * output_neurons[j].cum_delta[i];
			hidden_neurons[i].prev_weight[j] = temp_weight;
		}
	}
	//resetting the cumulative delta to be 0 for each neuuron.
	for (int i=0;i<hiddenN;i++)
		for (int j=0;j<inputN+1;j++)
			hidden_neurons[i].cum_delta[j] = 0;
	for (int i=0;i<outputN;i++)
		for (int j=0;j<hiddenN+1;j++)
			output_neurons[i].cum_delta[j] = 0;

}
