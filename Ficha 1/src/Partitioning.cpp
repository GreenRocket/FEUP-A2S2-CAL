/*
 * Partioning.cpp
 */

#include "Partitioning.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

#define MIN(a, b) ((a) < (b)) ? (a) : (b);


int s_recursive(int n,int k)
{
	if (k == 1 || k == n)
		return 1;

	return s_recursive(n-1, k-1) + k*s_recursive(n-1,k);
}

int s_dynamic(int n, int k)
{
	//int c = 0;

	int len = n-k+1;
	int values[len]; // a column of the S(n,k) triangle

	// initialize column for k=1 (all 1's)
	for (int i = 0; i < len; i++)
	{
		//c++;
		values[i] = 1;
	}

	// compute the next columns up to the given k (similar to C(n,k))
	for (int i = 2; i <= k; i++)
	{
		for (int j = 1; j < len; j++)
		{
			//c++;
			values[j] += i * values[j-1];
		}
	}

	//cout << c << endl;
	return values[len-1];
}

int s_dynamic_better(int n,int k)
{
	if(n == 1) return (k == 1) ? 1 : -1;
	if(n < 1) return -1;

	//int c = 0;

	int kStart = 0;
	int kStop;
	int kValues[k];
	//vector<int> kValues(k);

	for(int nStep = 1; nStep < n; nStep++)
	{
		if (nStep > (n - k)) kStart++;
		kStop = MIN(nStep, k-1);

		for(int kStep = kStop; kStep >= kStart; kStep--)
		{
			//c++;
			if(kStep == 0 || kStep == nStep)
				kValues[kStep] = 1;
			else
				kValues[kStep] = kValues[kStep - 1] + (kStep+1) * kValues[kStep];

			// DEBUG INFO
//			cout << nStep + 1 << ":" << kStep + 1;
//			cout << " - Array Index = " << kStep << " - ";
//			if (kStep != 0 && kStep != nStep)
//			{
//				cout << "{ " << nStep << ":" << kStep << "->" << kValues[kStep - 1] << " + ";
//				cout << (kStep+1) << " * " << nStep << ":" << kStep + 1 << "->" << kValues[kStep];
//				cout << "} = ";
//			}
//			cout << kValues[kStep] << endl;
//			if (kStep == kStart) cout << endl;
		}

	}
	//cout << c << endl;
	return kValues[kStop];
}


int b_recursive(int n)
{
	int s = 0;
	for(int k = 1; k <= n; k++)
		s += s_recursive(n, k);
	return s;
}

int b_dynamic(int n)
{
	int values[n + 1]; // a line of the S(n,k) triangle

	// compute the lines up to the given n
	for (int i = 1; i <= n; i++)
	{
		values[i] = 1;
		for (int k = i - 1; k > 1; k--)
		{
			values[k] = values[k-1] + k * values[k];
			//cout << i << ":" << k << " - " << values[k] << endl;
		}
	}

	int sum = 0;
	for (int k = 1; k <= n; k++)
		sum += values[k];
	return sum;
}

void compare_s_dynamic(int n, int itr)
{
	s_dynamic(6, 4);
	s_dynamic_better(6, 4);


//	auto start = std::chrono::high_resolution_clock::now();
//	for (int t = 0; t < itr; t++)
//		for (int i = 5; i <= n; i++)
//			for (int k = 1; k <= i; k++)
//				s_dynamic(i, k);
//
//	auto finish = std::chrono::high_resolution_clock::now();
//	auto elapsed = chrono::duration_cast<chrono::microseconds>(finish - start).count();
//	cout << "dynamic - " << elapsed << endl << endl;
//
//	start = std::chrono::high_resolution_clock::now();
//	for (int t = 0; t < itr; t++)
//		for (int i = 1; i <= n; i++)
//			for (int k = 1; k <= i; k++)
//				s_dynamic_better(i, k);
//
//	finish = std::chrono::high_resolution_clock::now();
//	elapsed = chrono::duration_cast<chrono::microseconds>(finish - start).count();
//	cout << "better dynamic - " << elapsed << endl;
}

