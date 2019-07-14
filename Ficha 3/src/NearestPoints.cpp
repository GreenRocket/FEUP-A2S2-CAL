/*
 * NearestPoints.cpp
 */

#include <limits>
#include <algorithm>
#include <cmath>
#include "NearestPoints.h"
#include "Point.h"

#define _WIN32_WINNT 0x0501
#include "mingw.thread.h"

const double MAX_DOUBLE = std::numeric_limits<double>::max();

Result::Result(double dmin, Point p1, Point p2) {
	this->dmin = dmin;
	this->p1 = p1;
	this->p2 = p2;
}

Result::Result() {
	this->dmin = MAX_DOUBLE;
	this->p1 = Point(0,0);
	this->p2 = Point(0,0);
}

/**
 * Auxiliary functions to sort vector of points by X or Y axis.
 */
static void sortByX(vector<Point> &v, int left, int right)
{
	std::sort(v.begin( ) + left, v.begin() + right + 1,
		[](Point p, Point q){ return p.x < q.x || (p.x == q.x && p.y < q.y); });
}

static void sortByY(vector<Point> &v, int left, int right)
{
	std::sort(v.begin( ) + left, v.begin() + right + 1,
		[](Point p, Point q){ return p.y < q.y || (p.y == q.y && p.x < q.x); });
}

/**
 * Brute force algorithm O(N^2).
 */
Result nearestPoints_BF(vector<Point> &vp)
{
	Result res;
	Point pI, pJ;
	double dst;
	for (unsigned i = 0; i < vp.size(); i++)
	{
		pI = vp[i];
		for (unsigned j = i + 1; j < vp.size(); j++)
		{
			pJ = vp[j];
			dst = pI.distance(pJ);
			if(dst < res.dmin)
			{
				res.p1 = pI;
				res.p2 = pJ;
				res.dmin = dst;
			}
		}
	}
	return res;
}

/**
 * Improved brute force algorithm, that first sorts points by X axis.
 */
Result nearestPoints_BF_SortByX(vector<Point> &vp) {
	Result res;
	sortByX(vp, 0, vp.size()-1);
	// TODO
	return res;
}


/**
 * Auxiliary function to find nearest points in strip, as indicated
 * in the assignment, with points sorted by Y coordinate.
 * The strip is the part of vp between indices left and right (inclusive).
 * "res" contains initially the best solution found so far.
 */
static void npByY(vector<Point> &vp, int left, int right, Result &res)
{
	Point pI, pJ;
	double dst;
	for (int i = left; i < right; i++)
	{
		pI = vp[i];
		for (int j = i + 1; j < right; j++)
		{
			pJ = vp[j];
			if (fabs(pI.y - pJ.y) > res.dmin)
				break;
			else if ( (dst = pI.distance(pJ)) < res.dmin)
			{
				res.p1 = pI;
				res.p2 = pJ;
				res.dmin = dst;
			}
		}
	}
}

/**
 * Recursive divide and conquer algorithm.
 * Finds the nearest points in "vp" between indices left and right (inclusive),
 * using at most numThreads.
 */
static Result np_DC(vector<Point> &vp, int left, int right, int numThreads)
{
	Result best;

	// Base case of two points - nearest points are next to each other
	if (right - left == 1)
	{
		best.p1 = vp[left];
		best.p2 = vp[right];
		best.dmin = vp[left].distance(vp[right]);
		return best;
	}

	// Base case of a single point: no solution, so distance is MAX_DOUBLE
	if (right <= left)
		return best;

	// Divide in halves (left and right) and solve them recursively,
	// possibly in parallel (in case numThreads > 1)
	int center;
	Result dL, dR;
	center = (left + right) / 2;
	if (numThreads < 2)
	{
		dL = np_DC(vp, left, center, numThreads);
		dR = np_DC(vp, center + 1, right, numThreads);
	}
	else
	{

		//std::thread t;

		std::thread t_np_DC { np_DC, vp, left, center, numThreads / 2 };
		dR = np_DC(vp, center + 1, right, numThreads / 2);

		 t_np_DC.join();
	}

	// Select the best solution from left and right
	best = (dL.dmin < dR.dmin) ? dL : dR;

	// Determine the strip area around middle point
	// Procurar elementos do meio para fora, cuja distancia seja menor que best.dmin
	int stripL = center, stripR = center;
	double centerX = vp[center].x;
	for (int i = stripL; i > left; i--)
	{
		if(centerX - vp[i].x <= best.dmin)
			stripL--;
		else
			break;
	}
	for (int i = stripR; i < right; i++)
	{
		if(vp[i].x - centerX <= best.dmin)
			stripR++;
		else
			break;
	}

	// Order points in strip area by Y coordinate
	sortByY(vp, stripL, stripR);

	// Calculate nearest points in strip area (using npByY function)
	npByY(vp, stripL, stripR, best);

	// Reorder points in strip area back by X coordinate
	sortByX(vp, stripL, stripR);

	return best;
}


/**
 * Defines the number of threads to be used.
 */
static int numThreads = 1;
void setNumThreads(int num)
{
	numThreads = num;
}

/*
 * Divide and conquer approach, single-threaded version.
 */
Result nearestPoints_DC(vector<Point> &vp) {
	sortByX(vp, 0, vp.size() -1);
	return np_DC(vp, 0, vp.size() - 1, 1);
}


/*
 * Multi-threaded version, using the number of threads specified
 * by setNumThreads().
 */
Result nearestPoints_DC_MT(vector<Point> &vp) {
	sortByX(vp, 0, vp.size() -1);
	return np_DC(vp, 0, vp.size() - 1, numThreads);
}
