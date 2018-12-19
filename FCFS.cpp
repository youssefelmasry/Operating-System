#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#define MAX_SIZE 10
using namespace std;


// vector <char> process_name; // Processes Names
short int num_line = 0;
float Throughput=0;
// vector <short int> Arrival_Time, burst_time, Deadline; // Processes Info
 ;

// Struct for each policy to have it's own info
struct Metrics{

	float AWT=0, ART=0, ATT=0;
};
// Struct for each process to have it's own info
struct Process
{
	short int wait_time = 0, response_time = 0, Turn_time = 0;
	short int Arrival_Time = 0, burst_time = 0, Deadline = 0;
	char process_name;
};
// vector of all processes info
vector <Process> Processes;

/*Funcitons prototype*/

// file load and parse
void load_file(string filename);
void parse(vector <string> vec);
float throughput();

bool burstTime(Process& a, Process& b); // comparison function
// scheduler polices functions 
Metrics FCFS(Metrics, vector<Process> p); // First Come First Serve policy
Metrics SJF(Metrics, vector<Process> p); // Shortest Job First

int main()
{
	load_file("in.txt");
	Throughput = throughput();

	Metrics fcfs;
	Metrics sjf;

	fcfs = FCFS(fcfs, Processes);
	cout<<fcfs.AWT<<endl;

	SJF(sjf, Processes);


	return 0;
}

void load_file(string filename)
{
	vector <string> vec; // vector to store lines of the input file
	string line = "";
	ifstream myfile(filename);
	if (myfile)
	{
		// getting the first line (titles line)
		getline(myfile,line);
		// the processes lines 
		while(getline(myfile,line))
		{
			num_line++;
			vec.push_back(line);
		}
		myfile.close();
		parse(vec);
	}
	else{cout<<"Error! Cannot open the file \n"; exit(1);}	
}

void parse(vector <string> vec)
{ 
	vector <string> :: iterator i;

    for (i = vec.begin(); i != vec.end(); i++)
    {	
		stringstream is(*i);
		string token = "";
		// put each line content in the corresponding vector
			Process p;

			getline(is, token,',');
			// string to chars
		   	p.process_name = token[0];

		   	getline(is, token,',');
		   	p.Arrival_Time = stoi(token);

		   	getline(is, token,',');
		   	p.burst_time = stoi(token);

		   	getline(is, token,',');
		   	p.Deadline = stoi(token);

		   	Processes.push_back(p);
    }
    // sort(begin(Arrival_Time), end(Arrival_Time));
}

float throughput()
{
	short int i;
	float tmp = 0.0;
	for(i = 0; i<num_line; i++)
	{
		tmp += Processes[i].burst_time;
	}
	return num_line/tmp;
}

bool burstTime(Process& a, Process& b) // comparison function
{
	return a.burst_time < b.burst_time;
}

Metrics FCFS(Metrics fcfs, vector<Process> p)
{
	short int i,j;
	vector<Process> processes;
	processes = p;
	// Calculating waiting time and response time. i.e: the response time equal waiting time in fcfs policy
	// starting from 1 (second process) because the first one does not wait
	for(i = 1; i<num_line; i++)
	{
		for(j=0; j<i; j++){
			processes[i].wait_time += processes[j].burst_time ;
		}
		processes[i].wait_time -= processes[i].Arrival_Time;
		fcfs.AWT+=processes[i].wait_time;

		processes[i].response_time = processes[i].wait_time;
		fcfs.ART += processes[i].response_time;
	}
	// Calculating turn around time
	for(i = 0; i<num_line; i++)
	{
		processes[i].Turn_time = processes[i].burst_time + processes[i].wait_time;
		fcfs.ATT += processes[i].Turn_time;
	}
	
	fcfs.AWT /= num_line;
	fcfs.ART /= num_line;
	fcfs.ATT /= num_line;

	return fcfs;
}

