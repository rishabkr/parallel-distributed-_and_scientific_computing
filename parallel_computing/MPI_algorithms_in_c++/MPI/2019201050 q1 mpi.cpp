# include <cmath>
# include <cstdlib>
# include <ctime>
# include <fstream>
# include <iostream>
# include <mpi.h>
#include <math.h>
#include <chrono>
#include <chrono>
# define M 1000
# define N 1000
double w[M][N];
double u[M][N];
// (size-1 must be divisor of M)

// mpic++ q1_mpi.cpp -o q1_mpi
// mpirun -np 5 q1_mpi

using namespace std;

const double epsilon = 0.001;

int main(int argc, char*argv[]){

  auto start = chrono::steady_clock::now();
 
  MPI_Init ( &argc, &argv );
  MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
  MPI_Comm_size ( MPI_COMM_WORLD, &size );

  if( M % (size-1) != 0){
    cout<<"Please input correct number of processes."<<endl;
    return 0;
  }

  if(rank == 0){
    long long epoch = 0;
    long long turn = 1;

    for (int i = 1; i < M - 1; i++ )
    {
      w[i][0] = 20.0;
    }
    for ( int i = 1; i < M - 1; i++ )
    {
      w[i][N-1] = 20.0;
    }
    for ( int j = 0; j < N; j++ )
    {
      w[M-1][j] = 20.0;
    }
    for (int j = 0; j < N; j++ )
    {
      w[0][j] = 20.0;
    }
    for ( int j = 300; j < N - 300; j++ )
    {
      w[0][j] = 100.0;
    }
    double diff = 10;
    int signal = 1;
    
    while(diff >= epsilon){
      for(int id = 1; id < size; id++)
      {     
            int tag = 1;
            MPI_Send(&signal, 1, MPI_INT, id, tag, MPI_COMM_WORLD );
      }

      int tm = M / (size-1);
      int tag = 2;

// for first
      MPI_Send(&w[0][0], (tm+1)*N , MPI_DOUBLE, 1, tag, MPI_COMM_WORLD );
      MPI_Recv(&u[0][0], (tm*N), MPI_DOUBLE, 1, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE );

// for internal
      for(int id = 2; id < size-1; id++){
        MPI_Send(&w[tm*(id-1)-1][0], (tm+2)*N , MPI_DOUBLE, id, tag, MPI_COMM_WORLD );
        MPI_Recv(&u[tm*(id-1)][0], (tm*N), MPI_DOUBLE, id, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
      }
      
// for last
      MPI_Send(&w[(tm*(size-2))-1][0], (tm+1)*N , MPI_DOUBLE, size-1, tag, MPI_COMM_WORLD );
      MPI_Recv(&u[(tm*(size-2))][0], (tm*N), MPI_DOUBLE, size-1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      double tempdiff = -1;

      for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            tempdiff = max(tempdiff, fabs(u[i][j] - w[i][j]));
        }
      }
      
      
      diff = tempdiff;
      ++epoch;
      // cout<<"epoch : "<< epoch <<" diff : "<<diff<<endl;
        
      if(turn == epoch)
      {
        cout<<"epoch : "<< epoch <<" diff : "<<diff<<endl;
        turn *= 2;
      }
      

      for(int i=0; i < M; ++i)
          for(int j =0; j < N; ++j)
              w[i][j] = u[i][j];

    }
    for(int id = 1; id < size; id++)
      {     
            signal = 0;
            int tag = 1;
            MPI_Send(&signal, 1, MPI_INT, id, tag, MPI_COMM_WORLD );
      }
    cout<<"epoch : "<< epoch <<" diff : "<<diff<<endl;
  }
  else
  {
    int recvsignal = 1;
    double diff = 10;
    int tm = M / (size-1);
    double tempw[tm+2][N];
    double u[tm][N];

    while(true){
      MPI_Recv(&recvsignal, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if(recvsignal == 0)
        break;
      if(rank == 1){
        MPI_Recv(&tempw, (tm+1)*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 1 ; i < tm; i++){
          for(int j = 1 ; j < N-1; j++){
              u[i][j] = ( tempw[i-1][j] + tempw[i+1][j] + tempw[i][j-1] + tempw[i][j+1] ) / 4.0;
          }
        }
        for(int i = 0; i < N; i++){
            u[0][i] = 20.0;
        }
        for(int i = 300; i < N-300; i++){
            u[0][i] = 100.0;
        }
        for(int i = 0; i < tm; i++){
            u[i][0] = 20.0;
            u[i][N-1] = 20.0;
        }
        MPI_Send(&u[0][0], tm*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

      }
      else if(rank == size-1){
        MPI_Recv(&tempw, (tm+1)*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 1 ; i <= tm-1; i++){
          for(int j = 1 ; j < N-1; j++){
              u[i-1][j] = ( tempw[i-1][j] + tempw[i+1][j] + tempw[i][j-1] + tempw[i][j+1] ) / 4.0;
          }
        }



         for(int i =0 ;i < N; i++){
          u[tm-1][i] = 20.0;
          }
          
        for(int i = 0; i < tm; i++){
          u[i][0] = 20.0;
          u[i][N-1] = 20.0;
        }

        MPI_Send(&u[0][0], tm*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
      }

     

      else{
        MPI_Recv(&tempw, (tm+2)*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 1; i < tm+1; i++){
          for(int j = 1; j < N-1; j++){
            u[i-1][j] = ( tempw[i-1][j] + tempw[i+1][j] + tempw[i][j-1] + tempw[i][j+1] ) / 4.0;
          }
        }

        for(int i = 0; i < tm; i++){
          u[i][0] = 20.0;
          u[i][N-1] = 20.0;
        }

        MPI_Send(&u[0][0], tm*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

      }
    }

   

  }
  MPI_Finalize();
  auto end = chrono::steady_clock::now();
  cout<<"Wallclock time = "<<chrono::duration_cast<chrono::seconds>(end-start).count()<<" sec";

  return 0;
}