//	Daniel Strawn
//	CSCI-4576

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvparser.h"
#include "mpi.h"

int main(
	int				argc		/*	i	*/,
	char**			argv		/*	i	*/)
{
	/*	initialize matricies from csv files: 	*/
	CsvParser* 		A			=	CsvParser_new("a.csv", ",", 0);
	CsvRow*			Arow;
					Arow		=	CsvParser_getRow(A);

	int				my_rank;
	int				p;
	int				q;
	int				i			=	0;
	int				j			=	0;
	int				k			=	0;
	int				dim			=	0;
	int				numARows	=	0;
	int				numACols	=	CsvParser_getNumFields(Arow);

	/*	initialize two-dimensional contiguous arrays	*/
	double**		a;
	double*			ai;
					a			=	(double	**)	malloc(numACols*sizeof(double*));
					ai			=	(double	*)	malloc(numACols*numACols*sizeof(double));
	
	double**		b;
	double*			bi;
					b			=	(double	**)	malloc(numACols*sizeof(double*));
					bi			=	(double	*)	malloc(numACols*numACols*sizeof(double));

	double**		c;
	double*			ci;
					c			=	(double	**)	malloc(numACols*sizeof(double*));
					ci			=	(double	*)	malloc(numACols*numACols*sizeof(double));
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

					dim 		= 	numACols;
					q			=	(int) dim/p;

	for (i = 0; i < dim; i++)
	{
		a[i] = ai + i*dim;
		b[i] = bi + i*dim;
		c[i] = ci + i*dim;
	}	

	/*	write the contents of a.csv, stored in CsvParser A, to a[][]	*/
	if (my_rank == 0)
	{
		printf("Reading csv...\n");
		do
		{
			const char	**rowFields = CsvParser_getFields(Arow);
			for (i = 0; i < CsvParser_getNumFields(Arow); i++)
			{
				a[numARows][i] = atof(rowFields[i]);
				b[numARows][i] = atof(rowFields[i]);
				//printf("%4.0f ", a[numARows][i]);
			}
			//printf("\n");
			CsvParser_destroy_row(Arow);
			numARows++;
		} while ((Arow = CsvParser_getRow(A)) && (numARows < numACols));

		/*	more redundant sanity checks	*/
		if(numARows != numACols)
		{
			printf("Matrix A is not square, aborting.\n");
			return 0;
		} 

		/*	purge A and B from memory.	*/
		CsvParser_destroy(A);
	}

	/*	Actual code starts here.	*/
	if (dim%p != 0)
	{
		MPI_Finalize();
		exit(-1);
	}

	MPI_Bcast(b, dim*dim, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Scatter(a, dim*q, MPI_FLOAT, a[my_rank*q], dim*q, MPI_FLOAT, 0, MPI_COMM_WORLD);

	for (i = my_rank*q; i < (my_rank+1)*q; i++)
		for (j = 0; j < dim; j++)
		{
			c[i][j] = 0.0;
			for (k = 0; k < dim; k++)
				c[i][j] += a[i][k]*a[k][j];
		}

	if (my_rank == 0)
		printf("collecting...\n");
		
	MPI_Gather(c[my_rank*q], dim*q, MPI_FLOAT, b, dim*q, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
	{
	for (i = 0; i < dim; i++)
	{
		for (j = 0; j < dim; j++)
		{
			printf("%4.0f", c[i][j]);
			if (j < (dim-1))
				printf(" ");
		}
		printf("\n");
	}
	}

	/*	purge matrices from memory.	*/
	free(a);
	free(ai);
	free(b);
	free(bi);
	free(c);
	free(ci);
	MPI_Finalize();
}
