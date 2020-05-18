#include<stdio.h>
#include<iostream>
#include<omp.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
using namespace std;
#define ll long long int
double **allocate_matrix(int mat_size)
{
    double **matrix;
    matrix=(double**)malloc(mat_size*sizeof(double*));
    for(ll i=0;i<mat_size;i++)
    {
        matrix[i]=(double*)malloc(mat_size*sizeof(double));
    }

      for(int i=0;i<mat_size;i++)
      {
          for(int j=0;j<mat_size;j++)
          {
              matrix[i][j]=rand()%2020-10;
          }
      }
    return matrix;
}
void print_matrix(double **matrix,int mat_size)
{
   
for(int i=0;i<mat_size;i++)
{
    for(int j=0;j<mat_size;j++)
    {
        cout<<matrix[i][j]<<" ";
    }
    cout<<endl;
}
}

double** decompose_matrix_serial1(double **mat,int mat_size)
{
clock_t start,end;
start=clock();
for(int k=0;k<mat_size-1;k++)
{
    for(int l=k+1;l<mat_size;l++)
    {
        mat[l][k]=mat[l][k]/mat[k][k];
    }
    for(int i=k+1;i<mat_size;i++)
    {
        for(int j=k+1;j<mat_size;j++)
        {
            mat[i][j]=mat[i][j]-mat[i][k]*mat[k][j];
        }
    }
}
end=clock();
cout<<"time taken is "<< float(end-start)/CLOCKS_PER_SEC<<endl;
   return mat;   
}


double* allocate_b(int mat_size)
{
  double *b=(double*)malloc(sizeof(double)*mat_size);
  for(int i=0;i<mat_size;i++)
  {
    b[i]=rand()%200;
  }
  return b;
}

void print_vector(double *b,int mat_size)
{ 
    for(int i=0;i<mat_size;i++)
    {
        cout<<b[i]<<" ";
    }
    cout<<endl;
}

double* forward_substitution(double **L,double* b,int mat_size)
{
    //cout<<" L "<<endl;
    double *y=(double*)malloc(sizeof(double)*mat_size);
    for(int i=0;i<mat_size;i++)
    {
        y[i]=b[i];
    }
    for(int i=0;i<mat_size;i++)
    {
        double temp=b[i];
        for(int j=0;j<i;j++)
        {
            temp=temp-L[i][j]*y[j];
        }
        y[i]=temp;
    }
    return y;
}


double* backward_substitution(double **U,double* y,int mat_size)
{
 
    //cout<<" U "<<endl;
    double *x=(double*)malloc(sizeof(double)*mat_size);
    for(int i=0;i<mat_size;i++)
    {
        x[i]=0;
    }
    x[mat_size-1]=y[mat_size-1]/U[mat_size-1][mat_size-1];
    for(int i=mat_size-1;i>=0;i--)
    {
        double sum=y[i];
        for(int j=i+1;j<mat_size;j++)
        {
            sum=sum-U[i][j]*x[j];
        }
        x[i]=sum/U[i][i];
    }
    return x;
}

double** copy_matrix(double** matrix,int mat_size)
{
    double **saved_matrix;
    saved_matrix=(double**)malloc(mat_size*sizeof(double*));
    for(ll i=0;i<mat_size;i++)
    {
        saved_matrix[i]=(double*)malloc(mat_size*sizeof(double));
    }

    for(int i=0;i<mat_size;i++)
    {
        for(int j=0;j<mat_size;j++)
        {
            saved_matrix[i][j]=matrix[i][j]; 
        }
    }
    return saved_matrix;
}

double* copy_b(double *b,int mat_size)
{
double *saved_b=(double*)malloc(sizeof(double)*mat_size);
  for(int i=0;i<mat_size;i++)
  {
    saved_b[i]=b[i];
  }
  return saved_b;
}

double *solveforx(double** matrix,double* b,int mat_size)
{
    double **L=decompose_matrix_serial1(matrix,mat_size);
    double **U=L;
    double *y=forward_substitution(L,b,mat_size);
    double *x=backward_substitution(U,y,mat_size);
    return x;
}   

double get_l2_norm(double **a,double *x,double* b,int mat_size)
{
    double arr[mat_size];
    for(int i=0;i<mat_size;i++)
    {
        double sum=0;
        for(int j=0;j<mat_size;j++)
        {  
            sum+=a[i][j]*x[j];
        }
        // cout<<sum<<endl;
         cout<<"------------------------------"<<endl;
         //cout<<"b[i] :"<<b[i]<<" A[i][xi] "<<float(sum)<<endl;
         cout<<"b["<<i<<"]-A["<<i<<"][x"<<i<<"] "<<float(b[i]-sum)<<endl;
        arr[i]=b[i]-sum;
    }
    double l2sum=0;
    for(int i=0;i<mat_size;i++)
    {
        l2sum+=pow(arr[i],2);
    }
    return sqrt(l2sum);
}
int main()
{
int mat_size;
int num_threads;
cout<<"Enter the matrix size "<<endl;
cin>>mat_size;
//cout<<"Enter the number of threads to run "<<endl;
//cin>>num_threads;
//mp_set_num_threads(num_threads);
double  **matrix=allocate_matrix(mat_size);
//below code to take input matrix if needed
 // cout<<"enter matrix "<<endl;
 // for(int i=0;i<mat_size;i++)
 // {
 //     for(int j=0;j<mat_size;j++)
 //     {
 //         cin>>matrix[i][j];
 //     }
 // }
double **saved_matrix=copy_matrix(matrix,mat_size);
double *b=allocate_b(mat_size);
// cout<<"enter b "<<endl;
// for(int i=0;i<mat_size;i++)
// {
//     cin>>b[i];
// }
double *saved_b=copy_b(b,mat_size);

cout<<"generated  matrix "<<endl;
print_matrix(matrix,mat_size);
cout<<"generated vector \n";
print_vector(b,mat_size);
printf("----------------------------------------\n");
cout<<"calculated vector x is "<<endl;
double *x=solveforx(matrix,b,mat_size);
print_vector(x,mat_size);
double res=get_l2_norm(saved_matrix,x,saved_b,mat_size);
cout<<"----------------------------------------------"<<endl;
cout<<"error is "<<res<<endl;
 //print_matrix(saved_matrix,mat_size);
    return 0;
}