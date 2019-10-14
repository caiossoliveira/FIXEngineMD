#include <stdio.h>
#include <stdlib.h>


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
	file = fopen("fix.051.snap-1message.log", "r");
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

char memory_message[200000];

void carryMessage(int msg_length){
	/*this function identify each FIX message, removing the information not necessary 
	for the pro-processing function.*/

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
			printf("%c", fix_message[i]);
	}

	printf("\n\n\n");

}

void preProcessingMDFull(int msg_length){
	//function that analyse the required tags of each type of message
	//required tag for MDFullReflesh: 911, 268, 269, 272, 273, 37017 (268, 269, 272, 273, 911, 37017)
	//dependents: 37100 if 269=3; 3939 if 269=g and 1147 and 1149 are sent; 270 when MD involves a price; 271 when MD involves a quantity;
	//dependents: 37016 if 269=2 and for equities market; 274 is the direction of tick, so it's required when reporting a trade; 
	//dependents: 342 when MDEntryType=c and SecurityTradingStatus=21; 336 is used to mark non-regular session trades. valid values: 6;
	//dependents: 1174 if MDEntryType=c and 326 is filled. valid values=101 and 102; 9325 if 269=5 or/and 286=4; 37013 if indicates previous day's closing price.
	//dependents: 37 if the bid or offer represents an order; 1003 if it's reporting a trade; 346 if it's a price-depth book entry; 290 if MDEntryType = 0 or 1;
	//dependents: 731 if MDEntryType=6; 37008 used with auction price banding; 37003 DailyAvgShared30D, for equity market only; 432 is used in BTB contracts only;
	//dependents: 37019 is used in BTB contracts only; 37023 used in BTB contracts only, if not sent, means the offer is not certified; 37024 is used in BTB contracs only;
	//dependents: 37025 is used BTB contracts only; 1140 ?; 1025 if 269=v; 31 if 269 =v; 811 if 2569=v; 711 if 269=D; 37008 is used with auction price banding;
	//dependents: 309, 305, 308, 810 is required if 37008 is present; 37018 used when 269=D and 37008 is present.

	printf("\n\nMarket Data Full Reflesh\n");

	//911, 268, 269, 272, 273, 37017
	char tagTotNumReports[4], tagNoMDEntries[4], tagMDEntryType, tagMDEntryDate[8], tagMDEntryTime[10], tag37017[10];
	int indexTagTotNumReports = 0, indexTagNoMDEntries = 0, indexTagMDEntryType = 0, indexTagMDEntryDate = 0, indexTagMDEntryTime = 0, indexTag37017 = 0;

	int aux_msg_length = msg_length;
	int i = 0;

	while(aux_msg_length > 0){

		if(memory_message[i] == '2'){ //tags 268, 269 and 273
			i++;
			aux_msg_length--;
			if(memory_message[i] == '6'){ //tag 268 and 269
				i++;
				aux_msg_length--;
				if(memory_message[i] == '8'){ //tag 268
					i++;
					aux_msg_length--;
					if(memory_message[i] == '='){ //tag268=
						i++;
						aux_msg_length--;
						while(memory_message[i] != '' && aux_msg_length > 0){
							i++;
							aux_msg_length--;
							tagNoMDEntries[indexTagNoMDEntries] = memory_message[i];
							indexTagNoMDEntries++;
						}
					}
				}
				else if(memory_message[i] == '9'){ //tag 269
					if(memory_message[i] == '='){
						i++;
						aux_msg_length--;
						while(memory_message[i] != '' && aux_msg_length > 0){
							i++;
							aux_msg_length--;
							tagMDEntryType = memory_message[i];
							indexTagMDEntryType++;
						}
					}
				}
			}
			else if(memory_message[i] == '7'){ //tag 273
				i++;
				aux_msg_length--;
				if(memory_message[i] == '2'){ //tag 272
					i++;
					aux_msg_length--;
					if(memory_message[i] == '='){
						i++;
						aux_msg_length--;
						while(memory_message[i] != '' && aux_msg_length > 0){
							i++;
							aux_msg_length--;
							tagMDEntryDate[indexTagMDEntryDate] = memory_message[i];
							indexTagMDEntryDate++;
						}
					}
					
				}
				if(memory_message[i] == '3'){ //tag 273
					i++;
					aux_msg_length--;
					if(memory_message[i] == '2'){ //tag 272
						i++;
						aux_msg_length--;
						if(memory_message[i] = '='){
							i++;
							aux_msg_length--;
							while(memory_message[i] != '' && aux_msg_length > 0){
								i++;
								aux_msg_length--;
								tagMDEntryDate[indexTagMDEntryDate] = memory_message[i];
								indexTagMDEntryDate++;
							}
						}	
					}
				}
			}
		}
		else if(memory_message[i] == '9'){ //tag 911
			i++;
			aux_msg_length--;
			i++;
			aux_msg_length--;
			if(memory_message[i] == '1'){ //tag 911
				i++;
				aux_msg_length--;
				if(memory_message[i] == '1'){ //tag 911
					i++;
					aux_msg_length--;
					if(memory_message[i] == '='){ //tag911=
						i++;
						aux_msg_length--;
						while(memory_message[i] != '' && aux_msg_length > 0){
							i++;
							aux_msg_length--;
							tagTotNumReports[indexTagTotNumReports] = memory_message[i];
							indexTagTotNumReports++;
						}
					}
				}
			}
		}
		else{
			i++;
			aux_msg_length--;
		}
	}

	printf("\n NoMDEntries: ");
	for(int i=0; i<indexTagNoMDEntries; i++){
		printf("%s", tagNoMDEntries);
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