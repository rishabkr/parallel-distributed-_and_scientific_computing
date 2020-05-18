#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#define THREADS_PER_BLOCK 1024


__global__ void st(int* a, int* max, int *min,int *sum,int* n)
{
    __shared__ int smax[THREADS_PER_BLOCK];
    __shared__ int smin[THREADS_PER_BLOCK];
    __shared__ int ssum[THREADS_PER_BLOCK];
    int N = *n;
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    // if (i < N)
      //   printf("%d\n", a[i]);
    if (i >= N)
        return;
    smax[tid] = a[i];
    smin[tid] = a[i];
    ssum[tid] = a[i];
    __syncthreads();
    for (unsigned int s = 1; s < blockDim.x && i < N; s *= 2)
    {
        if (tid % (2 * s) == 0)
        {
            if ((tid + s) < blockDim.x)//a[i] < a[i + s]
            {
                if(smax[tid] < smax[tid + s])
                    smax[tid] = smax[tid + s];//sdata[tid] = sdata[tid + s];
                if (smin[tid] > smin[tid + s])
                    smin[tid] = smin[tid + s];
                ssum[tid] += ssum[tid + s];
            }
        }
        __syncthreads();
    }
    if (tid == 0)
    {
        // printf("%d\n", sdata[0]);
        max[blockIdx.x] = smax[0];//sdata[0];
        min[blockIdx.x] = smin[0];
        sum[blockIdx.x] = ssum[0];
    }
}
__global__ void variance(int* a, double* var, double* av, int* n)
{
    __shared__ double svar[THREADS_PER_BLOCK];
    int N = *n;
    double mean = *av;
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    // if (i < N)
      //   printf("%d\n", a[i]);
    if (i >= N)
        return;
    svar[tid] = pow(a[i] - mean, 2);
    __syncthreads();
    for (unsigned int s = 1; s < blockDim.x && i < N; s *= 2)
    {
        if (tid % (2 * s) == 0)
        {
            if ((tid + s) < blockDim.x)//a[i] < a[i + s]
            {
                svar[tid] += svar[tid + s];
            }
        }
        __syncthreads();
    }
    if (tid == 0)
    {
        // printf("%d\n", sdata[0]);
        var[blockIdx.x] = svar[0];//sdata[0];
    }
}
double* stats(int* a, int N)
{
    int max = a[0], min = a[0];
    double tot = 0;
    double* res = (double*)malloc(sizeof(double) * 4);
    for (int i = 0; i < N; i++)
    {
        if (a[i] > max)
            max = a[i];
        if (a[i] < min)
            min = a[i];
        tot += a[i];
    }
    res[0] = min;
    res[1] = max;
    res[2] = tot / N;
    res[3] = 0;
    for (int i = 0; i < N; i++)
    {
        res[3] += pow((a[i] - res[2]), 2);
    }
    res[3] = sqrt(res[3] / N);
    return res;
}
int main(int argc, char* argv[])
{
    clock_t start, end;
    double cpu_time_used;
    int N = 1e8;
    int size = N * (int)(sizeof(int));
    int *a, *d_a,*max,*d_max,*d_n,*min,*d_min,*mean,*d_mean;
    double* var, * d_var, * d_av;
    a = (int*)malloc(size); 
    srand(0);
    for (int i = 0; i < N; i++)
    {
        a[i] = rand() + rand() + rand();
        //printf("%d\t", a[i]);
    }
    printf("\n");
    start = clock();
    double* res = stats(a, N);
    end = clock();
    printf("Min=%f\tMax=%f\tAverage=%f\tVariance=%f\n", res[0], res[1], res[2], res[3]);
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time elapsed in CPU implementation=%fs\n", cpu_time_used);
    int blocks = (int)ceil((double)N / THREADS_PER_BLOCK);
     max = (int*)malloc(blocks * sizeof(int));
     min = (int*)malloc(blocks * sizeof(int));
     mean = (int*)malloc(blocks * sizeof(int));
    cudaMalloc((void**)&d_a, size);
    cudaMalloc((void**)&d_n, sizeof(int));
    cudaMalloc((void**)&d_max, sizeof(int) * blocks);
    cudaMalloc((void**)&d_min, sizeof(int) * blocks);
    cudaMalloc((void**)&d_mean, sizeof(int) * blocks);
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_n, &N, sizeof(int), cudaMemcpyHostToDevice);
    start = clock();
    st <<<blocks, THREADS_PER_BLOCK >> > (d_a, d_max,d_min,d_mean, d_n);
    end = clock();
    cpu_time_used = ((double)(end - start));
    cudaMemcpy(max, d_max, sizeof(int) * blocks, cudaMemcpyDeviceToHost);
    cudaMemcpy(min, d_min, sizeof(int) * blocks, cudaMemcpyDeviceToHost);
    cudaMemcpy(mean, d_mean, sizeof(int) * blocks, cudaMemcpyDeviceToHost);
    cudaFree(d_max); cudaFree(d_min); cudaFree(d_mean); 
    start = clock();
    int mx = max[0];
    int mn = min[0];
    double av = 0;
    for (int i = 0; i < blocks; i++)
    {
        if (max[i] > mx)
            mx = max[i];
        if (min[i] < mn)
            mn = min[i];
        av += mean[i];
    }
    av /= N;
    end = clock();
    cpu_time_used += ((double)(end - start));
    var = (double*)malloc(blocks * sizeof(double));
    cudaMalloc((void**)&d_var, sizeof(double) * blocks);
    cudaMalloc((void**)&d_av, sizeof(double));
    cudaMemcpy(d_av, &av, sizeof(double), cudaMemcpyHostToDevice);
    start = clock();
    variance << <blocks, THREADS_PER_BLOCK >> > (d_a, d_var, d_av,d_n);
    end = clock();
    cpu_time_used += ((double)(end - start));
    cudaMemcpy(var, d_var, sizeof(double) * blocks, cudaMemcpyDeviceToHost);
    cudaFree(d_n); cudaFree(d_av); cudaFree(d_a); cudaFree(d_var);
    double va = 0;
    start = clock();
    for (int i = 0; i < blocks; i++)
    {
        va += var[i];
    }
    va /= N;
    va = sqrt(va);
    end = clock();
    cpu_time_used += ((double)(end - start));
    cpu_time_used/= CLOCKS_PER_SEC;
    printf("--------------------------------------\n");
    printf("Min = % d\tMax = % d\tAverage = % f\tVariance = % f\n", mn, mx, av, va);
    printf("Time elapsed in GPU implementation=%fs\n", cpu_time_used);
    return 0;
}
