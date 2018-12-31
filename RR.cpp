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
	short int start_time = 0, end_time = 0;

	short int wait_time = 0, response_time = 0, Turn_time = 0;
	
	short int Arrival_Time = 0, burst_time = 0, Deadline = 0;
	
	bool flag = true;
	char process_name;
};
// vector of all processes info
vector <Process> Processes;

/*Funcitons prototype*/

// file load and parse
void load_file(string filename);
void parse(vector <string> vec);
float throughput();

bool Sort_burstTime(Process& a, Process& b); // comparison function to sort struct

// scheduler polices function
Metrics RR(Metrics, vector<Process> p, short int quantam_time); // Shortest Remaining Time First

int main()
{
	load_file("in.txt");
	Throughput = throughput();

	Metrics rr;

	short int q_time = 1;

	rr = RR(rr, Processes, q_time);
	cout<<rr.ART<<endl;

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

bool Sort_burstTime(Process& a, Process& b) // comparison function
{
	return a.burst_time < b.burst_time;
}

Metrics RR(Metrics RR, vector<Process> p, short int quantam_time)
{
	vector <Process> pq; // priority queue according to burst time
	short int btime = 0, i = 1; // total burst time and line counter
	vector <Process> vec; // temporary vector
	
	vector <Process> processes; // a copy from vector Processes to not modify the main vector for the other schedulers
	processes = p;

	pq.push_back(processes[0]);
	processes[0].flag = false;

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
			 
			ptemp = pq[0]; pq.erase(pq.begin()); // save the first element from vector and pop it (as q.front() and q.pop())

			// check after the pop to set the start time of the current process as the current btime 
			if(ptemp.flag)
			 {
			 	ptemp.start_time = btime;

			 	ptemp.flag = false;

			 	ptemp.response_time = ptemp.start_time - ptemp.Arrival_Time; // calculate the response time
			 }

			// check if the current process's burst time is less than or equal the quantum time to add it to the fininshed vector
			if(ptemp.burst_time <=  quantam_time)
			{
				ptemp.wait_time = btime - ptemp.Arrival_Time - ptemp.wait_time; // calculate waiting time for current process

				ptemp.Turn_time = ptemp.wait_time + ptemp.burst_time;  // calculate turn around time for current process

				btime += ptemp.burst_time; // increase the burst time by the remaining burst time to be used by the next process

				ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue

				vec.push_back(ptemp); // push back to fininshed process queue (vector vec)	
			}
			// if not, proceed
			else
			{
				ptemp.wait_time += quantam_time; // to remove it later (the number of running time)

				ptemp.burst_time -= quantam_time; // decrease the burst time by quantum time

				btime += quantam_time; // increase the total burst time by quantum time

				pq.push_back(ptemp);
			}	

			// check which process has arrived and push it in the priority queue
			while(processes[i].Arrival_Time <= btime)
			{	
				pq.push_back(processes[i]); i++;
			}	
		}					
		else
		{	// the same instruction BUT after pushing all the processes in the queue but not all are finished

			Process ptemp;
			 
			ptemp = pq[0]; pq.erase(pq.begin());

			if(ptemp.flag) 
			{
				ptemp.start_time = btime; 

				ptemp.flag = false;

				ptemp.response_time = ptemp.start_time - ptemp.Arrival_Time;
			}
			
			if(ptemp.burst_time <=  quantam_time)
			{
				ptemp.wait_time = btime - ptemp.Arrival_Time - ptemp.wait_time;

				ptemp.Turn_time = ptemp.wait_time + ptemp.burst_time;  

				btime += ptemp.burst_time; 

				ptemp.end_time = btime;

				vec.push_back(ptemp); 	
			}
			// if not, proceed
			else
			{
				ptemp.wait_time += quantam_time;
				ptemp.burst_time -= quantam_time;
				btime += quantam_time;

				pq.push_back(ptemp);
			}
		}
	}
	
	// calculating AWT, ATT and ART
	for(auto i : vec)
	{
		RR.AWT += i.wait_time;
		RR.ATT += i.Turn_time;
		RR.ART += i.response_time;
	}
	RR.AWT /= num_line;
	RR.ATT /= num_line;
	RR.ART /= num_line;

	return RR;
}