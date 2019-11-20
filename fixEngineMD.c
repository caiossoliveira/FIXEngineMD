#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//MsgIdentifier functions
void openFile();
char readNextCharacter();
void messageIdentifier();
void carryMessage(int msg_length);

//Pre-processing functions
void preProcessing(int msg_length, char message_type);
void preProcessingMDFull();	
void preProcessingMDIncr();	

void marketDataHandler();

int main(){
	openFile();
	messageIdentifier();
}

/*BEGIN OF MESSAGE IDENTIFIER BLOCK*/

FILE* file; //variable shared by openFile and readNextCharacter

void openFile(){
	//file = fopen("fix.051.full_reflesh-simplified.log", "r");
	//file = fopen("fix.051.snap-resumido.log", "r");
	//file = fopen("fix.051.snap.log", "r");
	file = fopen("fix.051.snapfull-1message.log", "r");
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

char fix_message[500000]; //variable shared by messageIdentifier and carryMessage
char message[500000];
int next_message_length = 0; //variable shared by messageIdentifier and carryMessage

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
						message[msg_index] = readNextCharacter();
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

char memory_message[200000]; //memory shared between the msgIdentifier and preProcessing block

void carryMessage(int msg_length){
	/*this function identify each FIX message, removing the information not necessary 
	for the pre-processing function.*/

	int i;
	int msg_index = msg_length;
	int interval = 0;
	int aux_interval = 0;
	int aux_msg_index;
	next_message_length = 0;

	//if the msg has '\n', uses -1
	while(message[msg_index-1] != ''){ //count the index n of the message until find the SOH. This is the length of the next fix message //-1 bcs sometimes the msg_index get the SOH 
		next_message_length++;
		msg_index--;
	}

	for(i = 0; i < msg_length - next_message_length + 1; i++){ //+ 1 to get the SOH character
		fix_message[i] = message[i]; //fix_message receives the message without the next one
	}

	interval = msg_length - next_message_length + 1;
	aux_interval = interval; //this var is used to increment in the next for without changing the oficial interval variable. This way, it's not bug the next function functioning.

	for(i = 0; i < next_message_length; i++){
		message[i] = message[aux_interval-1];
		aux_interval++;
	}

	for(int i=0; i < interval; i++){ //alocate only the fix message to the buffer for the pre-processing
		memory_message[i] = fix_message[i];		
	}

	char message_type = ' ';
	for(int i=0; i < interval; i++){ //alocate only the fix message to the buffer for the pre-processing
		if(memory_message[i] == '3'){
			if(memory_message[i+1] == '5'){
				if(memory_message[i+2] == '='){
					message_type = memory_message[i+3];
					break;	
				}	
			}
		}
	}
	preProcessing(interval, message_type); //send the message to the pre-processing function
}

/*END OF MESSAGE IDENTIFIER BLOCK*/




/* BEGIN OF PRE-PROCESSING BLOCK */
void preProcessing(int msg_length, char message_type){

	switch(message_type){
		case 'W':
			preProcessingMDFull(msg_length);
		break;

		case 'X':
			preProcessingMDIncr();
		break;

		default:
			printf("Unidentified MessageType (%c)", message_type);
	}

	for(int i=0; i < msg_length; i++){ 
		if(memory_message[i] == '')
			printf("|");
		else
			printf("%c", memory_message[i]);
	}

	printf("\n\n\n");

}

void preProcessingMDFull(int msg_length){
	//function that analyse the required tags of each type of message
	//required tag for MDFullReflesh: 269, 270, 271

	printf("\n\nMarket Data Full Reflesh\n");


	int aux_msg_length = msg_length;
	int i = 0;
	int status_msg = 1; //true - valid message
		
	

	if(strstr(memory_message, "269=") != NULL && strstr(memory_message, "270=") != NULL && strstr(memory_message, "271=") != NULL){
		printf("\n\nThe message is valid \n");
	}
	else{
		printf("\n\nThe message is not valid \n");	
	}

	printf("\n");
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