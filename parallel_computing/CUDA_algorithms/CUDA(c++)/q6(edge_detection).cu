#include <stdio.h>
#include <cmath>
#include<stdlib.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;


__global__ void transform1st(float *a,float *b, int rows, int cols)
{
	//threadId -> Column 			blockId -> Row
	int self = threadIdx.x + blockIdx.x * blockDim.x;
	int l_u = ((threadIdx.x-1 >=0)&&(blockIdx.x-1 >=0)) ? ((threadIdx.x-1) + (blockIdx.x-1)* blockDim.x):-1;
	int l = (threadIdx.x-1 >=0) ? ((threadIdx.x-1) + blockIdx.x * blockDim.x):-1;
	int l_d = ((threadIdx.x-1 >=0)&&(blockIdx.x+1<rows)) ? ((threadIdx.x-1) + (blockIdx.x+1)* blockDim.x):-1;
	int r_u = ((threadIdx.x+1 < cols)&&(blockIdx.x-1>=0)) ? ((threadIdx.x+1) + (blockIdx.x-1)* blockDim.x):-1;
	int r = (threadIdx.x+1 < cols) ? ((threadIdx.x+1) + blockIdx.x * blockDim.x):-1;
	int r_d = ((threadIdx.x+1 <cols)&&(blockIdx.x+1 <rows)) ? ((threadIdx.x+1) + (blockIdx.x+1)* blockDim.x):-1;
	

	float luv = (l_u>=0) ? a[l_u] : 0;
	float lv = (l>=0) ? a[l] : 0;
	float ldv = (l_d>= 0) ? a[l_d] : 0;
	float ruv = (r_u >= 0) ? a[r_u] : 0;
	float rv = (r >= 0) ? a[r] : 0;
	float rdv = (r_d >= 0) ? a[r_d] : 0;
	
	b[self] = (-1*luv)+(-2*lv)+(-1*ldv)+(1*ruv)+(2*rv)+(1*rdv);
}

__global__ void transform2nd(float *a,float *b, int rows, int cols)
{
	//threadId -> Column 			blockId -> Row
	int self = threadIdx.x + blockIdx.x * blockDim.x;
	int l_u = ((threadIdx.x-1 >=0)&&(blockIdx.x-1 >=0)) ? ((threadIdx.x-1) + (blockIdx.x-1)* blockDim.x):-1;
	int u = (blockIdx.x-1 >=0) ? ((threadIdx.x) + (blockIdx.x-1) * blockDim.x):-1;
	int r_u = ((threadIdx.x+1 < cols)&&(blockIdx.x-1>=0)) ? ((threadIdx.x+1) + (blockIdx.x-1)* blockDim.x):-1;
	int l_d = ((threadIdx.x-1 >=0)&&(blockIdx.x+1<rows)) ? ((threadIdx.x-1) + (blockIdx.x+1)* blockDim.x):-1;
	int d = (blockIdx.x+1 <rows) ? ((threadIdx.x) + (blockIdx.x+1) * blockDim.x):-1;
	int r_d = ((threadIdx.x+1 <cols)&&(blockIdx.x+1 <rows)) ? ((threadIdx.x+1) + (blockIdx.x+1)* blockDim.x):-1;

	float luv = (l_u>=0) ? a[l_u] : 0;
	float uv = (u>=0) ? a[u] : 0;
	float ruv = (r_u >= 0) ? a[r_u] : 0;
	float ldv = (l_d>=0) ? a[l_d] : 0;
	float dv = (d >=0 ) ? a[d] : 0;
	float rdv = (r_d >= 0) ? a[r_d] : 0;

	b[self] = (-1*luv)+(-2*uv)+(-1*ruv)+(1*ldv)+(2*dv)+(1*rdv);
}

int main()
{
	Mat src, dst;
	float *src_a,*dst_a;
	src = imread("obh.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//namedWindow("start");
    	//imshow("start", src);
	dst = src.clone();
	printf("Rows = %d\n",src.rows);
	printf("Columns = %d\n",src.cols);
	

	src_a = (float *)malloc(sizeof(float)*src.rows*src.cols);
	dst_a = (float *)malloc(sizeof(float)*src.rows*src.cols);
	for(int y = 0; y < src.rows; y++)
            for(int x = 0; x < src.cols; x++)
	    		src_a[y*src.cols+x] = src.at<uchar>(y,x);
	/*Declaring GPU Arrays*/
	float *src_g,*dst_g,*final;
	int size = sizeof(float)*src.rows*src.cols;
	cudaMalloc((void **)&src_g,size);
	cudaMalloc((void **)&dst_g,size);
	

	cudaMalloc((void **)&final,size);
	cudaMemcpy(src_g, src_a, size, cudaMemcpyHostToDevice);
	transform1st<<<src.rows,src.cols>>>(src_g,dst_g,src.rows,src.cols);
	

	cudaMemcpy(dst_a, dst_g, size, cudaMemcpyDeviceToHost);
	cudaMemcpy(src_g, src_a, size, cudaMemcpyHostToDevice);
	

	transform2nd<<<src.rows,src.cols>>>(src_g,dst_g,src.rows,src.cols);
	cudaMemcpy(src_a, dst_g, size, cudaMemcpyDeviceToHost);
	cudaMemcpy(src_g, src_a, size, cudaMemcpyHostToDevice);
	cudaMemcpy(dst_g, dst_a, size, cudaMemcpyHostToDevice);
	

	for(int y = 0; y < src.rows; y++)
            for(int x = 0; x < src.cols; x++)
				dst.at<uchar>(y,x) = (uchar)dst_a[y*src.cols+x];
	/*Declaring GPU Arrays*/
	imwrite("obhfinal.jpg",dst);
	return 0;
}

