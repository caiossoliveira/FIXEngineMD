#include <stdio.h>
#include <stdlib.h>

char fix_message[5000]; //variable shared by messageIdentifier and Pre-processing
char message[5000];
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
	file = fopen("fix.051.full_reflesh-simplified.log", "r");
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

	int i;
	int msg_index = msg_length;
	int interval = 0;
	next_message_length = 0;

	while(message[msg_index] != ''){
		next_message_length++;
		msg_index--;
	}

	for(i = 0; i < msg_length - next_message_length + 1; i++){ //+ 1 to take the SOH character
		fix_message[i] = message[i];
	}

	interval = msg_length - next_message_length + 1;

	for(i = 0; i < next_message_length + 1; i++){
		message[i] = message[interval];
		interval++;
	}

	printf("=======================================================\n");
	for(int i=0; i<msg_index+1; i++){
		if(fix_message[i] == '')
			fix_message[i] = '|';
	}

	preProcessing(); //send the message to the pre-processing function

	for(int i=0; i < interval; i++){ //to remove trash of the 35 tag
		if(fix_message[i] != '\0'){
			printf("%c", fix_message[i]);	
		}
		
	}
	printf("\n");
	printf("========================================================\n");
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