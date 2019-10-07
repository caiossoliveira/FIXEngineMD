#include <stdio.h>
#include <stdlib.h>

char fix_message[200000]; //variable shared by messageIdentifier and Pre-processing
char message[200000];
char message_type = ' '; //variable shared by messageIdentifier and Pre-processing. Used to know what type of message the array is.
int count = 0;
int next_message_length = 0;

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
	//preProcessing();
}

void openFile(){
	//file = fopen("fix.051.full_reflesh-simplified.log", "r");
	file = fopen("fix.051.snap-resume.log", "r");
	//file = fopen("fix.051.snap.log", "r");
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

void carryMessage(int msg_length){
	/*this function identify each FIX message, removing the information not necessary 
	for the pro-processing function.*/
	//printf("\n\n ================================================================================ \n");
	//printf("\n msg_length: %d\n", msg_length);

	int i;
	int msg_index = msg_length;
	int interval = 0;
	int aux_interval = 0;
	int aux_msg_index;
	next_message_length = 0;

	/*printf("\n Whole message: \n");
	for(i=0; i<msg_length; i++){
		if(message[i] == '')
			printf("|");
		else
			printf("%c", message[i]);
	}*/

	//if the msg has '\n', uses -1
	while(message[msg_index-1] != ''){ //count the index n of the message until find the SOH. This is the length of the next fix message //-1 bcs sometimes the msg_index get the SOH 
		next_message_length++;
		msg_index--;
	}

	//printf("\n next_message_length: %d\n", next_message_length);

	for(i = 0; i < msg_length - next_message_length + 1; i++){ //+ 1 to get the SOH character
		fix_message[i] = message[i]; //fix_message receives the message without the next one
	}

	interval = msg_length - next_message_length + 1;
	aux_interval = interval; //this var is used to increment in the next for without changing the oficial interval variable. This way, it's not bug the next function functioning.

	//printf("\n interval: %d\n", interval);

	for(i = 0; i < next_message_length; i++){
		message[i] = message[aux_interval-1];
		aux_interval++;
	}

	/*for(int i=0; i<msg_index+1; i++){ //print until the msg_index.
		if(fix_message[i] == '')
			fix_message[i] = '|';
	}*/

	preProcessing(); //send the message to the pre-processing function

	//printf("\n fix message: \n");
	for(int i=0; i < interval; i++){ //to remove trash of the 35 tag
		if(fix_message[i] == '')
			printf("|");
		else
			printf("%c", fix_message[i]);	
		
		
	}

	printf("\n ================================================================================ \n\n");
}

void messageIdentifier(){
	int status_tag_35 = 0; //false
	int status_eof = 0; //false
	int msg_index = 0;
	int msg_length = 0;

	for(int i=0; i<5000; i++){
		message[i] = ' ';
	}

	while(!status_eof){ //
		
		char character = ' ';
		msg_index = next_message_length;

		while(status_tag_35 < 2 && status_eof == 0){ //false //the status 35 has to be false and the end of file too
			character = readNextCharacter();
			if(character == '\0'){ //if character equals NULL
				status_eof = 1; //true
			}
			else if(character == '3'){
				message[msg_index] = character;
				msg_index++;
				character = readNextCharacter();
				if(character == '5'){
					message[msg_index] = character;
					msg_index++;
					character = readNextCharacter();
					if(character == '='){
						message[msg_index] = character;
						msg_index++;
						status_tag_35++; //true //If the 35 tag is detected the status var is changed. This way the while funcion will be false. This means that the message is complete
						message_type = readNextCharacter(); //read the type of message
						message[msg_index] = message_type;
						msg_index++;
					}
					else{
						message[msg_index] = character;
						msg_index++;	
					}
				}
				else{
					message[msg_index] = character;
					msg_index++;
				}
			}
			else{
				message[msg_index] = character;
				msg_index++;
			}
		}
		status_tag_35 = 1;
		if(msg_index > 0){
			msg_length = msg_index;
			carryMessage(msg_length);
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
	printf("\n\nMarket Data Full Reflesh\n");
}	

void preProcessingMDIncr(){
	printf("\n\nMarket Data Incremental Reflesh\n");
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