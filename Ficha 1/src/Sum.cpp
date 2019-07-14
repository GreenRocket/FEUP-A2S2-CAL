/*
 * Sum.cpp
 */

#include "Sum.h"
#include <chrono>
#include <iostream>

using namespace std;

string calcSum(int* sequence, int size)
{
	int sum[size];
	int index[size];
	int currentSum;

	// m=1 -> largest value in sequence
	index[0] = 0;
	sum[0] = sequence[0];
	for(int i = 1; i < size; i++)
	{
		if(sequence[i] < sum[0])
		{
			index[0] = i;
			sum[0] = sequence[i];
		}
	}

	// 1 < m < 5 -> remaining cases
	for(int m = 1; m < size; m++)
	{
		// Base case -> first subsequence
		index[m] = 0;
		sum[m] = 0;
		for(int i = 0; i <= m; i++)
			sum[m] += sequence[i];
		currentSum = sum[m];

		for(int j = 1; j < size - m; j++)
		{
			currentSum += (sequence[j+m] - sequence[j-1]);
			if (currentSum < sum[m])
			{
				index[m] = j;
				sum[m] = currentSum;
			}
		}
	}

	// m=5 -> sum of sequence
	index[size-1] = 0;
	sum[size-1] = 0;
	for(int i = 0; i < size; i++)
		sum[size-1] += sequence[i];


	ostringstream oss;
	for(int m = 0; m < size; m++)
		oss << sum[m] << ',' << index[m] << ';';

	//cout << oss.str() << endl;

	return oss.str();
}

string calcSum2(int *sequence, int size)
{
    // sum[i] and index[i] will have the sum and start index of the
    // subsequence of length i+1 with smallest sum
    int sum[size];
    int index[size];
    // experiment with different start indices
    for (int i = 0; i < size; i++)
    {
        int val = 0; // sum of subsequence starting at index i
        for (int j = i; j < size; j++)
        {
            val += sequence[j];
            if (i == 0 || val < sum[j - i])
            {
                sum[j - i] = val;
                index[j - i] = i;
            }
        }
    }
    // return result as a string to compare more easily
    ostringstream oss;
    for (int i = 0; i < size; i++)
        oss << sum[i] << "," << index[i] << ";";
    return oss.str();
}

void testPerformanceCalcSum()
{
	srand(time(NULL)); //generates random seed

	int seq[1+1000];

	cout << "size; time" << endl;
	for (int size = 10; size <= 500; size += 10)
	{
		auto start = std::chrono::high_resolution_clock::now();
		for (int k = 0; k < 1000; k++)
		{
			for (int i = 0; i <size; i++)
				seq[i]= rand() % (10 * size) + 1;
			string res = calcSum(seq, size);
		}
		auto finish = std::chrono::high_resolution_clock::now();
		auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - start).count();
		cout << size << ";" << elapsed << endl;
	}
}

