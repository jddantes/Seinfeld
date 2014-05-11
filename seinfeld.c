#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct{
	char goal[100];
	int done;
	char journal[100];
	int id;
} entry;

entry * ptr = 0;
int n = 0;

char date[10];
char date_label[25];

void fatal (char * message);
int numDig (int n);
void writeFile();
void readFile();
void display(int state);

int main(){

	/*
		Get timeinfo
	*/

	time_t rawtime;
	time (&rawtime);

	struct tm * timeinfo = localtime(&rawtime);

	strftime (date,10,"%Y%b%d",timeinfo);
	strftime (date_label,25,"%B %d, %Y",timeinfo);


	/*
		Read goals and initialize array
	*/

	FILE * fp = fopen("goals","r");

	if(fp == NULL){
		fatal("file \"goals\" not found");
	}

	int i;

	char buffer[120];
	while(fgets(buffer, 120, fp) != NULL){
		if(buffer[strlen(buffer)-1]=='\n')
			buffer[strlen(buffer)-1] = 0;

		n++;
		ptr = (entry *) realloc(ptr, n*sizeof(entry));

		int id;
		sscanf(buffer, "%d", &id);

		if(n!=id){
			char err_message[50];
			sprintf(err_message, "goals incorrectly numbered: expected %d, read %d", n, id);
			fatal(err_message);
		}

		ptr[n-1].id = n;
		strcpy(ptr[n-1].goal, buffer+numDig(id)+1);
		ptr[n-1].done = 0;
		ptr[n-1].journal[0] = 0;
	}

	fclose(fp);

	writeFile();

	if( ( fp = fopen(date, "r") ) == NULL)
		writeFile(); 
	else 
		readFile();


	fclose(fp);
	
	/*
		Do actions
	*/

	display(0);	



	/*
		End of Program
	*/
	writeFile();

	free(ptr);
	ptr = NULL;

	return 0;
}

void fatal(char * message){
	printf("seinfeld: error: %s\n",message);
	exit(0);
}

int numDig (int n){
	if(n<0)
		n = -n;
	if (n == 0)
		return 1;
	int cnt = 0;
	while(n>0){
		n/=10;
		cnt++;
	}
	return cnt;

}

void writeFile(){
	/*
		Write to date file
	*/

	FILE * fp = fopen(date, "w");

	if(fp == NULL){
		fatal("could not write to date file");
	}

	fgets(date_label, 25, fp);

	fprintf(fp, "%s\n", date_label);
	int i;
	for(i=0; i<n; i++){
		fprintf(fp, "\n");
		fprintf(fp, "%d %d\n", ptr[i].id, ptr[i].done);
		fprintf(fp, "%s\n", ptr[i].goal);
		if(ptr[i].done){
			fprintf(fp, "[DONE] %s\n", ptr[i].journal);
		} else {
			fprintf(fp, "[NOT DONE]\n");
		}
	}

	fclose(fp);
}

void readFile(){
	/*
		Read date file 
	*/

	FILE * fp = fopen(date, "r");

	if(fp == NULL){
		fatal("failed reading date file");
	}

	if(fp != NULL){
		fgets(date_label, 25, fp);
		date_label[strlen(date_label)-1] = 0;
		int i;
		for(i=0; i<n; i++){
			char buffer[120];
			fgets(buffer,120,fp); // For blank lines between entries
			fgets(buffer, 120, fp);
			sscanf(buffer, "%d %d", &ptr[i].id, &ptr[i].done); 

			fgets(buffer, 120, fp);
			buffer[strlen(buffer)-1] = 0;
			strcpy(ptr[i].goal, buffer);

			fgets(buffer, 120, fp);
			buffer[strlen(buffer-1)] = 0;
			if(ptr[i].done && strlen(buffer) > 5){
				strcpy(ptr[i].journal, buffer+5);
			} else {
				ptr[i].journal[0] = 0;
			}
		}
	}

	fclose(fp);
}

void display(int state){
	int i;
	for(i=0; i<n; i++){
		if(ptr[i].done == state){
			printf("%d %s\n", ptr[i].id, ptr[i].goal);
		}
	}
}