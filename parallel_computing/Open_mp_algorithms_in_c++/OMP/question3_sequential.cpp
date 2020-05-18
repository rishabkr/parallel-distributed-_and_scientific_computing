#include <bits/stdc++.h>
using namespace std ;

void merge(int start, int end, int dir, int *arr)
{
	int N = end-start+1 ;
	for(int j = N/2 ; j > 0 ; j/=2)
	{
	
		for(int i = start ; i+j <= end ; ++i)
			if(dir == (arr[i] > arr[i+j]))
				swap(arr[i], arr[i+j]) ;
	}
}

void bitonic_sort(int start, int end, int *arr)
{
	int N = end-start+1 ;
	for(int j = 2 ; j <= N ; j *= 2)
		for(int i = 0 ; i < N ; i += j)
		{
			if( (i/j) % 2 == 0 )
				merge(i, i+j-1, 1, arr) ;
			else
				merge(i, i+j-1, 0, arr) ;
		}
}

int next_power_2(int x)
{
	int tmp = x ;
	while(tmp % 2 == 0)
		tmp = tmp >> 1 ;
	if(tmp == 1) return x ;
	int pow = 1 ;
	while(x)
	{
		x/=2 ;
		pow *= 2 ;
	}
	return pow ;
}

int main()
{
	//Number generation part.
	int n = 5e6 ;
	int N = next_power_2(n);
	cout << "N is " << N << '\n' ;
	int *A = new int[N] ;
	for(int i = 0 ; i < n ; ++i)
		A[i] = rand() ;
	for(int i = n ; i < N ; ++i)
		A[i] = 0 ;

	/*
	cout << "before : " ; 
	for(int i = 0 ; i < N ; ++i) cout << A[i] << ' ' ;
	cout << '\n' ; 
	*/

	bitonic_sort(0, N-1, A) ;

	/*
	cout << "after : " ; 
	for(int i = 0 ; i < N ; ++i) cout << A[i] << ' ' ;
	cout << '\n' ; 
	*/
	cout << "Done\n" ;
	delete[] A ;
	return 0 ;
}
