#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <sys/stat.h>
#include "lpc.h"
#include "wave.h"
#include "error.h"
#include "spectrum.h"

static double *hamming(double *w, const int leng)
{
   int i;
   double arg;
   double *p;

   arg = 2*M_PI / (leng - 1);
   for (p = w, i = 0; i < leng; i++)
      *p++ = 0.54 - 0.46 * cos(i * arg);

   return (w);
}
void window( double *x, const int size){
	int i;
	double *w = NULL;

	w=malloc(sizeof(double)*size);

	hamming(w, size);

	for (i = 0; i < size; i++)
		x[i] = x[i] * w[i];
	free(w);
}

double *fir(double *x,int n,double* a,int na){
	double *y;
	int i,j;
	y=calloc(1,sizeof(double)*n);
	for(i=0;i<n;i++){
		for(j=0;j<na;j++){
			if(i-j>=0)
				y[i]+=a[j]*x[i-j];
		}
	}
	return y;
}


static void usage(void){
	fprintf(stderr,"Usage: spectrum [option] [wav file]\n");
	fprintf(stderr," -l [frame length]\n");
	fprintf(stderr," -p [frame shift]\n");
	fprintf(stderr," -d [lpc order]\n");
	fprintf(stderr," -n [frame number]\n");
  	exit(1);
}

#define FREQ_NUM 6000
int main(int argc,char **argv){
	double *frame;
	int j;
	int frameLength=320;
	int frameShift=320;
	int frameNumber=0;
	int lpcOrder = 16;

	int		ch;
	extern char	*optarg;
	extern int	optind, opterr;


	while ((ch = getopt(argc, argv, "l:p:d:n:")) != -1){
    		switch (ch){
    		case 'l':
			frameLength=atoi(optarg);
			break;
		case 'p':
			frameShift=atoi(optarg);
			break;
		case 'd':
			lpcOrder=atoi(optarg);
			break;
		case 'n':
			frameNumber=atoi(optarg);
			break;
		default:
		      usage();
		}
	}
	argc -= optind;
	argv += optind;

	if(argc!=1) usage();


	Sound *snd;
	if((snd = Read_Wave(argv[0])) == NULL){
		printf("error:%s\n",argv[0]);
		exit(1);
	}
	dmalloc(frame,sizeof(double)*frameLength);


	for(j=0;j<frameLength && (frameShift*frameNumber+j) < snd->datanum;j++){
		frame[j]=(double)snd->monaural16[frameShift*frameNumber+j];
	}

	//プリエンファシスフィルタをかける
	double b[2]={1.0,-0.99};
	frame=fir(frame,j,b,2);

	//ハミング窓
	window(frame,j);


	double *r = autocorr(frame,j, lpcOrder + 1);

		

	double *a;
	double e=LevinsonDurbin(r,&a,lpcOrder);

	double tmp[1];
	tmp[0]=sqrt(e);
	double *p=freqz(tmp,a,1,lpcOrder+1,FREQ_NUM);
		
	for(j=0;j<FREQ_NUM/2;j++){
		p[j]=20*log(p[j]);
		if( 2.0 * M_PI * j / (2048) > 0.5) break;
		printf("%.5lf\t%.5lf\n", 2.0 * M_PI * j / FREQ_NUM,p[j]);
	}
	free(p);
	free(r);
	free(a);
	
	return EXIT_SUCCESS;
}
