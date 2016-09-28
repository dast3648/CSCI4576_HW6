//	Daniel Strawn
//	CSCI-4576

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvparser.h"

int main(
	int				argc		/*	i	*/,
	char**			argv		/*	i	*/)
{
	/*	initialize matricies from csv files: 	*/
	CsvParser* 		A			=	CsvParser_new("a.csv", ",", 0);
	CsvRow*			Arow;
					Arow		=	CsvParser_getRow(A);

	CsvParser*		B			=	CsvParser_new("a.csv", ",", 0);
	CsvRow*			Brow;
					Brow		=	CsvParser_getRow(B);

	int				i			=	0;
	int				j			=	0;
	int				k			=	0;
	int				dim			=	0;
	int				numARows	=	0;
	int				numACols	=	CsvParser_getNumFields(Arow);
	int				numBRows	=	0;
	int				numBCols	=	CsvParser_getNumFields(Brow);

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

	/*	sanity check	*/
	if(numACols != numBCols)
	{
		printf("Input matrix dimensions do not match, aborting.\n");
		return 0;
	}

	dim = numACols;

	for (i = 0; i < dim; i++)
	{
		a[i] = ai + i*dim;
		b[i] = bi + i*dim;
		c[i] = ci + i*dim;
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
		return 0;
	} 

	if(numBRows != numBCols)
	{
		printf("Matrix A is not square, aborting.\n");
		return 0;
	} 

	/*	purge A and B from memory.	*/
	CsvParser_destroy(A);
	CsvParser_destroy(B);

	/*	Actual code starts here.	*/
	for (i = 0; i < dim; i++)
	{
		for (j = 0; j < dim; j++)
		{
			c[i][j] = 0.0;
			for (k = 0; k < dim; k++)
			{
				c[i][j] += a[i][k]*b[k][j];
			}
			printf("%4.0f", c[i][j]);
			if (j < (dim-1))
				printf(" ");
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
	return 0;	
}
