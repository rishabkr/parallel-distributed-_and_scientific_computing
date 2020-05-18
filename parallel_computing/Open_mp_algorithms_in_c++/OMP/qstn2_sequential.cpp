#include<stdio.h>
#include<iostream>
#include<omp.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<fstream>
using namespace std;
#define ll long long int
int main()
{	ofstream of;
	of.open("output_parallel.txt");
	ios_base::sync_with_stdio(false);
	cout.tie(NULL);
	cout<<"hellooo";
	static int arr[25][25][25][25]={0,0,0,0};
omp_set_num_threads(4);
 #pragma omp parallel
 {
 #pragma omp for collapse(3)
for (int i = 0 ; i < 25 ; i++)
for (int j = 0 ; j < 25 ; j++)
for (int k = 10 ; k < 25 ; k++)
for (int l = 10 ; l < 25 ; l++)
{
arr[i][j][k][l] = i + (j*2) + arr [i][j][k-10][l];
}
//#pragma omp barrier
}
 
 for (int i = 0 ; i < 25 ; i++)
 for (int j = 0 ; j < 25 ; j++)
 for (int k = 10 ; k < 25 ; k++)
 for (int l = 10 ; l < 25 ; l++)
{
	of<<arr[i][j][k][l]<<endl;
 }
 of.close();
return 0;
}

