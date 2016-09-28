// Daniel Strawn
// CSCI-4576

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

int main(
	int				argc		/*	i	*/,
	char**			argv		/*	i	*/)
{
	const char 		*short_opt = "g:m:i:r:";
	struct			option	long_opt[] =
	{
		{"granularity",	required_argument,	NULL,	'g'},
		{"iterations",	required_argument, 	NULL,	'i'},
		{"messages",	required_argument,	NULL,	'm'},
		{"runs",		required_argument, 	NULL,	'r'},
		{NULL,			0,					NULL,	0}
	};

	int 	size 			= 1;
	int 	iterations 		= 1;
	int		m 				= 2;
	int		runs			= 1;
	int 	my_rank;
	int		p;
	int		opt;
	int		j;

	/*	quick and dirty way to adjust granularity of linear message test	*/
	int granularity;
	granularity 			= 512;

	double	mTick;
	double	sum				=	0;
	double	average			=	0;
	double	mean			=	0;
	double 	sum1 			= 	0;
	double	variance 		= 	0;
	double	std_deviation;
	double	trialTime;
	double	error;

	while (-1 != (opt = getopt_long(argc, argv, short_opt, long_opt, NULL)))
	{
		switch(opt)
		{
			case 'i':
			iterations 	= atoi(optarg);
			break;

			case 'm':
			m 			= atoi(optarg);
			if(my_rank == 0)
				printf("m: %d\n", m);

			if ((m != 0) && (m != 1) && (m != 2))
			{
				if (my_rank == 0)
					printf("Invalid Message argument.\n Options: \n	0: increase size by powers of 2 upto 4MB. \n 1: increase size linearly upto 16KB.\n");
				return 0;
			}
			break;

			case 'g':
			if(m == 1)
				granularity	= atoi(optarg);
				
			if(my_rank == 0)
				printf("granularity: %d\n", granularity);
			break;

			case 'r':
				runs = atoi(optarg);
			break;
		}
	}

	double	runResult[runs];

	double benchmark(
		int			rank			/*	i	*/,
		int			trials			/*	i	*/,
		int			numBytesk		/*	i	*/,
		MPI_Comm	comm			/*	i	*/);

	int	ipow(
		int			base			/*	i	*/,
		unsigned int 	exp			/*	i	*/);
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if(my_rank == 0)
	{
		mTick = MPI_Wtick();
		printf("MPI Tick for current machine: %2.1E\n", mTick);
	}

	if(	m == 2)
		benchmark(my_rank, iterations, size, MPI_COMM_WORLD);
	else if( m == 1)
	{
		for(int i = granularity; i < (16384 + granularity); i += granularity)
		{
			sum = 0;
			for(int r = 0; r < runs; r++)
			{
				mean	=	benchmark(my_rank, iterations, i, MPI_COMM_WORLD);
				if(my_rank == 0)
				{
					sum				+=	mean;
					runResult[r]	=	mean;
					average			=	sum	/ (double) runs;
					sum1			+=	pow((runResult[r] - average), 2);
				}
			}
			variance		=	sum1 / (double) runs;
			std_deviation	=	sqrt(variance);
			error			=	1.96*0.025*(std_deviation/sqrt((float) runs));
			if(my_rank == 0)
			{
				//printf("standard deviation: %4.2f\n", std_deviation);	
				printf("Average latency to send %d bytes over %d iterations: %4.2f microseconds +/- %4.2E\n",	i,	iterations,	mean, error);
				//printf("error: %4.2E\n", error);
			}
			
		}
	}
	else
	{
		for(j = 1; j < (int) 23; j++)
		{
			int power = j;
			size = (int) ipow(2, power);
			sum = 0;

			for(int r = 0; r < runs; r++)
			{
				mean 	= 	benchmark(my_rank, iterations, size, MPI_COMM_WORLD); 
				if(my_rank == 0)
				{
					sum				+=	mean;
					runResult[r]	=	mean;
					average			=	sum	/ (double) runs;
					sum1			+=	pow((runResult[r] - average), 2);
				}
			}
			variance		=	sum1 / (double) runs;
			std_deviation	=	sqrt(variance);
			error			=	1.96*0.025*(std_deviation/sqrt((float) runs));
			if(my_rank == 0)
			{
				//printf("standard deviation: %4.2f\n", std_deviation);	
				printf("Average latency to send %d bytes over %d iterations: %4.2f microseconds +/- %4.2E\n",	size,	iterations,	mean, error);
				//printf("error: %4.2E\n", error);
			}
		} 
	}

	MPI_Finalize();
}

double benchmark(		
	int 		rank			/*	i	*/, 
	int			trials			/*	i	*/, 
	int			numBytes		/*	i	*/,
	MPI_Comm 	comm			/*	i	*/)
{

	char	*message;
	message =	(char *) malloc(numBytes);

	for(int i = 0; i < numBytes; i++)
		message[i] 			=	'q';

	double 	start;
	double	finish;

	MPI_Status status;

	if (rank == 0)
	{
		MPI_Barrier(comm);

		start = MPI_Wtime();
		
		for(int i = 0; i < trials; i++)
		{
			MPI_Send(message, sizeof(message), 
				MPI_CHAR, 1, 0, comm);

			MPI_Recv(message, sizeof(message), 
				MPI_CHAR, 1, 0, comm, &status);
		}
		finish = MPI_Wtime();
	}
	else
	{
		MPI_Barrier(comm);

		for(int i = 0; i < trials; i++)
		{
			MPI_Recv(message, sizeof(message), 
				MPI_CHAR, 0, 0, comm, &status);

			MPI_Send(message, sizeof(message), 
				MPI_CHAR, 0, 0, comm);
		}
	
	}

	if (rank == 0)
	{
		finish 			-= 	start;
		finish 			/= 	(double) trials;		
		finish 			*= 	1000000.0;
	}

	free(message);

	return finish;
}

/*	helper function sourced from stackoverflow.com/questions/101439/	*/
int ipow(int base, unsigned int exp)
{
    int ret = 1;
    while ( exp != 0 )
    {
        if ( (exp & 1) == 1 )
            ret *= base;
        base *= base;
        exp = exp >> 1;
    }
    return ret;
}
