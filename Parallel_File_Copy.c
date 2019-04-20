// Cagdas Bilecen - 2014510015
// The input parameters/arguments should be entered like "./x - - y"
// Parameter 1 : First "-" means = create source.txt on same folder
// Parameter 2 : Second "-" means = create destination.txt on same folder
// Parameter 3 : Number of threads
// All the parameters should be entered with spaces between each other.
// Please run the code as follows : 
// Compile: gcc pthread -lrt -o test 2014510015.c
// Run: ./test - - 10
// If you run the file with another file length/size, you must delete previous files that has created.(To avoid possible problems.)
// All requirements of and all controls has done.(with md5sum)
// Simulation updated when transferred data for one thread, reaches %50 and %100.
// lenghtOfFile is default file size (bytes), you can change it.


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <aio.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <semaphore.h>
#define SOURCE_FILE_NAME "source.txt"
#define DESTINATION_FILE_NAME "destination.txt"
int lenghtOfFile = 3000 ; //BYTE // length of file // you can change this size.
//probably it is not gonna create a problem but; if you want to change the size of file, firstly; please delete old destination file to avoid possible problems.


//--------------------------------------------//

//global variables//
int carriedBytes;
int threadNumber; // number of threads
int lastOne; //to control bytes of last part
FILE * fp; //source
FILE * fp2; //destination
char destinationPath[100];
char sourcePath[100];
int percentages[10]; //for the bonus part of homework. size of 10 as threads number.
int remainderAlerter=0; // alert remaining characters to terminal screen if lenghtOfFile%threadNumber!=0
sem_t mutex; //for safety

//dynamically display of process percentage of each thread
void* simulation(){
	int x=0;
	
	while(x<threadNumber){
	printf("Thread number %d has carried own bytes --> %c %d\n",x+1,37,percentages[x]); //37 is ascii code of '%'
	x++;	
	}
	printf("----------- Part completed ----------- \n");
	}

/*void *algorithmIfRemainder() {  //requirement algorithm when file length % thread number not 0

	int participation = lenghtOfFile / threadNumber;
	int modVar = lenghtOfFile%threadNumber;
	int minusVar = lenghtOfFile-lastOne;
	controlLastThread=1;
	lastOne=modVar;
	lenghtOfFile=minusVar;
	lenghtOfFile=participation; //there is no lastOne left.
} */

void *randomLetterGenerator() { 
	int lp= 0;
	char randomletter;
	while(lp<lenghtOfFile) {
	randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random () % 26];
	int i= 0;
	fprintf(fp,"%c",randomletter);
	
	lp++;
	
}

	/*if(remainderAlerter==1){ //remainderAlerter is 1 state (filelength%threadNumber!=0);
		int j=0;
		for(j=0;j<lastOne; j++){
			fprintf(fp,"%c",randomletter);
		}
	} */
}

void *fileCopier(void *arg) {  // the thread function.
	sem_init(&mutex,0,1); //safety
	int id=(int)arg; // assign thread's  id
	percentages[id-1]=0; //initially 0 and simulate it //id-1 cause arg is 1
	simulation(); 


	//read from source 
	int fd=0,err,ret,fd2=0;
	int size; //byte lenght that current thread's will be read. 
	
	if(id==threadNumber && remainderAlerter==1){ // the last thread reads the rest.
		size = ((lenghtOfFile)/threadNumber) + lastOne;
		//size=lenghtOfFile/threadNumber;
	} 
	else {
	size = lenghtOfFile/threadNumber; }
	
	
	char *data = (char*)malloc(size);
	struct aiocb aio_r ,aio_w;
	fd= open(sourcePath,O_RDONLY);
	if(fd==-1) {
		perror("open");
		return 0;
	}
	memset(&aio_r,0,sizeof(aio_r));
	memset(data,0,sizeof(data));
	aio_r.aio_fildes=fd;
	aio_r.aio_buf=data;
	aio_r.aio_nbytes = size;
	

	aio_read(&aio_r);

	while(aio_error(&aio_r)==EINPROGRESS) { }

	err=aio_error(&aio_r);
	ret=aio_return(&aio_r);

	if(err!=0){
		printf("Error at aio_error(): %s\n",strerror(err));
		close(fd);
		exit(2);
	}
	percentages[id-1]=percentages[id-1]+50; //incremented and simulate
	simulation();


	//writing on destination start //
	
	fd2 = open(destinationPath, O_WRONLY|O_CREAT, 0777);
	if(fd2 == -1){
		perror("can not be open");
		return 0;
	}
	memset(&aio_w,0,sizeof(struct aiocb));
	aio_w.aio_fildes=fd2;
	aio_w.aio_buf=data;
	aio_w.aio_nbytes =size;

	

	aio_write(&aio_w);

	while(aio_error(&aio_w)==EINPROGRESS){ }
	err=aio_error(&aio_w);
	ret=aio_return(&aio_w);
	
	if(err!=0){
		printf("Error at aio_error(): %s\n",strerror(err));
		close(fd2);
		exit(2);
	}
	percentages[id-1]=percentages[id-1]+50; //incremented again and simulate
	simulation();

	close(fd);
	close(fd2);
	//writing on destination end //
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	
	//controll parameter 1 
	if(strcmp(argv[1] , "-") == 0){ //same path
		strcpy(sourcePath,SOURCE_FILE_NAME);
	}
	else //if source path is different folder ! not current folder
	{
		strcpy(sourcePath , argv[1]);
		strcat(sourcePath , SOURCE_FILE_NAME); //concating//
		
		
	}
	//controll parameter 2
	if(strcmp(argv[2], "-") == 0){ //same path
		strcpy(destinationPath,DESTINATION_FILE_NAME); 
	}
	else //if destination path is different folder ! not current folder
		{
		strcpy(destinationPath, argv[2]);
		strcat(destinationPath,DESTINATION_FILE_NAME); //concating
		}


	//controll parameter 3 
	threadNumber=atoi(argv[3]);
	
	//controll thread numbers
	if(threadNumber>10 ){
		printf("Thread number only 0<ThreadNum<10. Enter true number!\n");
		return 0;
	}	
	if( threadNumber<=0 ) { 
		printf("Thread number can not be smaller than zero or equal to.\n");
		return 0 ;	
	}


	//controll length of file
	if(lenghtOfFile>100000000){ //100 MB
		printf("File length can not be greater than 100 MB!\n");
		return 0;
	}
	if(lenghtOfFile<1) { 
		printf("File length can not be smaller than 1.\n");
		return 0;
	}
	
		
	
	
	
	//CREATING SOURCE FILE START////
	fp = fopen(sourcePath , "w"); //first, open file .
	if(fp==NULL) {
		printf("error, file can not be open.");
		return 0; 
	}
	int participation = lenghtOfFile / threadNumber;
	int modVar = lenghtOfFile%threadNumber;
	int minusVar = lenghtOfFile-lastOne;
	lastOne=0;
	if(modVar==0) {
		//lenghtOfFile= participation; 
	}
	else {  //if modVar not 0  then last thread should carry this characters.
		
		remainderAlerter=1;
		//printf("kalanlar var");
		lastOne=modVar; //number of remaining 
		lenghtOfFile= lenghtOfFile;
		//algorithmIfRemainder(); //invoke the algorithm
	}


	
	//write random letters to source file ! 
	randomLetterGenerator();
	
	fclose(fp); //close file
	
	
	//CREATING SOURCE FILE END /////////////
	
	//thread creations
	pthread_t tid[threadNumber];
	
	int i=0;
	while(i<threadNumber){
	if((pthread_create(&(tid[i]),NULL,fileCopier, (void *)i+1))<0){
		printf("Thread could not be created!");
		return 0;
	}
	printf("----------- Thread number %d has created! ---------- \n" , i+1);
	i++;
	}

	sleep(2); //waiting for let threads done its job.
	printf("#All file has been copied from source to destination.\n");
	if(remainderAlerter==1) { 
	printf("#There is some remaining characters!\n");
	printf("#Last thread carried the remaining characters\n");
	
	}
	printf("#Source Path: %s\n",sourcePath);
	printf("#Destination Path: %s\n",destinationPath);
	//printf("last one %d",lastOne);
	pthread_exit(NULL); //thread exit

	
	//reset percentages
	int ext=0;
	for(ext=0;ext<threadNumber;ext++){
		percentages[ext]=0;
	 
	}
	}
	
