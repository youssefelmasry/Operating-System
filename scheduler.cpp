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

// Struct for each policy to have it's own info
struct Metrics{
	// average waiting time, average responce time, average turn around time
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

bool burstTime(Process& a, Process& b); // comparison function to sort struct
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

	sjf = SJF(sjf, Processes);
	cout<<sjf.AWT<<endl;


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
			// taking the first element from token to convert string to char
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

Metrics SJF(Metrics sjf, vector<Process> p)
{
	vector <Process> pq; // priority queue according to burst time
	short int btime = 0, i = 1; // accumlating burst time
	vector <Process> vec; // temporary vector
	// a copy from vector Processes to not modify the main vector for the other schedulers
	vector <Process> processes;
	processes = p;

	pq.push_back(processes[0]);
	while(!pq.empty())
	{
		if (i < num_line)
		{
			// check if there any processes has arrived at the same time
			while(processes[i].Arrival_Time == processes[i-1].Arrival_Time)
			{
				pq.push_back(processes[i]); i++;
			}
			Process ptemp;// temporal object to save the current process
			sort(pq.begin(),pq.end(), burstTime); // sort the vector as it is a priority queue 
			ptemp = pq[0]; pq.erase(pq.begin()); // save the first element from vector and pop it (as q.front() and q.pop())
			ptemp.wait_time = btime - ptemp.Arrival_Time; // calculate waiting time for current process
			ptemp.Turn_time = ptemp.wait_time + ptemp.burst_time; // calculate turn around time for current process
			vec.push_back(ptemp); // push back to fininshed process queue (vector vec)
			btime += ptemp.burst_time; // increase the burst time to be used by the next process

			// check if after the process finished how many other processes had arrived
			while(processes[i].Arrival_Time <= btime)
			{	
				pq.push_back(processes[i]); i++;
			}	
		}					
		else
		{	// the same instruction BUT after pushing all the processes in the queue but not all are finished

			Process ptemp;
			sort(pq.begin(),pq.end(), burstTime);
			ptemp = pq[0]; pq.erase(pq.begin());
			ptemp.wait_time = btime - ptemp.Arrival_Time;
			ptemp.Turn_time = ptemp.wait_time + ptemp.burst_time;
			vec.push_back(ptemp);
			btime += ptemp.burst_time;
		}
	}

	// calculating AWT, ATT and ART
	for(auto i : vec)
	{
		sjf.AWT += i.wait_time;
		sjf.ATT += i.Turn_time;
		sjf.ART += i.response_time;
	}

	sjf.AWT /= num_line;
	sjf.ATT /= num_line;
	sjf.ART /= num_line;

	return sjf;
}