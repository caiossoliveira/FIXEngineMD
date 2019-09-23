#include <stdio.h>
#include <stdlib.h>

char message[2500]; //variable shared by messageIdentifier and Pre-processing
char message_type = ' '; //variable shared by messageIdentifier and Pre-processing. Used to know what type of message the array is.

FILE* file; //variable shared by openFile and readNextCharacter

void openFile();
char readNextCharacter();
void clearMessage(int m);

void messageIdentifier();
void preProcessing();
void marketDataHandler();

void preProcessingMDFull();	
void preProcessingMDIncr();	

int main(){
	openFile();
	messageIdentifier();
	preProcessing();
}

void openFile(){
	file = fopen("fix.051.mixed-resumido.log", "r");
}

char readNextCharacter(){
	char character = ' ';

	if(fread(&character, 1, 1, file) > 0){
		return character;	
	}
	else{
		return '\0'; //return NULL
	}	
}

void clearMessage(int m){
	/*this function intents to clean the message, removing the information not necessary 
	for the pro-processing function. This way, the function removes the time*/

	while(message[m] != '' && m != 0){
		message[m] = '\0';
		m--;
	}
}

void messageIdentifier(){
	/*this functions detects each FIX message
	  It is able to detect each 35 tag and this way to mount the message
	*/

	int status_tag_35 = 0; //false
	int status_eof = 0; //false


	for(int i=0; i<2500; i++){
		message[i] = ' ';
	}

	while(!status_eof){ //
		int m = 0;
		char character = ' ';

		status_tag_35 = 0; //false

		while(status_tag_35 == 0 && status_eof == 0){ //false //the status 35 has to be false and the end of file too
			character = readNextCharacter();
			if(character == '\0'){ //if character equals NULL
				status_tag_35 = 1; //true
				status_eof = 1; //true
			}
			if(character == '3'){
				message[m] = character;
				m++;
				character = readNextCharacter();
				if(character == '5'){
					message[m] = character;
					m++;
					character = readNextCharacter();
					if(character == '='){
						status_tag_35 = 1; //true //If the 35 tag is detected the status var is changed. This way the while funcion will be false. This means that the message is complete
						message_type = readNextCharacter(); //read the type of message
					}
				}
				else{
					message[m] = character;
					m++;
				}
			}
			else{
				message[m] = character;
				m++;
			}
		}
		if(m > 0){
			clearMessage(m);
			printf("=======================================================\n");
			for(int i=0; i<m; i++){
				if(message[i] == '')
					message[i] = '|';
			}

			preProcessing(); //send the message to the pre-processing function

			for(int i=1; i<m; i++){ //to remove trash of the 35 tag
				if(message[i] != '\0'){
					printf("%c", message[i]);	
				}
				
			}
			printf("\n");
			printf("========================================================\n");
		}
	}
}

void preProcessing(){

	switch(message_type){
		case 'W':
			preProcessingMDFull();
		break;

		case 'X':
			preProcessingMDIncr();
		break;

		default:
			printf("Message type do not identified (%c)", message_type);
	}
}

void preProcessingMDFull(){
	printf("Market Data Full Reflesh \n\n");
}	

void preProcessingMDIncr(){
	printf("Market Data Incremental Reflesh \n\n");
}

void marketDataHandler(){
	int n = 0;
	char ctipo_de_oferta;
	int tipo_de_oferta;
	char cvalor_ativo[10];
	float fvalor_ativo;
	float highest_bid = 0.0;
	float lowest_offer = 9999.9;
}