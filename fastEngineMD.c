#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Functions prototypes
void openFile(); //used to open the file
char readNextCharacter(); //used the read the characters os the file
void fastDecoder(); //used to call all the functions of the FAST decoder
void readField(); //used to read which field of the FAST message
void readNextByte(); //used to read which byte of the FAST message
void identifyField(); //used to identify which field of the message
void identifyMessage1(); //used to get the principal message of the FIX fields
int binaryToInt(long long n);

FILE* file; //variable shared by openFile and readNextCharacter
int isFirstByte = 1; //is it the first byte of the field = true
char field[5000]; //var used to save the field of the message
char byte[8]; //var used to save which byte of the field of the message
int status_eof = 0; //used to know if is it the end of the file. Iniciates with false

int main(){
	openFile();
	fastDecoder();
}

void openFile(){
	file = fopen("fast.01.log", "r");
}

char readNextCharacter(){
	char character = ' ';

	if(fread(&character, 1, 1, file) > 0)
		return character;	
	else
		return '\0'; //return NULL
}

void fastDecoder(){
	for(int i=0; i<5000; i++){
		field[i] = ' ';
	}

	while(!status_eof){ //
		readField();
	}
}

//Get the fields
void readField(){
	readNextByte();
	if(isFirstByte == 1){
		strcpy(field, byte);
		isFirstByte = 0; //false
		if(byte[0] == '1'){
			identifyField();
		}
	}
	else{
		strcat(field, byte);
		if(byte[0] == '1'){
			identifyField();
			isFirstByte = 0;
		}
	}
}

void readNextByte(){
	for(int i=0; i<8; i++){ //get 1 byte
		byte[i] = readNextCharacter();
		if(byte[i] == '\0' || byte[i] == '*') //if character equals NULL
			status_eof = 1; //true
	}
}

//identify the fields
int statusSeqNumber = 0; //false
int seqNumber = 0; //false

int statusPMap = 0; //false
int pMap[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int statusTemplateId = 0; //false
int templateId = 0;

int statusMsg = 0; //false

void identifyField(){

	if(statusSeqNumber == 0){
		seqNumber = binaryToInt(atoi(field)) - 128;
		printf("\n seqNumber: %d \n", seqNumber);
		statusSeqNumber = 1; //true
		strcpy(field, "");
	}
	else if(statusPMap == 0){
		for(int i = 0; i < 8; i++){
			pMap[i] = field[i] - 48;
			printf("\n pMap: %d ", pMap[i]);
		}
		statusPMap = 1;
		strcpy(field, "");
	}
	else if(templateId == 0){
		templateId = binaryToInt(atoi(field)) - 128;
		printf("\n\n TemplateID: %d \n", templateId);
		statusTemplateId = 1; //true
		strcpy(field, "");
	}
	else if(statusMsg == 0){
		if(templateId == 1){
			identifyMessage1();
		}
	}

	if(statusSeqNumber == 1 && statusPMap == 1 && statusTemplateId == 1 && statusMsg == 1){
		statusSeqNumber = 0;
		statusPMap = 0;
		statusTemplateId = 0;
		statusMsg = 0;
	}
}

//get the fields of the message
//template 1: Symbol | MDEntryType | MDEntryPx | MDEntrySize | MDEntryTime. 

int statusSymbol = 0;
int statusMDEntryType = 0;
int statusMDEntryPx = 0;
int statusMDEntrySize = 0;
int statusMDEntryTime = 0;

char symbol[50];
int MDEntryType = -1;
float MDEntryPx = 0.0;
int MDEntrySize = 0;
char MDEntryTime[15];

void identifyMessage1(){
	if(statusSymbol == 0){
		strcpy(symbol, field);
		printf("\n\n Symbol: %s \n", symbol);
		statusSymbol = 1; //true
		strcpy(field, "");
	}
	else if(statusMDEntryType == 0){
		MDEntryType = binaryToInt(atoi(field)) - 128;
		printf("\n MDEntryType: %d \n", MDEntryType);
		statusMDEntryType = 1; //true
		strcpy(field, "");
	}
	else if(statusMDEntryPx == 0){
		MDEntryPx = binaryToInt(atoi(field)) - 128;
		printf("\n MDEntryPx: %.2f \n", MDEntryPx);
		statusMDEntryPx = 1; //true
		strcpy(field, "");
	}
	else if(statusMDEntrySize == 0){
		MDEntrySize = binaryToInt(atoi(field)) - 128;
		printf("\n MDEntrySize: %d \n", MDEntrySize);
		statusMDEntrySize = 1; //true
		strcpy(field, "");
	}
	else if(statusMDEntryTime == 0){
		strcpy(MDEntryTime, field);
		printf("\n MDEntryTime: %s \n", MDEntryTime);
		statusMDEntryTime = 1; //true
		strcpy(field, "");
	}


	if(statusSymbol == 1 && statusMDEntryType == 1 && statusMDEntryPx == 1 && statusMDEntrySize == 1 && statusMDEntryTime == 1){
		statusSymbol = 0;
		statusMDEntryType = 0;
		statusMDEntryPx = 0;
		statusMDEntrySize = 0;
		statusMDEntryTime = 0;

		statusMsg = 1;
	}
}

int binaryToInt(long long n) {
    int dec = 0, i = 0, rem;
    while (n != 0){
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }
    return dec;
}

