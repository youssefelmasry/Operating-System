#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
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

	short int org_btime = 0;

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

bool Dead_Line(Process& a, Process& b); // comparison function to sort struct
// scheduler polices functions 
Metrics EDF(Metrics, vector<Process> p); // Earliest Deadline First


int main()
{
	load_file("in.txt");
	Throughput = throughput();

	Metrics edf;

	edf = EDF(edf, Processes);
	cout<<edf.AWT<<endl;

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
		   	p.org_btime = stoi(token);

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

bool Dead_Line(Process& a, Process& b) // comparison function
{
	return a.Deadline < b.Deadline;
}

Metrics EDF(Metrics edf, vector<Process> p)
{
	vector <Process> pq; // priority queue according to burst time
	short int btime = 0, i = 1; // accumlating burst time
	vector <Process> vec; // temporary vector
	// a copy from vector Processes to not modify the main vector for the other schedulers
	vector <Process> processes;
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
			sort(pq.begin(),pq.end(), Dead_Line); // sort the vector as it is a priority queue

			ptemp = pq[0]; pq.erase(pq.begin()); // save the first element from vector and pop it (as q.front() and q.pop())

			if(ptemp.flag)
			{
			 	ptemp.start_time = btime;

			 	ptemp.flag = false;

			 	ptemp.response_time = ptemp.start_time - ptemp.Arrival_Time; // calculate the response time
			}

			// check if the current process's burst time is 1 to add it to the fininshed vector
			if(ptemp.burst_time == 1)
			{
				ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue

				vec.push_back(ptemp); // push back to fininshed process queue (vector vec)

				btime ++; // increase the burst time to be used by the next process
			}
			// if not, proceed
			else
			{
				// increament the btime while the currect process's burst time is less than the arrived smallest one's burst time
				while((ptemp.Deadline <= pq[0].Deadline) && ptemp.burst_time)
				{
					if(pq[0].flag && (ptemp.Deadline == pq[0].Deadline))
					{
					 	pq[0].start_time = btime;

					 	pq[0].flag = false;

					 	pq[0].response_time = pq[0].start_time - pq[0].Arrival_Time; // calculate the response time
					}
					btime++;

					while(processes[i].Arrival_Time <= btime && i+1 < num_line)
						{	
							pq.push_back(processes[i]); i++;

							sort(pq.begin(),pq.end(), Dead_Line);
						}

					ptemp.burst_time--;
				}
				// if process finished push it in the finished vector
				if(ptemp.burst_time == 0)
				{
					ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue
					
					vec.push_back(ptemp);
				}
				// else push it back in the priority queue
				else
					pq.push_back(ptemp);
			}	
			// check which process has arrived and push it in the priority queue
			while(processes[i].Arrival_Time <= btime && i < num_line)
			{	
				pq.push_back(processes[i]); i++;
			}	
		}					
		else
		{	// the same instruction BUT after pushing all the processes in the queue but not all are finished

			Process ptemp;
			sort(pq.begin(),pq.end(), Dead_Line);

			ptemp = pq[0]; pq.erase(pq.begin());

			if(ptemp.flag)
			{
			 	ptemp.start_time = btime;

			 	ptemp.flag = false;

			 	ptemp.response_time = ptemp.start_time - ptemp.Arrival_Time; // calculate the response time
			}
			
			btime += ptemp.burst_time;

			ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue

			vec.push_back(ptemp);
		}
	}

	// calculating AWT, ATT and ART
	for(auto i : vec)
	{
		cout<< i.process_name<<'\t'<<i.start_time<<'\t'<<i.end_time<<endl;
		
		short int tat = i.end_time - i.Arrival_Time;

		edf.ATT += tat;

		edf.AWT += tat-i.org_btime;
		
		edf.ART += i.response_time;
	}
	edf.AWT /= num_line;
	edf.ATT /= num_line;
	edf.ART /= num_line;

	return edf;
}
