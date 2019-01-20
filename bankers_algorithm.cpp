#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

#define RESOURCES 4
#define PROCESSES 5

// Data Structure
int allocation[PROCESSES][RESOURCES] = {	{0,0,1,2},
												{1,0,0,0},
												{1,3,5,4},
												{0,6,3,2},
												{0,0,1,4}};

int maximum[PROCESSES][RESOURCES] = {	{0,0,1,2},
											{1,7,5,0},
											{2,3,5,6},
											{0,6,5,2},
											{0,6,5,6}};

int available[RESOURCES] = {1,5,2,0};

int work[RESOURCES] = {0};

bool finish[PROCESSES] = {false,false,false,false,false};

// Functions' prototypes
void need_func(int need_arr[][RESOURCES]);
void safety(int need_arr[][RESOURCES]);
void update_work(int position);
void print(int* arr, int* seq);

int main(int argc, char const *argv[])
{
	// Copy the available array into the work array.
	memcpy(work, available, RESOURCES * sizeof(int));

	int need[PROCESSES][RESOURCES]; // declare need matrix
	need_func(need); // calculate need matrix

	safety(need);

	return 0;
}
void need_func(int need_arr[][RESOURCES])
{
	// Need = Max - Allocation
	for (int i = 0; i < PROCESSES; ++i)
	{
		for (int j = 0; j < RESOURCES; ++j)
		{
			need_arr[i][j] = abs(maximum[i][j] - allocation[i][j]);
		}
	}
}

void safety(int need_arr[][RESOURCES])
{
	int done=-1;
	int sequence[PROCESSES] = {0}; int c =0;
	int fininshed = 0;
	bool good = true;

	while(done < RESOURCES)
	{
		for (int i = 0; i < PROCESSES; ++i)
		{
			if (!finish[i])
			{
				for (int j = 0; j < RESOURCES; ++j)
				{
					if (need_arr[i][j] > work[j])
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
void update_work(int process_num)
{
	for(int i = 0; i < RESOURCES; i++)
	{
		work[i] += allocation[process_num][i];
	}
}

void print(int* arr, int* seq)
{
	cout<<"Last Safe Available: \n";

	for (int i = 0; i < RESOURCES; ++i)
	{
		cout<<arr[i]<<' ';
	}cout<<endl;

	cout<<"The Safe Sequence: \n";

	for (int i = 0; i < PROCESSES; ++i)
	{
		cout<<seq[i]<<' ';
	}cout<<endl;
}