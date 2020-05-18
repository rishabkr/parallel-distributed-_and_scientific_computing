#include <cstdio>
#include <cstring>
#define N 1000

float h[N][N] ;
float room[N+2][N+2] ;

int main()
{
	const int fireplace_start = 0.3 * N ;
	const int fireplace_len = 0.4 * N ;
	memset(h, 0, sizeof h) ;
	memset(room, 0, sizeof room) ;	
	for(int i = 0 ; i < N+2 ; ++i)
	{
		if(i > fireplace_start && i <= fireplace_start + fireplace_len)
		       room[0][i] = 100 ;
		else
			room[0][i] = 20 ;	

		room[N+1][i] = 20 ;
	}

	for(int i = 1 ; i < N+1 ; ++i)
		room[i][0] = room[i][N+1] = 20 ;

	//init complete.

	for(int itr = 1 ; itr <= 100 ; ++itr)
	{
		for(int i = 1 ; i <= N ; ++i)
		{
			for(int j = 1 ; j <= N ; ++j)
			{
				h[i-1][j-1] = 0.25 *(room[i-1][j] + room[i+1][j] + room[i][j-1] + room[i][j+1]);
			}
		}
		for(int i = 1 ; i <= N ; ++i)
		{
			for(int j = 1 ; j <= N ; ++j)
			{
				room[i][j] = h[i-1][j-1] ;
			}
		}

	}
	/*
	for(int i = 0 ; i < N ; ++i)
	{
		for(int j = 0 ; j < N ; ++j)
			printf("%.2f ", h[i][j]);
		printf("\n");
	}
	*/
}
