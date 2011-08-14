/*
The following design decisions have been made : 

1. For any escape sequence that is not recognized, for example \a, the backslash has been dropped and simply 'a' is printed.

2. Operators such as +/- have been considered separate from integers and floats themselves. Therefore something like -123 would be considered as the token '-' and the integer '123' separately.

3. For hex sequences inside strings, only two characters after the \x are considered while computing the hex value, the following characters are not treated as part of the hex sequence. Also a hex sequence which is out of range generates a corresponding message.
*/
#define BUFFLEN (1024)
const char* yyfilename="";
char filename[100];
int yylinenum=0;

//Process Identifiers
int identifiers()
{
        yylval.cVal = (char *)malloc(yyleng+1);
	strcpy(yylval.cVal,yytext);
        yylval.cVal[yyleng] = '\0';
	return TOK_IDENT;
}

//Function to print a warning/error message
void print_error_message(const char *message)
{
	printf("%s:%d : %s\n",yyfilename,yylinenum,message);
}

//Checks whether a character is a valid hex character
int ishex(char c)
{
	if((c>='a' && c<='f') || (c>='A' && c<='F') || (c>='0' && c<='9'))
		return 1;
	return 0;
}

//Checks whether the characters following \x form a valid hex sequence
int check_hex(char *text, int index)
{
	if(index+2>=strlen(text))
		return 0;
	if(ishex(text[index+1]) && ishex(text[index+2]))
		return 1;
	return 0;
}

//Compute the value of the \x hex sequence
char process_hex(char c1, char c2)
{
	int ret=0;
	if(c1>='a' && c1<='f')
		ret=10+c1-'a';
	else if(c1>='A' && c1<='F')
		ret=10+c1-'A';
	else
		ret=c1-'0';

	if(c2>='a' && c2<='f')
		ret=ret*16+10+c2-'a';
	else if(c2>='A' && c2<='F')
		ret=ret*16+10+c2-'A';
	else
		ret=ret*16+c2-'0';
	if(ret>127)
		print_error_message("hex escape sequence out of range");
	return ret;
}

//Function that deals with all escape sequences and processes them accordingly 
int escape(char *answer,int *in,char *original_text,int *len)
{
	int index=*in;
	index++;
	char c=original_text[index];
	switch(c)
	{
		/*case 'a' : answer[(*len)++]='\a';
			   (*in)++;
			   break;
		case 'b' : answer[(*len)++]='\b';
			   (*in)++;
			   break;
		case 'f' : answer[(*len)++]='\f';
			   (*in)++;
			   break;*/
		case 'n' : answer[(*len)++]='\n';
			   (*in)++;
			   break;
		case 'r' : answer[(*len)++]='\r';
			   (*in)++;
			   break;
		case 't' : answer[(*len)++]='\t';
			   (*in)++;
			   break;
		/*case 'v' : answer[(*len)++]='\v';
			   (*in)++;
			   break;*/
		case '\'' : answer[(*len)++]='\'';
			   (*in)++;
			   break;
		case '\"' : answer[(*len)++]='\"';
			   (*in)++;
			   break;
		case '\?' : answer[(*len)++]='\?';
			   (*in)++;
			   break;
		case '\\' : answer[(*len)++]='\\';
			   (*in)++;
			   break;
		case 'x' : if(!check_hex(original_text,index))
			   {
				   print_error_message("\\x used incorrectly");
				   return 0;
			   }
			   else
			   {
				   answer[(*len)++]=process_hex(original_text[index+1],original_text[index+2]);
				   (*in)+=3;
			   }
			   break;
		default :   //print_error_message("Invalid Escape Sequence");
			    //return 0;
			    answer[(*len)++]=c;
			    (*in)++;
			    break;
	}
	return 1;
}

void error_string() 
{
	print_error_message("Strings cannot span multiple lines");
}

//String processing function
int process_string(char *answer, char *original_text)
{
	int slen=strlen(original_text);
	int len=0;
	int i;
	for(i=0;i<slen;i++)
		if(original_text[i]!='\\')
			answer[len++]=original_text[i];
		else
			if(!escape(answer,&i,original_text,&len))
				return 0;
	answer[len]='\0';
	return 1;
}

int strings()
{
	char *answer=(char *)malloc(sizeof(char)*BUFFLEN);
	if(!process_string(answer,yytext))
		return 0;
	answer++;
	answer[strlen(answer)-1]='\0';
	yylval.cVal=answer;
	return 1;
}


//Process # style messages generated by cpp
void comments()
{
	sscanf(yytext,"# %d %s",&yylinenum,filename);
	filename[strlen(filename)-1]='\0';
	yyfilename=filename+1;
}

//Process integers
int integer()
{
	sscanf(yytext,"%d",&yylval.uVal);
	return TOK_UINTNUM;
}

//Process hex integers
int hex_integer()
{
	sscanf(yytext,"%x",&yylval.uVal);
	return TOK_UINTNUM;
}

//Process floats
int floats()
{
	sscanf(yytext,"%lf",&yylval.dVal);
	return TOK_DOUBLENUM;
}