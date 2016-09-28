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

	CsvParser*		B			=	CsvParser_new("b.csv", ",", 0);
	CsvRow*			Brow;
					Brow		=	CsvParser_getRow(B);

	int				dest;					/*	process I'm sending to	*/
	int				dim			=	0;		/*	dimension of output 	*/
	int				i			=	0;		/*	junk					*/
	int				j			=	0;		/*	junk					*/
	int				k			=	0;		/*	junk					*/
	int				my_rank;				/*	process rank			*/

	int				remRow;					/*	junk, dim%p				*/
	int				offset		=	0;		/*	row junk				*/

	int				p;						/*	number of processes		*/
	int				q;						/*	junk, dim/p				*/
	int				rows;					/*	row being sent			*/
	int				source;					/*	process I'm rec'v from	*/
	int				tag;					/*	From 0: 1, From p: 2	*/

	int				numARows	=	0;
	int				numACols	=	CsvParser_getNumFields(Arow);
	int				numBRows	=	0;
	int				numBCols	=	CsvParser_getNumFields(Brow);

					dim 		= 	numACols;

	/*	initialize two-dimensional contiguous arrays	*/
	double**		a;
	double*			ai;
					a			=	(double	**)	malloc(numACols*sizeof(double*));
					ai			=	(double	*)	malloc(numACols*numACols*sizeof(double));
	
	double**		b;
	double*			bi;
					b			=	(double	**)	malloc(numBCols*sizeof(double*));
					bi			=	(double	*)	malloc(numBCols*numBCols*sizeof(double));

	double**		c;
	double*			ci;
					c			=	(double	**)	malloc(numBCols*sizeof(double*));
					ci			=	(double	*)	malloc(numBCols*numBCols*sizeof(double));

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Status		status;

	/*	setup	*/
	if (my_rank == 0)
	{
		/*	sanity/error checks	*/
		if (numACols != numBCols)
		{
			printf("Input matrix dimensions do not match, aborting.\n");
			exit(-1);
		}
		if (p < 2)
		{
			printf("Process count must be greater than 2, aborting.\n");
			exit(-1);
		}
		if (dim%p != 0)
		{
			printf("%d\n", dim%p);
			MPI_Finalize();
			exit(-1);
		}

		for (i = 0; i < dim; i++)
		{
						a[i] 		= 	ai + i*dim;
						b[i] 		= 	bi + i*dim;
						c[i] 		= 	ci + i*dim;
		}	
	
		/*	write the contents of a.csv, stored in CsvParser A, to a[][]	*/
		do
		{
			const char	**rowFields = CsvParser_getFields(Arow);
			for (i = 0; i < CsvParser_getNumFields(Arow); i++)
			{
				a[numARows][i] = atof(rowFields[i]);
				//printf("%4.2f,", a[numARows][i]);
			}
			//printf("\n");
			CsvParser_destroy_row(Arow);
			numARows++;
		} while ((Arow = CsvParser_getRow(A)) && (numARows < numACols));
	
		/*	write the contents of b.csv, storied in CsvParser B, to b[][]	*/
		do
		{
			const char	**rowFields = CsvParser_getFields(Brow);
			for (i = 0; i < CsvParser_getNumFields(Brow); i++)
			{
				b[numBRows][i] = atof(rowFields[i]);
				//printf("%4.2f ", b[numBRows][i]);
			}
			//printf("\n");
			CsvParser_destroy_row(Brow);
			numBRows++;
		} while ((Brow = CsvParser_getRow(B)) && (numBRows < numBCols));
	
		/*	more redundant sanity checks	*/
		if(numARows != numACols)
		{
			printf("Matrix A is not square, aborting.\n");
			exit(-1);
		} 
	
		if(numBRows != numBCols)
		{
			printf("Matrix A is not square, aborting.\n");
			exit(-1);
		} 

		/*	purge A and B from memory.	*/
		CsvParser_destroy(A);
		CsvParser_destroy(B);
	}	/*	end setup	*/

	/*	Matrix multiplication.	*/

	MPI_Bcast(b,	dim*dim,
			MPI_DOUBLE,	0,	MPI_COMM_WORLD);
	MPI_Scatter(a,			dim*q,	MPI_DOUBLE,	
				a[source],	dim*q,	MPI_DOUBLE
				,	0,	MPI_COMM_WORLD);

	printf("working on slice %d (from row %d to %d)\n", my_rank, source, dest-1);
	for (i = source; i < dest; i++)
		for (j = 0; j < dim; j++)
		{
					c[i][j]		=	0;

			for (k = 0; k < dim; k++)
					c[i][j]		+=	a[i][k]*b[k][j];
		}

	MPI_Gather(c[source],	dim*q,
			MPI_DOUBLE,	c,	dim*q,	MPI_DOUBLE,	0,	MPI_COMM_WORLD);

	/*
	if (my_rank == 0)
	{
		for (dest = 1; dest <= p; p++)
		{
					rows		=	(dest <= remRow) ? q+1 : q ;

			printf("Sending %d rows to process %d offset=%d\n",rows,dest,offset);
			MPI_Send(&offset,	1,	
					MPI_INT,	dest,	tag,	MPI_COMM_WORLD);

			MPI_Send(&rows,		1,
					MPI_INT,	dest,	tag,	MPI_COMM_WORLD);

			MPI_Send(&a[offset][0],	rows*dim,	
					MPI_DOUBLE,	dest,	tag,	MPI_COMM_WORLD);

			MPI_Send(&b,	dim*dim,	
					MPI_DOUBLE,	dest,	tag,	MPI_COMM_WORLD);

			offset += rows;
		}
		
					tag			=	2;
		for (i = 1; i <= p; i++)
		{
					source		=	i;
			MPI_Recv(&offset,	1,
					MPI_INT,	source,	tag,	MPI_COMM_WORLD,	&status);
			MPI_Recv(&rows,		1,
					MPI_INT,	source,	tag,	MPI_COMM_WORLD,	&status);
			MPI_Recv(&
		}
	}
	*/
	if (my_rank == 0)
		for (i = 0; i < dim; i++)
		{
			for (j = 0; j < dim; j++)
			{
				printf("%4.2f", c[i][j]);
				if (j < (dim-1))
					printf(", ");
			}
			printf("\n");
		}

	/*	purge matrices from memory.	*/
	free(a);
	free(ai);
	free(b);
	free(bi);
	free(c);
	free(ci);

	MPI_Finalize();
	return 0;	
}
