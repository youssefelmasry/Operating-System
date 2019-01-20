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
	string policy_name = "";
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

vector <Process> vec_arr[5];

/*Funcitons prototype*/

// file load and parse
void load_file(string filename);

void parse(vector <string> vec);

float throughput();

bool Sort_burstTime(Process& a, Process& b); // comparison function to sort struct by burst time

bool Dead_Line(Process& a, Process& b); // comparison function to sort struct by dead line

bool awt_sort(Metrics& a, Metrics& b); // comparison function to sort struct by awt

bool art_sort(Metrics& a, Metrics& b); // comparison function to sort struct by art

bool att_sort(Metrics& a, Metrics& b); // comparison function to sort struct by att

void ouput_file(string); // put the result in an output file function

void best_performance(Metrics* arr);

void tasks_list(Metrics* arr);

void modify(Metrics* arr, string);

template <typename T> string tostr(const T& t) { 
   ostringstream os; 
   os<<t; 
   return os.str(); 
}

// scheduler polices functions 
Metrics FCFS(Metrics, vector<Process> p); // First Come First Serve policy
Metrics SJF(Metrics, vector<Process> p); // Shortest Job First
Metrics SRTF(Metrics, vector<Process> p); // Shortest Remaining Time First
Metrics RR(Metrics RR, vector<Process> p, short int quantam_time); // Round Robin
Metrics EDF(Metrics, vector<Process> p); // Earliest Deadline First

int main(int argc, char const *argv[])
{
	if(argc != 3)
    {
        cout<< "please Enter a valid input format\n\t ./scheduler <filename.txt> <quantum_number>\n";
        exit(0);
    }
	load_file(argv[1]);
	short int q_time = stoi(argv[2]);
	Throughput = throughput();

	Metrics fcfs;
	Metrics sjf;
	Metrics srtf;
	Metrics rr;
	Metrics edf;

	fcfs = FCFS(fcfs, Processes);

	sjf = SJF(sjf, Processes);

	srtf = SRTF(srtf, Processes);

	rr = RR(rr, Processes, q_time);

	edf = EDF(edf, Processes);

	Metrics arr[5] = {fcfs, sjf, srtf, rr, edf};
	
	best_performance(arr);

	tasks_list(arr);

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

bool Sort_burstTime(Process& a, Process& b) // comparison function
{
	return a.burst_time < b.burst_time;
}

bool Dead_Line(Process& a, Process& b) // comparison function
{
	return a.Deadline < b.Deadline;
}

bool awt_sort(Metrics& a, Metrics& b) // comparison function
{
	return a.AWT < b.AWT;
}

bool art_sort(Metrics& a, Metrics& b) // comparison function
{
	return a.ART < b.ART;
}

bool att_sort(Metrics& a, Metrics& b) // comparison function
{
	return a.ATT < b.ATT;
}

Metrics FCFS(Metrics fcfs, vector<Process> p)
{
	short int i,j;
	vector<Process> processes;
	processes = p;

	processes[0].end_time = processes[0].burst_time;

	// Calculating waiting time and response time. i.e: the response time equal waiting time in fcfs policy
	// starting from 1 (second process) because the first one does not wait
	for(i = 1; i<num_line; i++)
	{
		for(j=0; j<i; j++){
			processes[i].wait_time += processes[j].burst_time ;
		}
		processes[i].wait_time -= processes[i].Arrival_Time;
		fcfs.AWT+=processes[i].wait_time;

		processes[i].start_time = processes[i].wait_time;
		processes[i].end_time = processes[i].wait_time + processes[i].burst_time;

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
	fcfs.policy_name = "fcfs";

	vec_arr[0] = processes;

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

			sort(pq.begin(),pq.end(), Sort_burstTime); // sort the vector as it is a priority queue 

			ptemp = pq[0]; pq.erase(pq.begin()); // save the first element from vector and pop it (as q.front() and q.pop())

			if(ptemp.flag)
			{
			 	ptemp.start_time = btime;

			 	ptemp.flag = false;

			 	ptemp.response_time = ptemp.start_time - ptemp.Arrival_Time; // calculate the response time
			}			

			btime += ptemp.burst_time; // increase the burst time to be used by the next process

			ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue

			vec.push_back(ptemp); // push back to fininshed process queue (vector vec)

			

			// check if after the process finished how many other processes had arrived
			while(processes[i].Arrival_Time <= btime && i < num_line)
			{	
				pq.push_back(processes[i]); i++;
			}	
		}					
		else
		{	// the same instruction BUT after pushing all the processes in the queue but not all are finished

			Process ptemp;

			sort(pq.begin(),pq.end(), Sort_burstTime);

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
		//cout<< i.process_name<<'\t'<<i.start_time<<'\t'<<i.end_time<<endl;
		
		short int tat = i.end_time - i.Arrival_Time;

		sjf.ATT += tat;

		sjf.AWT += tat-i.org_btime;
		
		sjf.ART += i.response_time;
	}

	sjf.AWT /= num_line;
	sjf.ATT /= num_line;
	sjf.ART /= num_line;
	sjf.policy_name = "sjf";

	vec_arr[1] = vec;

	return sjf;
}

Metrics SRTF(Metrics srtf, vector<Process> p)
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
			sort(pq.begin(),pq.end(), Sort_burstTime); // sort the vector as it is a priority queue

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
				btime ++; // increase the burst time to be used by the next process

				ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue

				vec.push_back(ptemp); // push back to fininshed process queue (vector vec)
			}
			// if not, proceed
			else
			{
				// increament the btime while the currect process's burst time is less than the arrived smallest one's burst time
				while((ptemp.burst_time <= pq[0].burst_time) && ptemp.burst_time)
				{
					if(pq[0].flag && (ptemp.burst_time == pq[0].burst_time))
					{
					 	pq[0].start_time = btime+1;

					 	pq[0].flag = false;

					 	pq[0].response_time = pq[0].start_time - pq[0].Arrival_Time; // calculate the response time
					}
					btime++;

					while(processes[i].Arrival_Time <= btime && i+1 < num_line)
						{	
							pq.push_back(processes[i]); i++;

							sort(pq.begin(),pq.end(), Sort_burstTime);
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
			sort(pq.begin(),pq.end(), Sort_burstTime);

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
		short int tat = i.end_time - i.Arrival_Time;

		srtf.ATT += tat;

		srtf.AWT += tat-i.org_btime;
		
		srtf.ART += i.response_time;
	}
	srtf.AWT /= num_line;
	srtf.ATT /= num_line;
	srtf.ART /= num_line;
	srtf.policy_name = "srtf";

	vec_arr[2] = vec;

	return srtf;
}

Metrics RR(Metrics rr, vector<Process> p, short int quantam_time)
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

				btime += ptemp.burst_time; // increase the burst time by the remaining burst time to be used by the next process

				ptemp.end_time = btime; // set then end time as the current total burst time before pushing the process to the fininshed queue

				vec.push_back(ptemp); // push back to fininshed process queue (vector vec)	
			}
			// if not, proceed
			else
			{

				ptemp.burst_time -= quantam_time; // decrease the burst time by quantum time

				btime += quantam_time; // increase the total burst time by quantum time

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
			 
			ptemp = pq[0]; pq.erase(pq.begin());

			if(ptemp.flag) 
			{
				ptemp.start_time = btime; 

				ptemp.flag = false;

				ptemp.response_time = ptemp.start_time - ptemp.Arrival_Time;
			}
			
			if(ptemp.burst_time <=  quantam_time)
			{
				btime += ptemp.burst_time; 

				ptemp.end_time = btime;

				vec.push_back(ptemp); 	
			}
			// if not, proceed
			else
			{
				ptemp.burst_time -= quantam_time;

				btime += quantam_time;

				pq.push_back(ptemp);
			}
		}
	}
	
	// calculating AWT, ATT and ART
	
	// Turn around time = completion time - arrival time
	// Waiting time = Turn around time - burst time

	for(auto i : vec)
	{
		
		short int tat = i.end_time - i.Arrival_Time;

		rr.ATT += tat;

		rr.AWT += tat-i.org_btime;
		
		rr.ART += i.response_time;
	}
	rr.AWT /= num_line;
	rr.ATT /= num_line;
	rr.ART /= num_line;
	rr.policy_name = "rr";

	vec_arr[3] = vec;

	return rr;
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
		short int tat = i.end_time - i.Arrival_Time;

		edf.ATT += tat;

		edf.AWT += tat-i.org_btime;
		
		edf.ART += i.response_time;
	}
	edf.AWT /= num_line;
	edf.ATT /= num_line;
	edf.ART /= num_line;
	edf.policy_name = "edf";

	vec_arr[4] = vec;

	return edf;
}

void ouput_file(string result)
{
	ofstream ouput;
	ouput.open("output.txt", ios::out | ios::app);
	ouput<<result<<"\n\n";
	ouput.close();
}

void modify(Metrics* arr, string info)
{
	// convert from float to str and put the whole string into the file as one line
	if(info == "AWT\t")
	{
		for (int i = 0; i < 5; ++i)
		{
			info += arr[i].policy_name + "(" + tostr(arr[i].AWT) + ")"+ "\t\t";
		}
	}
	
	else if(info == "ART\t")
	{
		for (int i = 0; i < 5; ++i)
		{
			info += arr[i].policy_name + "(" + tostr(arr[i].ART) + ")"+ "\t\t";
		}
	}

	else if(info == "ATT\t")
	{
		for (int i = 0; i < 5; ++i)
		{
			info += arr[i].policy_name + "(" + tostr(arr[i].ATT) + ")"+ "\t\t";
		}
	}
	// put into ouput file
	ouput_file(info);
}
void best_performance(Metrics* arr)
{
	ouput_file("scheduler polices best_performance: ");

	Metrics copy_array[5]; // copy of arr to not modify it to use it later
	copy(arr, arr+5, copy_array);

	// sort the according to least time of awt, art and att of all polices

	sort(copy_array, copy_array+5, awt_sort);

	modify(copy_array, "AWT\t");

	sort(copy_array, copy_array+5, att_sort);

	modify(copy_array, "ATT\t");

	sort(copy_array, copy_array+5, art_sort);

	modify(copy_array, "ART\t");
}

void tasks_list(Metrics* arr)
{
	ouput_file("\n\n\nTasks List: ");
	for (int i = 0; i < 5; ++i)
	{
		string line = "Policy: ";
		line += arr[i].policy_name;
		ouput_file(line);

		line = "Task\t\tStart Time\t\tEnd Time\t\tDuration";
		ouput_file(line);

		for(auto j : vec_arr[i])
		{
			line = j.process_name;
			line += "\t\t\t\t" + tostr(j.start_time) + "\t\t\t\t" + tostr(j.end_time) + "\t\t\t\t" + tostr(j.end_time - j.start_time); 
			ouput_file(line);
		}	
	}
}