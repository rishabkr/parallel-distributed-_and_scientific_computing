#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include<stdlib.h>
#include <cstdio>
#include <cstring>
#define THREADS_PER_BLOCK 1024
__global__ void iterate(float *room, float *h,int *n,int *iter)
{
	int N = *n;
	int it = *iter;
	int i = blockIdx.x;
	int j = threadIdx.x;		//indices i,j of h
	if (j >= N)
		return;
	int ir=i+1;
	int jr=j+1;	//corresponding indices in room
	for (int k = 0; k < it; k++)
	{
		h[i * N + j] = 0.25 * (room[(ir - 1) * (N + 2) + jr] + room[(ir + 1) * (N + 2) + jr] + room[ir * (N + 2) + jr - 1] + room[ir * (N + 2) + jr + 1]);
		__syncthreads();
		room[ir * (N + 2) + jr] = h[i * N + j];
		__syncthreads();
	}
}
int main()
{
	int N = 1000,it=100;
	int *d_n,*d_it;
	float* h, * room;
	h = (float*)malloc(sizeof(float) * N * N);
	room = (float*)malloc(sizeof(float) * (N+2) * (N+2));
	const int fireplace_start = 0.3 * N;
	const int fireplace_len = 0.4 * N;

	memset(h, 0, sizeof h);
	memset(room, 0, sizeof room);

	for (int i = 0; i < N + 2; ++i)
	{
		if (i > fireplace_start&& i <= fireplace_start + fireplace_len)
			room[i] = 100;
		else
			room[i] = 20;

		room[(N+1)*(N+2)+i] = 20;
	}

	for (int i = 1; i < N + 1; ++i)
		room[i*(N+2)] = room[i*(N+2)+(N + 1)] = 20;
	float *d_h, *d_room;
	cudaMalloc((void**)&d_h, sizeof(float) *N*N);
	cudaMalloc((void**)&d_n, sizeof(int));
	cudaMalloc((void**)&d_it, sizeof(int));
	cudaMalloc((void**)&d_room, sizeof(float) * (N+2) * (N+2));
	cudaMemcpy(d_room, room, sizeof(float) * (N + 2) * (N + 2), cudaMemcpyHostToDevice);
	cudaMemcpy(d_n, &N, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_it,&it, sizeof(int), cudaMemcpyHostToDevice);
	iterate<<<N, THREADS_PER_BLOCK >> > (d_room,d_h,d_n,d_it);
	cudaMemcpy(h, d_h, sizeof(float) * N * N, cudaMemcpyDeviceToHost);
	cudaFree(d_n); cudaFree(d_it); cudaFree(d_h); cudaFree(d_room);
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
			printf("%.2f ", h[i*(N)+j]);
		printf("\n");
	}
}
