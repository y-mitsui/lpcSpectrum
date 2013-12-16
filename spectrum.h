#define dfopen(fp,fileName,mode,errorProcess)	{	\
		if((fp=fopen(fileName,mode))==NULL){	\
			perror(fileName);		\
			errorProcess;			\
		}					\
}

#define dmalloc(p,size)	{				\
		if((p=malloc(size))==NULL){	\
			fatalError("out of memory");	\
		}					\
}

#define dcalloc(p,size)	{				\
		if((p=calloc(1,size))==NULL){	\
			fatalError("out of memory");	\
		}					\
}
