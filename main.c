#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

static int file_set=0;
static int number_set=0;

static int file_name_len=0;
static char* file_name;

void print_help(){
	printf("Usage ./bf [options]\n");
	printf("OPTIONS\n");
	printf("\tf<file>\t--file <file>\tSpecify the file to load the program from\n");
	printf("\tn\t--number\tSpecify that numbers should be used for I/O (instead of chars)\n");
	printf("\n");
	exit(1);
}

struct string_len{
	char* string;
	int len;
};

void load_program(struct string_len* str){
	FILE* stream;
	if(file_set){
		//load from file 
		stream = fopen(file_name,"r");
		free(file_name);
	} else {
		//read from stdin
		stream = stdin;
	}
	//read data

	const int BUF_LEN = 2048;
	char buf[BUF_LEN];
	int pos = 0;
	while(fgets(buf,BUF_LEN,stream)!=NULL){
		int curpos = ftell(stream);
		int num_read = curpos - pos;
		if(str->len == 0){
			str->len+=num_read;
			str->string = (char*)malloc(sizeof(char)*(str->len));
			strncpy(str->string,buf,str->len);
		} else {
			str->len+=num_read;
			str->string = (char*)realloc(str->string,sizeof(char)*(str->len));
			strncat(str->string,buf,str->len);
		}
		pos = curpos;
	}
	//if file, close stream
	if(file_set){
		fclose(stream);
	}
}

int find_right(int start, char* tape, const int tape_len){
	int i,count=0;
	for(i=start;i<tape_len;++i){
		if(tape[i]=='['){
			count++;
		}
		if(tape[i]==']'){
			if(count==0){
				return i;
			}
			count--;
		}
	}
	//reached end without finding 
	return -1;
}

int find_left(int start, char* tape, const int tape_len){
	int i,count=0;
	for(i=start;i>=0;--i){
		if(tape[i]==']'){
			count++;
		}
		if(tape[i]=='['){
			if(count==0){
				return i;
			}
			count--;
		}
	}
	//reached 0 without finding 
	return -1;
}

char get_input(){
	char input;
	if(number_set){
		int iinput;
		scanf("%d",&iinput);
		input=iinput;
	} else {
		scanf("%c",&input);
	}
	return input;
}

void put_output(char out){
	if(number_set){
		printf("%d\n",out);
	} else {
		printf("%c",out);
	}
}

void run_program(struct string_len* program, char* tape, const int tape_len){
	int pointer,i,ignore=0;
	for(i=0;i<tape_len;++i){
		tape[i]=0;
	}
	for(i=0;i<program->len;++i){
		if(ignore && program->string[i]!='\n'){
			continue;
		}
		switch(program->string[i]){
			case('+'):
				tape[pointer]++;
				break;
			case('-'):
				tape[pointer]--;
				break;
			case('>'):
				pointer++;
				break;
			case('<'):
				pointer--;
				break;
			case('['):
				if(tape[pointer]==0){
					i=find_right(i,tape,tape_len);
				}
				break;
			case(']'):
				if(tape[pointer]!=0){
					i=find_left(i,tape,tape_len);
				}
				break;
			case(','):
				tape[pointer]=get_input();
				break;
			case('.'):
				put_output(tape[pointer]);
				break;
			case('\'')://' starts a line comment
				//ignore everything till newline
				ignore=1;
				break;	
			case('\n'):
				ignore=0;
				break;
			default:
				break;
				//do nothing for unrecognized characters	
		}
	}
}

int main(int argc, char* const argv[]){
	static struct option long_options[] =
	{
		{"file",required_argument,&file_set,2},
		{"numbers",no_argument,&number_set,1},
		{"help",no_argument,0,0},
		{0,0,0,0}
	};

	int option_index=0,c=0;
	while(1){
		c = getopt_long(argc,argv,"f:nh", long_options,&option_index);
		if(c==-1){
			break;
		}
		switch(c){
			case(0):
				if(option_index==0){
					if(file_set == 2){
						file_name_len = strlen(optarg);
						file_name = (char*)malloc(sizeof(char)*file_name_len);
						strncpy(file_name,optarg,file_name_len);
					}
				}
				if(option_index==2){
					print_help();
				}
				if(long_options[option_index].flag!=0){
					break;
				}
				//no arguments without flags
				break;
			case('n'):
				number_set = 1;
				break;
			case('f'):
				if(!file_set){
					file_set=1;
					file_name_len = strlen(optarg);
					file_name = (char*)malloc(sizeof(char)*file_name_len);
					strncpy(file_name,optarg,file_name_len);
				}
				break;
			case('h'):
				print_help();
				break;
			case('?'):
				break;
			default:
				abort();
		}
	}

	if(file_set){
//		printf("File = %s\n",file_name);
	}
	if(number_set){
//		printf("Number I/O is on\n");
	}

	struct string_len program;
	program.len=0;
	load_program(&program);
	if(program.len < 1){
		fprintf(stderr,"Program file (%s) is empty, quitting\n",file_name);
		exit(1);
	}

	const int TAPE_LEN=256;
	char tape[TAPE_LEN];
	run_program(&program,tape,TAPE_LEN);

	free(file_name);
	return 0;
}
