/*
	matread: read a 2D array from a text file.
	
	Supports NaN and doubles...
	Meant to be a fast replacement for matlab's load command.
	
	Daniel Roggen, 2013-2017
*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <mex.h>

#define printf mexPrintf


//extern "C" int parse(char *data,int *sx,int *sy,double **outdata,bool **outnan);
int parse(char *data,int *sx,int *sy,double **outdata,bool **outnan,int fixedcol);

#define MAXFNAME 255



void Syntax()
{
	printf("matread reads a 2D array from a text file, space separated.\n");
	printf("matread is meant to be a fast replacement to matlab's load.\n\n");
	printf("   data = matread(filename[,numcol])\n\n");
	printf("   filename: file name in which to write the matrix\n");
	printf("   numcol:   optional parameter indicating how many columns to create in data.\n");
	printf("             numcol can be used to handle with files with lines of various sizes.\n");
	printf("             if a line has less than numcol samples the remaining columns are filled with zeros.\n");
	printf("             if a line has more than numcol samples these samples are lost.\n");
	printf("   data:     data read from the file\n");
}
void Error(char *s)
{
	mexErrMsgTxt(s);
}


void mexFunction( int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[] )
{
	char filename[MAXFNAME+1];
	int sx,sy;
	int x,y;
	double r=0;
	int fwerr=0;
	int percent=0;
	FILE *f;
	int ok=false;
	unsigned fsize;
	char *buffer;
	bool *outnan;
	double *outdata;
	double *mptr;
	size_t n;
	long long nanvar = 0xfff6ac007ffead00;
	int fixedcol=0;		// Zero=not fixed columns
	
	// Check in/out args
	if(nlhs!=1)
	{
		Syntax();
		Error("Missing output argument");
	}
	if( !(nrhs==1 || nrhs==2) )
	{
		Syntax();
		Error("Invalid input argument");
	}
	
	if(nrhs==2)
	{
		// Fixed number of columns
		if(mxGetClassID(prhs[1]) != mxDOUBLE_CLASS || mxGetM(prhs[1])!=1  || mxGetN(prhs[1])!=1)
		{
						
			Syntax();
			Error("Invalid number of columns");
		}
		// Get the number of columns
		fixedcol = *mxGetPr(prhs[1]);
		//printf("Fixed columns: %d\n",fixedcol);
	}
	
	if(mxGetClassID(prhs[0]) != mxCHAR_CLASS)
	  Error("Error: filename must be a string");
	
	mxGetString(prhs[0],filename,MAXFNAME);
	
	///printf("Reading %s\n",filename);
	f=fopen(filename,"rb");
	if(f==0)
	{
	  Error("Error: can't open file");
	}
	
	
	fseek(f, 0L, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);
		
	buffer = (char*)malloc((fsize+1)*sizeof(char));
	if(!buffer)
		Error("Error: can't allocate enough memory");


	n=fread(buffer,1,fsize,f);
	if( n!=fsize )
	{
		free(buffer);
		Error("Error: can't read the file");
	}	
	fclose(f);
	
	// Null terminate the data.
	buffer[fsize]=0;

	ok = parse(buffer,&sx,&sy,&outdata,&outnan,fixedcol);
	free(buffer);
	

	if(ok!=0)
		Error("Error: can't parse file");
	
	//printf("Read a %d x %d array from %s\n",sy,sx,filename);

	plhs[0] = mxCreateDoubleMatrix(sy,sx,mxREAL);
	mptr = mxGetPr(plhs[0]);
	
	for(y=0;y<sy;y++)
	{
		for(x=0;x<sx;x++)
		{
			if(outnan[y*sx+x])
				*(long long*)&mptr[x*sy+y] = nanvar;
			else
				mptr[x*sy+y] = outdata[y*sx+x];
		}
	}

	free(outdata);
	free(outnan);

}





