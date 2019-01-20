#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

#define RESOURCES 4
#define PROCESSES 5

// Data Structure

short int allocation[PROCESSES][RESOURCES] = {	{0,0,1,2},
												{1,0,0,0},
												{1,3,5,4},
												{0,6,3,2},
												{0,0,1,4}};

short int maximum[PROCESSES][RESOURCES] = {	{0,0,1,2},
											{1,7,5,0},
											{2,3,5,6},
											{0,6,5,2},
											{0,6,5,6}};

short int need[PROCESSES][RESOURCES]; // declare need matrix

short int available[RESOURCES] = {1,5,2,0};

short int work[RESOURCES] = {0};

bool finish[PROCESSES] = {false,false,false,false,false};

// Functions' prototypes

void need_func(short int need_arr[][RESOURCES]); // calculate need matrix

void safety(); // check if the state if safety or not (safety algorithm)

void update_work(short int position); // update work(available) if process's need <= available

void print(short int* arr, short int* seq); // print the safety sequence and last available

void resouce_request(short int* req, short int process_num); // request resource(req) for a process(process_num)

bool less_need_available(short int* req, short int process_num); // check if request resource less than available and need



int main(int argc, char const *argv[])
{
	// Copy the available array short into the work array.
	memcpy(work, available, RESOURCES * sizeof(int));

	need_func(need); 

	short int request[] = {0,2,1,0}; // request instance

	resouce_request(request, 4);

	return 0;
}

void need_func(short int need_arr[][RESOURCES])
{
	// Need = Max - Allocation
	for (short int i = 0; i < PROCESSES; ++i)
	{
		for (short int j = 0; j < RESOURCES; ++j)
		{
			need_arr[i][j] = abs(maximum[i][j] - allocation[i][j]);
		}
	}
}

void safety()
{
	short int done=-1; // set to any number other than 0 because of the first iteration in the while loop

	short int sequence[PROCESSES] = {0}; short int c =0; // put the finished processes in sequence, and a counter c

	short int fininshed = 0; // a variable to check if a process cannot change its state by remaining the same value twice

	bool good = true; // to check if need less than work or not

	while(done < RESOURCES)
	{
		for (short int i = 0; i < PROCESSES; ++i)
		{
			if (!finish[i])
			{
				for (short int j = 0; j < RESOURCES; ++j)
				{
					if (need[i][j] > work[j])
					{
						good = false;
						break;
					}
				}
				// if need[i] <= work
				if (good)
				{
					update_work(i);
					finish[i] = true;
					sequence[c++] = i;
				}
				else
					good = true;
			}
			else
				done++;	
		}
		
		if (done == PROCESSES)
		{
			print(work, sequence);
			break;
		}

		else if (done == fininshed)
		{
			cout<<"allocation is imposible\n";
			break;
		}
		else
			fininshed = done;
		done = 0;
	}
}

// new available = current available + allocation
void update_work(short int process_num)
{
	for(short int i = 0; i < RESOURCES; i++)
	{
		work[i] += allocation[process_num][i];
	}
}

void print(short int* arr, short int* seq)
{
	cout<<"Last Safe Available: \n";

	for (short int i = 0; i < RESOURCES; ++i)
	{
		cout<<arr[i]<<' ';
	}cout<<endl;

	cout<<"The Safe Sequence: \n";

	for (short int i = 0; i < PROCESSES; ++i)
	{
		cout<<seq[i]<<' ';
	}cout<<endl;
}

void resouce_request(short int* req, short int process_num)
{
	if (less_need_available(req, process_num))
	{
		// update the values
        for(short int i = 0; i < RESOURCES; i++) 
        {
		    available[i] -= req[i];
		    allocation[process_num][i] += req[i];
		    need[process_num][i] -= req[i];
		}
		//memcpy(work, available, RESOURCES * sizeof(int));

		// check for safety after update the table
		safety();
	}
}

bool less_need_available(short int* req, short int process_num)
{
	// check if req(proc_Num) is less than need(proc_Num)
	for (int i = 0; i < RESOURCES; ++i)
	{
		if (need[process_num][i] > req[i])
		{
			cout<<"The request can't be given because it exceeds the need\n";
			exit(1);
		}
		if (available[i] > req[i])
		{
			cout<<"The request can't be given because it exceeds the available\n";
			exit(1);
		}
	}
	return true;
}