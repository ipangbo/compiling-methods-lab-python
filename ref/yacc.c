#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAX_LINE_LENGTH 1024

/* 以下定义行缓冲区 */
char linebuffer[MAX_LINE_LENGTH] = {0};  /* 行缓冲 */
int  process_pos = 0;        /* 待处理字符位置 */
int  real_length = 0;        /* 当前程序行的长度 */
int  lineNO_in_file = 0;     /* 当前程序行在文件中的行号 */


/***********************************************
   函数名：TrimString
   功能：  去除行首和行尾的 空格 回车 换行符等脏字符
   参数：  无
   返回值：去除脏字符行长度
************************************************/
int TrimString( )
{
	int i = 0,j = 0,k = 0;
	int len = (int)strlen(linebuffer);

	while( i < MAX_LINE_LENGTH && linebuffer[i] == 32 )
	   i++; /* 去除行首空格*/
    j = len - 1;

	while(j >= 0 && j < MAX_LINE_LENGTH && ( linebuffer[j] == 32 || linebuffer[j] == 0x0d || linebuffer[j] == 0x0a  || linebuffer[j] == '\t') )
	{
		linebuffer[j] = 0; /*去除行尾的空格和回车换行 TAB符*/
		j--;
	}
    for(k = i;k <=j+1;k++)
      linebuffer[k-i] = linebuffer[k];

    real_length = (int)strlen(linebuffer);

	return real_length;
}

/***********************************************
   函数名：GetString
   功能：取得一行字符串
   参数：FILE* srcfile
   返回值：0 失败
           >0  读取的行长度
************************************************/
int GetString(FILE* srcfile)
{
	real_length = 0;        /* 当前程序行的长度置0 */
	process_pos = 0;        /* 待处理字符位置置0 */
	memset(linebuffer, 0 ,MAX_LINE_LENGTH*sizeof(char)); /* 清除行缓冲区原有内容*/

    if( fgets(linebuffer ,MAX_LINE_LENGTH-1 ,srcfile ) != NULL)
    {
      lineNO_in_file++;
	  real_length = TrimString( ); /*(int)strlen(linebuffer);*/

	  while( real_length == 0 && !feof(srcfile))  /*去除空行*/
	  {
	     if( fgets(linebuffer,MAX_LINE_LENGTH-1 ,srcfile) == NULL)
			 break;
		 else
		 {
		     real_length = TrimString( ); /*(int)strlen(linebuffer);*/
			 lineNO_in_file++;
		 }
	  }
	}
	return real_length;
}

/***********************************************
   函数名：GetCurrentLineNo
   功能：取得当前程序行号
   参数：无
   返回值：行号
************************************************/
int GetCurrentLineNo()
{
	return lineNO_in_file;
}

/***********************************************
   函数名：GetCurrntPos
   功能：取得当前字符位置
   参数：无
   返回值：当前字符位置
************************************************/
int GetCurrntPos()
{
	return process_pos;
}

/***********************************************
   函数名：GetNextChar
   功能：从行缓冲区取1字符函数
   参数：char *ch 取得的字符地址
   返回值：0 行结束
           1 成功
************************************************/
int GetNextChar(char *ch)
{
 /* while(process_pos < real_length && linebuffer[process_pos] == 32)
     process_pos++;*/

  if(process_pos < real_length)
  {
	  *ch = linebuffer[process_pos];
      process_pos++;
	  return 1;
  }
  else
    return 0;
}

/***********************************************
   函数名：BackChar
   功能：行缓冲读写位置回退1字符
   参数：无
   返回值：无
************************************************/
void  BackChar()
{
    if(process_pos > 0)
		process_pos --;
}

/*******************************************************
种别码表
单词		种别码	单词		种别码	单词	种别码
break		1		void		17		!=		33
case		2		while		18		>=		34
char		3		标识符		19		<=		35
continue	4		整常数		20		++		36
default		5		实常数		21		--		37
do			6		字符常数	22		(		38
double		7		字符串		23		)		39
else		8		+			24		[		40
float		9		-			25		]		41
for			10		*			26		;		42
if			11		/			27		,		43
int			12		%			28		.		44
return		13		=			29		&		45
struct		14		>			30		'		46
switch		15		<			31		"		47
unsigned	16		==			32		_		48
                                        {       49
                                        }       50
										:       51
********************************************************/

typedef struct defTOKEN
{
  int typecode;  /*种别码*/
  int entry;     /* 符号表入口 */

}TOKEN; /* TOKEN字 */

typedef struct defNAME
{
  int pos_in_wordlist; /* 当前字在字串列表中的位置 */
  int length;          /* 当前字在字串列表中的长度 */
}NAME;  /* NAME为符号表中的一项 */

typedef struct defSYMTAB
{
  NAME name; /* 名字 */
  int type;  /* 数据类型 */
  int kind;  /* 数据种类 */
  int val;   /* 占位，词法分析时无意义 */
  int addr;  /* 占位，词法分析时无意义 */
}SYMTAB; /* 符号表中一行 */


typedef struct defErrorInfo
{
  int errorcode;  /* 错误码 */
  char errorinfo[64]; /* 出错信息说明*/
  int line;   /* 出错行号  */
  int pos;  /* 错误位置  */

}ERRORINFO; /* 出错信息表中一行 */

#define SYMTAB_LENGTH 128
#define WORDLIST_LENGTH 1024
#define TOKENLIST_LENGTH 256
#define ERRORINFO_LENGTH 256

typedef struct defALLRESULT
{
  SYMTAB* pSymtab;        /* 符号表首地址*/
  int nSymTabTotalLength; /* 符号表总长度*/
  int nSymTabUsedLength;  /* 符号表实际长度,也即Tail*/

  char* pWordList;        /* 字符列表首地址*/
  int nWordListTotalLength; /* 字符列表总长度*/
  int nWordListUsedLength;  /* 字符列表实际长度，也即Tail */

  TOKEN* pTokenList;         /* TOKEN字列表首地址*/
  int nTokenListTotalLength; /* TOKEN字列表总长度*/
  int nTokenListUsedLength;  /* TOKEN字列表实际长度，也即Tail */

  ERRORINFO* pErrorInfoList;         /* 错误信息列表首地址*/
  int nErrorInfoListTotalLength; /* 错误信息列表总长度*/
  int nErrorInfoListUsedLength;  /* 错误信息列表实际长度，也即Tail */

}ALLRESULT;

ALLRESULT ResultData; /* 结果数据,包括 符号表、TOKEN字列表 错误信息列表  */

/***********************************************
 函数：DeleteResultData
 功能：回收结果数据空间
 参数：无
 返回值：无
***********************************************/
void DeleteResultData()
{
	if(ResultData.pSymtab!=NULL)
	   free(ResultData.pSymtab);

	if(ResultData.pWordList!=NULL)
	   free(ResultData.pWordList);

	if(ResultData.pTokenList!=NULL)
	   free(ResultData.pTokenList);

	if(ResultData.pErrorInfoList!=NULL)
	   free(ResultData.pErrorInfoList);

}


/***********************************************
 函数：InitResultData
 功能：分配结果数据空间并初始化结果数据
 参数：无
 返回值： 0 失败
          1 成功
***********************************************/
int InitResultData()
{
	/* 初始化符号表 */
	ResultData.nSymTabTotalLength = SYMTAB_LENGTH;
	ResultData.nSymTabUsedLength = 0;
	ResultData.pSymtab = (SYMTAB*)malloc(sizeof(SYMTAB)*SYMTAB_LENGTH);
	if(ResultData.pSymtab == NULL)
		return 0;
    memset(ResultData.pSymtab, 0 ,sizeof(SYMTAB)*SYMTAB_LENGTH );

	/* 初始化符号表的字符表 */
	ResultData.nWordListTotalLength = WORDLIST_LENGTH;
	ResultData.nWordListUsedLength = 0;
	ResultData.pWordList = (char*)malloc(sizeof(char)*WORDLIST_LENGTH);
	if(ResultData.pWordList == NULL)
	{
      DeleteResultData();
	  return 0;
	}
    memset(ResultData.pWordList, 0 ,sizeof(char)*WORDLIST_LENGTH );

	/* 初始化TOKEN字列表 */
	ResultData.nTokenListTotalLength = TOKENLIST_LENGTH;
	ResultData.nTokenListUsedLength = 0;
	ResultData.pTokenList = (TOKEN*)malloc(sizeof(TOKEN)*TOKENLIST_LENGTH);
	if(ResultData.pTokenList == NULL)
	{
      DeleteResultData();
	  return 0;
	}
    memset(ResultData.pTokenList, 0 ,sizeof(TOKEN)*TOKENLIST_LENGTH  );

    /*初始化出错信息列表*/
	ResultData.nErrorInfoListTotalLength = ERRORINFO_LENGTH;
	ResultData.nErrorInfoListUsedLength = 0;
	ResultData.pErrorInfoList = (ERRORINFO*)malloc(sizeof(ERRORINFO)*ERRORINFO_LENGTH);
	if(ResultData.pErrorInfoList == NULL)
	{
      DeleteResultData();
	  return 0;
	}
    memset(ResultData.pErrorInfoList, 0 ,sizeof(ERRORINFO)*ERRORINFO_LENGTH  );

   return 1;
}

/***********************************************
   函数名：SaveTokenToFile
   功能：保存TOKEN列表到文件
   参数：FILE* tokenfile
   返回值：0 失败
           1 成功
************************************************/
int SaveTokenToFile(FILE* tokenfile)
{
    int i = 0;
	if(ResultData.pTokenList != NULL)
	{
	   for(i = 0; i < ResultData.nTokenListUsedLength; i++)
	   {
		   if(fprintf(tokenfile, "%d %d ",ResultData.pTokenList[i].typecode, ResultData.pTokenList[i].entry ) < 0)
			   return 0;
	   }
	   return 1;
	}

	return  0;
}

/***********************************************
   函数名：SaveErrorInfoToFile
   功能：保存ERRORINFO列表到文件
   参数：FILE* errorinfofile
   返回值：0 失败
           1 成功
************************************************/
int SaveErrorInfoToFile(FILE* errorinfofile)
{
    int i = 0;
	if(ResultData.pErrorInfoList != NULL)
	{
	   for(i = 0; i < ResultData.nErrorInfoListUsedLength; i++)
	   {
		   if(fprintf(errorinfofile, "ERRORCODE:%d  LINES:%d  POSITION:%d  ERRORINFO:%s\n",ResultData.pErrorInfoList[i].errorcode ,
			             ResultData.pErrorInfoList[i].line , ResultData.pErrorInfoList[i].pos, ResultData.pErrorInfoList[i].errorinfo) < 0)
			   return 0;
	   }
	   return 1;
	}

	return  0;
}

/***********************************************
   函数名：SaveSymTabToFile
   功能：保存符号列表到文件
   参数：FILE* symtabfile
   返回值：0 失败
           1 成功
************************************************/
int SaveSymTabToFile(FILE* symtabfile)
{
    int i = 0;

	if(ResultData.pSymtab != NULL && ResultData.nSymTabUsedLength > 0)
	{
	   for(i = 0; i < ResultData.nSymTabUsedLength; i++)
	   {
		   if(fprintf(symtabfile, "(%d %d %d %d) ",ResultData.pSymtab[i].name.pos_in_wordlist, ResultData.pSymtab[i].name.length,
			   ResultData.pSymtab[i].kind , 0 ) < 0) /*ResultData.pSymtab[i].type*/
			 return 0;
	   }
	}
    fputs("\r\n", symtabfile);
	if(ResultData.pWordList != NULL && ResultData.nWordListUsedLength > 0)
	{
	   for(i = 0; i < ResultData.nWordListUsedLength; i++)
	   {
		   if(ResultData.pWordList[i] == 0)
              ResultData.pWordList[i] = 32;
	   }

	   if(fputs(ResultData.pWordList, symtabfile) == EOF)
		   return 0;

	   return 1;
	}

	return  0;
}

/***********************************************
   函数名：AppendToken
   功能：添加TOKEN字
   参数：int kind    种类码
         int pos     在关键字表中的位置
   返回值：>=0 在TOKEN列表中的位置
            -1 失败
************************************************/
int AppendToken(int kind , int pos )
{

	TOKEN* pTokenList = NULL;
    int res = 0 ;
    /* TOKEN列表已满,则增加 TOKENLIST_LENGTH 长度的空间 */
	if(ResultData.nTokenListTotalLength <= ResultData.nTokenListUsedLength )
    {
        pTokenList = (TOKEN*)malloc(sizeof(TOKEN)*(ResultData.nTokenListTotalLength+TOKENLIST_LENGTH) );
	    if(pTokenList == NULL)
		   return -1;
		memcpy( pTokenList ,ResultData.pTokenList, sizeof(TOKEN)*ResultData.nTokenListTotalLength);
        free(ResultData.pTokenList);
		ResultData.pTokenList = pTokenList;
		ResultData.nTokenListTotalLength += TOKENLIST_LENGTH;
    }

	/* 将TOKEN 添加到TOKEN列表*/
	ResultData.pTokenList[ResultData.nTokenListUsedLength].typecode = kind;
	ResultData.pTokenList[ResultData.nTokenListUsedLength].entry = pos;

	res = ResultData.nTokenListUsedLength ++;

    return  res;
}

/***********************************************
   函数名：AppendErrorInfo
   功能：添加TOKEN字
   参数：int errorcode    错误码
         int line         出错程序行号
         int pos;         出错位置
   返回值：>=0 在TOKEN列表中的位置
            -1 失败
************************************************/
int AppendErrorInfo(int errorcode ,char* errorinfo,  int line, int pos )
{
	ERRORINFO* pErrorInfoList = NULL;
    int res = 0 ;
    /* ERRORINFO列表已满,则增加 ERRORINFO_LENGTH 长度的空间 */
	if(ResultData.nErrorInfoListTotalLength <= ResultData.nErrorInfoListUsedLength )
    {
		pErrorInfoList = (ERRORINFO*)malloc(sizeof(ERRORINFO)*(ResultData.nErrorInfoListTotalLength+ERRORINFO_LENGTH) );
	    if(pErrorInfoList == NULL)
		   return -1;
		memcpy( pErrorInfoList ,ResultData.pErrorInfoList, sizeof(ERRORINFO)*ResultData.nErrorInfoListTotalLength);
		free(ResultData.pErrorInfoList);
		ResultData.pErrorInfoList = pErrorInfoList;
		ResultData.nErrorInfoListTotalLength += ERRORINFO_LENGTH;
    }

	/* 将ERRORINFO 添加到ERRORINFO列表*/
	ResultData.pErrorInfoList[ResultData.nErrorInfoListUsedLength].errorcode = errorcode;
	ResultData.pErrorInfoList[ResultData.nErrorInfoListUsedLength].line = line;
	ResultData.pErrorInfoList[ResultData.nErrorInfoListUsedLength].pos = pos;
	strcpy(ResultData.pErrorInfoList[ResultData.nErrorInfoListUsedLength].errorinfo, errorinfo);

	res = ResultData.nErrorInfoListUsedLength ++;

    return  res;
}

//关键字表 编号由位置隐含
#define KEYWORD_COUNT 19
#define KEYWORD_MAXLENGTH 20
char KeyWordTable[KEYWORD_COUNT][KEYWORD_MAXLENGTH]
     = {"", "break", "case", "char" ,"continue" ,"default", "do",     "double",   "else" , "float",
	    "for" ,  "if",   "int",  "return",   "struct" , "switch" ,"unsigned" ,"void",  "while" };

/***********************************************
   函数名：LookupKeyword
   功能：查找关键字
   参数：char* idstr 要检查的标识符串
   返回值：>0 关键字码
            0 失败
************************************************/
int LookupKeyword(char* idstr)
{
  int i=0;
  for(i = 1; i<KEYWORD_COUNT; i++)
  {
     if(strcmp(KeyWordTable[i], idstr) == 0  )
		return i; /* 找到则返回关键字的种别码 */
  }
  return 0;
}


/***********************************************
   函数名：AppendSymList
   功能：添加符号表项
   参数：char* idstr 要添加的标识符或常数
         int kind    种类码
   返回值：>=0 标识符或常量在符号表中的位置
            -1 失败
************************************************/
int AppendSymList(char* idstr , int kind)
{
	SYMTAB* pSymtab = NULL;
	char* pWordList = NULL;
    int res = 0 ;
    /* 符号表已满,则增加SYMTAB_LENGTH长度的空间 */
	if(ResultData.nSymTabTotalLength <= ResultData.nSymTabUsedLength )
    {
        pSymtab = (SYMTAB*)malloc(sizeof(SYMTAB)*(ResultData.nSymTabTotalLength+SYMTAB_LENGTH) );
	    if(pSymtab == NULL)
		   return -1;
		memset(pSymtab , 0 ,ResultData.nSymTabTotalLength + SYMTAB_LENGTH );
		memcpy( pSymtab ,ResultData.pSymtab, sizeof(SYMTAB)*ResultData.nSymTabUsedLength);
        free(ResultData.pSymtab);
		ResultData.pSymtab = pSymtab;
		ResultData.nSymTabTotalLength += SYMTAB_LENGTH;
    }

    /* 字符列表已满,则增加WORDLIST_LENGTH 长度的空间 */
	if(ResultData.nWordListTotalLength - ResultData.nWordListUsedLength <= (int)strlen(idstr))
    {
		pWordList = (char*)malloc(sizeof(char)*(ResultData.nWordListTotalLength+WORDLIST_LENGTH) );
	    if(pWordList == NULL)
		   return -1;
		memset(pWordList , 0 ,ResultData.nWordListTotalLength+WORDLIST_LENGTH);
		memcpy( pWordList ,ResultData.pWordList, sizeof(char)*ResultData.nWordListTotalLength);
        free(ResultData.pWordList);
		ResultData.pWordList = pWordList;
		ResultData.nWordListTotalLength += WORDLIST_LENGTH;
    }

	/* 将标识符或常量添加到符号表 */
    strcpy(ResultData.pWordList + ResultData.nWordListUsedLength,idstr);
	ResultData.pSymtab[ResultData.nSymTabUsedLength].name.pos_in_wordlist = ResultData.nWordListUsedLength;
	ResultData.pSymtab[ResultData.nSymTabUsedLength].name.length = (int)strlen(idstr);
	ResultData.pSymtab[ResultData.nSymTabUsedLength].kind = kind;

	ResultData.nWordListUsedLength += (int)strlen(idstr)+1;

	res = ResultData.nSymTabUsedLength ++;

    return  res;
}

/***********************************************
   函数名：LookupSymList
   功能：查填符号表
   参数：char* idstr 要查、填的标识符及常数
         int kind    种类码
   返回值：>=0 标识符或常量在符号表中的位置
            -1 失败
************************************************/
int LookupSymList(char* idstr , int kind)
{
  int i=0;

  for(i = 0; i<ResultData.nSymTabUsedLength; i++)
  {
	  if(ResultData.pSymtab[i].kind == kind && strcmp( ResultData.pWordList + ResultData.pSymtab[i].name.pos_in_wordlist, idstr) == 0 )
		 return i; /* 找到则返回标识符或常数在符号表中的位置 */
  }

  /* 没找到则添加到符号表中 */
  return  AppendSymList( idstr ,  kind);
}

/***********************************************
   函数名：RecognitionID
   功能：处理标识符和关键字
   参数： char ch 当前字符(字母 或 下划线)
   返回值：1 识别成功
           0 失败
************************************************/
#define WORD_LENGTH 128
int RecognitionID(char ch)
{
	char word[WORD_LENGTH] = {0};
	int  wordlen = 0;
    TOKEN token;
	int flag = 0; /* 当前行结束标识 */

	word[wordlen] = ch;
	wordlen++;

	while(GetNextChar(&ch) )
	{
	  if( ch >= 'a' && ch <= 'z' || ch >='A' && ch <= 'Z' || ch >='0' && ch <= '9' || ch == '_' )
	  {  /*ch是字母或 数字或 下划线*/
		  if( wordlen < WORD_LENGTH-1)
		  {/* 字长小于标识符长度上限*/
	   		word[wordlen] = ch;
			wordlen++;
		  }
	  }
	  else
	  {
		/* 其它字符*/
		flag = 1;
	    break;
	  }
	}

	if(flag)
       BackChar(); /* 当前行未结束，字符位置回退1字符*/

	token.typecode = LookupKeyword(word);
    if( token.typecode > 0 ) /* 是关键字 */
	{
		token.entry = -1;
		if(AppendToken(token.typecode, token.entry)>=0)
		{
			printf("keyword: %s %d \n",word , token.typecode);
	        return 1;
		}
	}
	else /* 是标识符 */
	{
	  token.typecode = 19; /* 19为 标识符种别码 */
	  token.entry = LookupSymList(word ,token.typecode );
	  if(token.entry >=0 )
	  {
		if(AppendToken(token.typecode, token.entry)>=0)
		{
		   printf("symble: %s %d \n",word , token.entry);
		   return 1;
		}
	  }
	}

    AppendErrorInfo(1,"error identifier" , GetCurrentLineNo(), GetCurrntPos() );
    printf("error");
	return 0;
}

/***********************************************
   函数名：RecognitionStr
   功能：处理字符串常量
   参数： 无
   返回值：1 识别成功
           0 失败
************************************************/
int RecognitionStr()
{
    char word[WORD_LENGTH] = {0};
	int  wordlen = 0;
    TOKEN token;
	int flag = 0; /* 当前行结束标识 */
    char ch = 0;

    AppendToken(47, -1) ;

	while(GetNextChar(&ch))
	{
	  if(ch == '\"') /* 字符串匹配完毕 */
	  {
	    flag = 1;
		break;
	  }
	  else
	  {
	      if( wordlen < WORD_LENGTH-1)
		  {/* 字长小于字符串长度上限*/
	   		word[wordlen] = ch;
			wordlen++;
		  }
	  }
	}

	if(flag)
	{
	    token.typecode = 23; /* 23为 字符串常量种别码 */
		token.entry = LookupSymList(word ,token.typecode );
		if(token.entry >=0 )
		{
			if(AppendToken(token.typecode, token.entry)>=0)
			{
	           AppendToken(47, -1) ;
			   printf("string const: %s %d \n",word , token.entry);
			   return 1;
			}
		}
	}

	AppendErrorInfo(2,"error charactor" , GetCurrentLineNo(), GetCurrntPos() );
    printf("error\n");
    return 0;
}


/***********************************************
   函数名：RecognitionChar
   功能：处理字符型常量
   参数： 无
   返回值：1 识别成功
           0 失败
************************************************/
int RecognitionChar()
{
    char word[WORD_LENGTH] = {0};
	int  wordlen = 0;
    TOKEN token;
    char ch = 0;

	if(GetNextChar(&ch))
	{
	  if(ch != '\'') /* 有效字符 */
	  {
	    word[wordlen] = ch;
		wordlen++;

        if(GetNextChar(&ch))
	    {
	       if(ch == '\'') /* 合法字符型常量 */
		   {
				token.typecode = 22; /* 22为 字符型常量种别码 */
				token.entry = LookupSymList(word ,token.typecode );
				if(token.entry >=0 )
				{
					if(AppendToken(token.typecode, token.entry)>=0)
					{
						printf("charactor const: %s %d \n",word , token.entry);
						return 1;
					}
				}

		   }
		   else
		   {
				BackChar();  /* 非法的\' ,字符后面没有\' */
			    AppendErrorInfo(3,"error charactor const" , GetCurrentLineNo(), GetCurrntPos() );
				printf("error \' \n");
				return 0;
		   }

		}
		else
		{
		    AppendErrorInfo(3,"error charactor const" , GetCurrentLineNo(), GetCurrntPos() );
			printf("error \' \n");/*  非法的\',字符后面没有\' */
			return 0;
		}
	  }
	  else
	  {
        BackChar(); /*  两个\'中间没有字符 */
	    AppendErrorInfo(3,"error charactor const" , GetCurrentLineNo(), GetCurrntPos() );
	    printf("error \' \n");
		return 0;
	  }

	}
	else
	{
		printf("error \' \n");/*  非法的\',字符后面没有\' */
 	    AppendErrorInfo(3,"error charactor const" , GetCurrentLineNo(), GetCurrntPos() );

		return 0;
	}
    AppendErrorInfo(3,"error charactor const" , GetCurrentLineNo(), GetCurrntPos() );

    printf("error\n");
    return 0;
}

/*******************************************************
种别码表
单词		种别码	单词		种别码	单词	种别码   单词	种别码
break		1		void		17		!=		33       {      49
case		2		while		18		>=		34       }      50
char		3		标识符		19		<=		35
continue	4		整常数		20		++		36
default		5		实常数		21		--		37
do			6		字符常数	22		(		38
double		7		字符串		23		)		39
else		8		+			24		[		40
float		9		-			25		]		41
for			10		*			26		;		42
if			11		/			27		,		43
int			12		%			28		.		44
return		13		=			29		&		45
struct		14		>			30		'		46
switch		15		<			31		"		47
unsigned	16		==			32		_		48
********************************************************/

/*******************************************************
   函数名：RecognitionDel
   功能： 识别界限符
   参数： char ch 待识别界限符
   返回值：1 识别成功
           0 失败
*******************************************************/
int RecognitionDel(char ch)
{
	int kind = 0;
	char ch1 = 0;

    if(ch == '*')
      kind = 26;
	else if(ch == '/')
		kind = 27;
	else if(ch == '%')
		kind = 28;
	else if(ch == '(')
		kind = 38;
	else if(ch == ')')
		kind = 39;
	else if(ch == '[')
		kind = 40;
	else if(ch == ']')
		kind = 41;
	else if(ch == ';')
		kind = 42;
	else if(ch == ',')
		kind = 43;
	else if(ch == '&')
		kind = 45;
	else if(ch == '{')
		kind = 49;
	else if(ch == '}')
		kind = 50;
	else if(ch == '+')
	{
	   if(GetNextChar(&ch1))
	   {
	      if(ch1 == '+')
			  kind = 36;/* 自增++ */
		  else
		  {
			  BackChar();
			  kind = 24;
		  }
	   }
	   else  /*行结束*/
		  kind = 24;

	}
    else if(ch == '-')
	{
	   if(GetNextChar(&ch1))
	   {
	      if(ch1 == '-')
			  kind = 37; /* 自减-- */
		  else
		  {
			  BackChar();
			  kind = 25;
		  }
	   }
	   else  /*行结束*/
		  kind = 25;
	}
    else if(ch == '=')
	{
	   if(GetNextChar(&ch1))
	   {
	      if(ch1 == '=')
			  kind = 32; /* == */
		  else
		  {
			  BackChar();
			  kind = 29;
		  }
	   }
	   else  /*行结束*/
		  kind = 29;
	}
    else if(ch == '>')
	{
	   if(GetNextChar(&ch1))
	   {
	      if(ch1 == '=')
			  kind = 34; /* >= */
		  else
		  {
			  BackChar();
			  kind = 30;
		  }
	   }
	   else  /*行结束*/
		  kind = 30;
	}
    else if(ch == '<')
	{
	   if(GetNextChar(&ch1))
	   {
	      if(ch1 == '=')
			  kind = 35; /* <= */
		  else
		  {
			  BackChar();
			  kind = 31;
		  }
	   }
	   else  /*行结束*/
		  kind = 31;
	}
    else if(ch == '!')
	{
	   if(GetNextChar(&ch1))
	   {
	      if(ch1 == '=')
			  kind = 33; /* != */
		  else
		  {
			  BackChar();
			  kind = 0; /* 非法字符! */
		  }
	   }
	   else  /*行结束，是非法字符! */
		  kind = 0;
	}

    if( kind > 0 )  /*合法分隔符*/
	{
		if(AppendToken(kind, -1)>=0)
		{
			printf("delimiter: %c%c %d \n",ch ,ch1, kind);
			return 1;
		}
	}

    AppendErrorInfo(4,"illegl charactor" , GetCurrentLineNo(), GetCurrntPos() );
	printf("illegl charactor: %c \n", ch);

    return 0;

}


/***********************************************
   函数名：ProcessComments
   功能：处理注释 和 除号
   参数： 无
   返回值：1 识别成功
           0 失败
************************************************/
int ProcessComments()
{
  char ch = 0;

  if(GetNextChar(&ch))
  {
	if(ch == '*')  /* 注释符“/ *”     */
	{
		while( GetNextChar(&ch) )
		{
			if(ch == '*')
			{
				if(GetNextChar(&ch) && ch == '/') /* 找到注释符“* /” */
				{
					printf("comment\n");
					return 1;
				}
			}
		}
	}
	else /*除号*/
	{
		BackChar(); /*字符位置回退1字符*/
	    AppendToken( 27 , -1 );  /* 输出 ( /的种别码27 , "_" )*/
		printf("delimiter: %c%c %d \n",'/' ,0, 27);
		return 1;
	}
  }
  else /*后面没有字符了，是除号*/
  {
	 AppendToken( 27 , -1 );  /* 输出 ( /的种别码27 , "_" )*/
	 printf("delimiter: %c%c %d \n",'/' ,0, 27);
	 return 1;
  }

  AppendErrorInfo(0,"error comment" , GetCurrentLineNo(), GetCurrntPos() );
  printf("error\n");
  return 0;
}


/***********************************************
   函数名：RecognitionConst
   功能：  识别整型和实型常量(包括科学记数法的实型常量)
   参数：  ch	数字首符
   返回值：1 识别成功
           0 失败
************************************************/
int RecognitionConst (char ch)
{
    char word[WORD_LENGTH] = {0};
	int  wordlen = 0;
 	int flag = 0; /* 当前行结束标识 */
    int entry = 0;

	word[wordlen] = ch;
	wordlen++;

	while(GetNextChar(&ch))
	{
	  if( ch >='0' && ch<='9' )
	  {
	     word[wordlen] = ch;
	     wordlen++;
	  }
	  else
	  {
		  flag = 1;
		  break;
	  }
	}

	if(flag == 0) /*当前行结束,则是整型常量 ，查填符号表 */
	{
	  	entry = LookupSymList(word ,20 ); /* 整常量种别码为20*/
		if( entry >=0 )
		{
			if(AppendToken(20 , entry)>=0)
			{
				printf("Int Const: %s %d \n",word , entry);
				return 1;
			}
		}

	}
	else
	{
	   if(ch == '.')
	   {
		    word[wordlen] = ch;
			wordlen++;

			if(GetNextChar(&ch)) /* .后面有字符 */
			{
				if( ch >='0' && ch<='9' ) /*是数字*/
				{
					word[wordlen] = ch;
					wordlen++;

					flag = 0;
                    while(GetNextChar(&ch))
					{
						if( ch >='0' && ch<='9' )
						{
							word[wordlen] = ch;
							wordlen++;
						}
						else
						{
							flag = 1;
							break;
						}
					}

					if(flag == 0) /*行结束，则不是科学记数法表示的实型常量,查添符号表 */
					{
					    entry = LookupSymList(word ,21 ); /* 实型常量种别码为21*/
						if( entry >=0 )
						{
							if(AppendToken(21 , entry) >= 0)
							{
								printf("Real Const: %s %d \n",word , entry);
								return 1;
							}
						}
					}
					else /*可能是科学记数法表示的实型常量*/
					{
						if( ch == 'e' || ch == 'E')	/* 判断是否科学记数法表示的实型常量 */
						{
						    word[wordlen] = ch;
							wordlen++;

							if(GetNextChar(&ch))
							{
								if(ch == '+' || ch == '-')/* e 或 E 后面是 正负号 */
								{
									word[wordlen] = ch;
									wordlen++;

									if(GetNextChar(&ch))
									{
										if(ch >= '0' && ch <= '9') /* e+- 后面 是数字 ，则取后续数字*/
										{
											word[wordlen] = ch;
											wordlen++;

											while(GetNextChar(&ch))
											{
												if(ch >= '0' && ch <= '9')
												{
						    						word[wordlen] = ch;
													wordlen++;
												}
												else
												{
													BackChar(); /* 实数字符取完而 行未结束，回退一格 */
													break;
												}
											}
											/* 查添符号表 */
											entry = LookupSymList(word ,21 ); /* 实型常量种别码为21*/
											if( entry >=0 )
											{
												if(AppendToken(21 , entry) >= 0)
												{
													printf("Real Const: %s %d \n",word , entry);
													return 1;
												}
											}

										}
										else/* e+- 后面不是数字 则出错并应将字符位置回退1格*/
										{
											BackChar();
										    wordlen--;
										    word[wordlen] = 0;
										    AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
											printf("error real const %s\n",word);
											return 0;
										}
									}
									else /* e+- 后面没有字符 则出错，并使字符位置回退一格*/
									{
										BackChar();
										wordlen--;
										word[wordlen] = 0;
					   					AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
										printf("error real const %s\n",word);
										return 0;
									}

								}
								else if( ch >= '0' && ch <= '9') /* e 或 E 后面是 数字，则取后续数字 */
								{
										word[wordlen] = ch;
										wordlen++;

										while(GetNextChar(&ch))
										{
											if(ch >= '0' && ch <= '9')
											{
												word[wordlen] = ch;
												wordlen++;
											}
											else
											{
												BackChar(); /* 实数字符取完而 行未结束，回退一格 */
												break;
											}
										}
										/* 查添符号表 */
										entry = LookupSymList(word ,21 ); /* 实型常量种别码为21*/
										if( entry >=0 )
										{
											if(AppendToken(21 , entry) >= 0)
											{
												printf("Real Const: %s %d \n",word , entry);
												return 1;
											}
										}

								}
								else /*e 或 E 后面不是数字 ,则是整型常量 ,查添符号表,并应将字符位置回退1格 */
								{
							       AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
				                   printf("error real const: %s\n",word);
								   return 0;
								}

							}
							else /* e 或 E 后面没有字符,行结束，则出错 */
							{
							    AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
				                printf("error real const: %s\n",word);
								return 0;
							}
						}
						else  /*不是科学记数法表示的实型常量,查添符号表 ,同时应将字符位置回退1格 */
						{
						    BackChar();
							entry = LookupSymList(word ,21 ); /* 实型常量种别码为21*/
							if( entry >=0 )
							{
								if(AppendToken(21 , entry) >= 0)
								{
									printf("Real Const: %s %d \n",word , entry);
									return 1;
								}
							}

						}
					}

				}
				else /*小数点后面没有数字 给出出错信息  */
				{
				   AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
				   printf("error real const: %s\n",word);
				   return 0;
				}
			}
			else/*小数点后面没有字符,行结束 给出出错信息  */
			{
				AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
				printf("error real const: %s\n",word);
				return 0;
			}

	   }
	   else if( ch == 'e' || ch == 'E')	/* 判断是否科学记数法表示的实型常量 */
	   {
		   word[wordlen] = ch;
		   wordlen++;

           if(GetNextChar(&ch))
		   {
		       if(ch == '+' || ch == '-')/* e 或 E 后面是 正负号 */
			   {
				    word[wordlen] = ch;
					wordlen++;

			        if(GetNextChar(&ch))
		            {
                       if(ch >= '0' && ch <= '9') /* e+- 后面 是数字 ，则取后续数字*/
					   {
					      word[wordlen] = ch;
					      wordlen++;

						  while(GetNextChar(&ch))
						  {
						     if(ch >= '0' && ch <= '9')
							 {
						    	  word[wordlen] = ch;
					              wordlen++;
							 }
							 else
							 {
							    BackChar(); /* 实数字符取完而 行未结束，回退一格 */
								break;
							 }
						  }
						  /* 查添符号表 */
						  entry = LookupSymList(word ,21 ); /* 实型常量种别码为21*/
						  if( entry >=0 )
						  {
							if(AppendToken(21 , entry) >= 0)
							{
								printf("Real Const: %s %d \n",word , entry);
								return 1;
							}
						  }

					   }
					   else/* e+- 后面不是数字 则出错并应将字符位置回退1格*/
					   {
						    BackChar();
						    wordlen--;
						    word[wordlen] = 0;
						    AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
							printf("error real const %s\n", word);
							return 0;
					   }
					}
					else /* e+- 后面没有字符 则出错*/
					{
						BackChar();
						wordlen--;
						word[wordlen] = 0;

					   	AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
						printf("error real const: %s\n",word);
						return 0;
					}

			   }
			   else if( ch >= '0' && ch <= '9') /* e 或 E 后面是 数字，则取后续数字 */
			   {
			        word[wordlen] = ch;
					wordlen++;

					while(GetNextChar(&ch))
					{
						if(ch >= '0' && ch <= '9')
						{
						    word[wordlen] = ch;
					        wordlen++;
						}
						else
						{
							BackChar(); /* 实数字符取完而 行未结束，回退一格 */
							break;
						}
					}
					/* 查添符号表 */
					entry = LookupSymList(word ,21 ); /* 实型常量种别码为21*/
					if( entry >=0 )
					{
						if(AppendToken(21 , entry) >= 0)
						{
							printf("Real Const: %s %d \n",word , entry);
							return 1;
						}
					}

			   }
			   else /*e 或 E 后面不是+-号或 数字 ,则出错 */
			   {
				    AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
					printf("error real const: %s\n",word);
					return 0;
			   }

		   }
		   else /* e 或 E 后面没有字符,行结束，则出错 */
		   {
				AppendErrorInfo(7,"error real const" , GetCurrentLineNo(), GetCurrntPos() );
				printf("error real const: %s\n",word);
				return 0;
		   }
	   }
	   else if(ch >= 'A' && ch<='Z' || ch >= 'a' && ch<='z' || ch == '_') /* 数字后面不是界限符，出错 */
	   {
	       AppendErrorInfo(6,"error int const" , GetCurrentLineNo(), GetCurrntPos() );
		   printf("error int const: %s\n",word);
		   return 0;
	   }
	   else /* 是整型常量 ，查填符号表，字符位置回退1格 */
	   {
		    BackChar();
	        entry = LookupSymList(word ,20 ); /* 整常量种别码为20*/
			if( entry >=0 )
			{
				if(AppendToken(20 , entry)>=0)
				{
				   printf("Int Const: %s %d \n",word , entry);
				   return 1;
				}
			}
	   }
	}

    return 0;
}

/***********************************************
   函数名：Classify
   功能：单词首字符分类
   参数：ch	首符
   返回值：1 识别成功
           0 失败
************************************************/
int Classify(char ch)
{
	int res = 0;
    if(ch >= 'a' && ch <= 'z' || ch >='A' && ch <= 'Z' || ch == '_') /*字母和下划线*/
	{
	   res = RecognitionID(ch);
	}
	else if(ch >= '0' && ch <= '9')/*数字*/
	{
	 res =  RecognitionConst ( ch);
	}
	else if(ch == '/')  /*斜线*/
	{
	   res = ProcessComments();
	}
    else if(ch == '\'')  /*单引号*/
	{
	   res =  RecognitionChar();

	}
	else if( ch == '\"')  /*双引号*/
	{
	   res = RecognitionStr();
	}
	else  if(ch!=32 && ch!=0x0d && ch!=0x0a && ch!= '\t') /*除空格、回车、换行、tab外的其它字符*/
	{
	   res = RecognitionDel( ch);

	}

	return res;
}

/*MAX_PATH 文件路径长度上限 */
#define MAX_PATH   260

/************************************************
   函数名：Scaner
   功能： 词法分析
   参数： char* srcfile 源文件名（文本格式）
   返回值：1 成功
           0 失败
*************************************************/
int Scaner(char* srcfilename)
{
	char ch = 0;
    FILE *fpSrc = NULL, *fpToken = NULL, *fpSymble = NULL, *fpError = NULL;
    /* fpSrc 目标（TOKEN）文件指针 fpSymble 符号表文件文件指针 fpError 出错信息文件指针*/

    int i = 0;
	char objfilename[MAX_PATH] = {0}, symfilename[MAX_PATH] = {0}, errfilename[MAX_PATH] = {0};
    /* objfilename 目标（TOKEN）文件名（文本文件） symfilename 符号表文件名（文本文件） errfilename 出错信息文件名（文本文件）*/

	if(( fpSrc = fopen(srcfilename, "r"))==NULL )
       return 0;

	while(i< MAX_PATH-4 && srcfilename[i]!=0 && srcfilename[i]!='.' )
	{
		objfilename[i] = srcfilename[i];
		symfilename[i] = srcfilename[i];
		errfilename[i] = srcfilename[i];
	    i++;
	}
	strcat(objfilename,".tok"); /* 生成 目标（TOKEN）文件名 */
	strcat(symfilename,".sym"); /* 生成 符号表文件名*/
	strcat(errfilename,".err"); /* 生成 出错信息文件名*/


	if(( fpToken = fopen(objfilename, "w"))==NULL )
	{
		fclose(fpSrc);
		return 0;
	}
	if(( fpSymble = fopen(symfilename, "w"))==NULL )
	{
		fclose(fpSrc);
		fclose(fpToken);

		return 0;
	}
 	if(( fpError = fopen(errfilename, "w"))==NULL )
	{
		fclose(fpSrc);
		fclose(fpToken);
		fclose(fpSymble);
		return 0;
	}
    while(GetString(fpSrc)) /* 处理程序中的每一行，空行除外 */
	{
		process_pos = 0;  /* 待处理字符位置 */
		real_length = (int)strlen(linebuffer);  /* 当前程序行的长度 */

		while(GetNextChar(&ch)) /* 处理当前行的字符，直到行结束 */
		{
			Classify(ch);
		}
	}

	SaveTokenToFile(fpToken);
    SaveSymTabToFile(fpSymble);
	SaveErrorInfoToFile(fpError);

	fclose(fpSrc);
	fclose(fpToken);
	fclose(fpSymble);
	fclose(fpError);
	return 1;
}

/*****非终结符表
（1）FunctionGroup    0x0100
（2） Function        0x0200
（3） FunctionHead    0x0300
（4）FunctionBody     0x0400
（5）BaseType         0x0500
（6）ForParList       0x0600
（7）FormListMore     0x0700
（8）Sentence         0x0800
（9）Sen              0x0900
（10）DecSentence     0x0a00
（11）IdList          0x0b00
（12）IdMore          0x0c00
（13）AssignmentSen   0x0d00
（14）FunCallSen      0x0e00
（15）ActParList      0x0f00
（16）ActParListMore  0x1000
（17）ConditionalSen  0x1100
（18）SwitchBody      0x1200
（19）ConsExpression  0x1300
（20）ConsConExpressionMore 0x1400
（21）ConsCountExpression   0x1500
（22）ConsTermMore    0x1600
（23）ConsTerm        0x1700
（24）ConsFactorMore  0x1800
（25）ConsFactor      0x1900
（26）ConsLost        0x1a00
（27）SwiBodyMore     0x1b00
（28）CaseBody        0x1c00
（29）LoopSen         0x1d00
（30）ForExpression   0x1e00
（31）ReturnSen       0x1f00
（32）Expression      0x2000
（33）ConExpression   0x2100
（34）ConExpressionMore  0x2200
（35）CountExpression    0x2300
（36）RemainTerm      0x2400
（37）Term            0x2500
（38）RemainFactor    0x2600
（39）Factor          0x2700
（40）Lost            0x2800
（41）Variable        0x2900
（42）CmpOp           0x2a00
（43）AddOp           0x2b00
（44）MultOp          0x2c00
（45）SingleOp        0x2d00
（46）FunCallSenMore  0x2e00
（47）ActParListen    0x2f00
（48）ActParLisPrintf 0x3000
（49）ActParLisScanf  0x3100
（50）FunctionName    0x3200
*/

/*******种别码表(终结符列表)
break		1	0x0001
case	    2	0x0002
char	    3	0x0003
continue	4	0x0004
default		5	0x0005
do			6	0x0006
double		7	0x0007
else		8	0x0008
float		9	0x0009
for			10	0x000a
if			11	0x000b
int			12	0x000c
return		13	0x000d
struct		14	0x000e
switch		15	0x000f
unsigned   	16	0x0010
void		17	0x0011
while		18	0x0012
ID  		19	0x0013
Intc 		20	0x0014
Floatc		21	0x0015
Charc   	22	0x0016
Stringc		23	0x0017
+			24	0x0018
-			25	0x0019
*			26	0x001a
/			27	0x001b
%			28	0x001c
=			29	0x001d
>			30	0x001e
<			31	0x001f
==			32	0x0020
!=		    33  0x0021
>=		    34  0x0022
<=		    35  0x0023
++		    36  0x0024
--		    37  0x0025
(		    38  0x0026
)		    39  0x0027
[		    40  0x0028
]		    41  0x0029
;		    42  0x002a
,		    43  0x002b
.		    44  0x002c
&		    45  0x002d
'		    46  0x002e
"		    47  0x002f
_	     	48  0x0030
{           49  0x0031
}           50  0x0032
:           51  0x0033

*/
/******************文法列表 94条文法，每条文法长度 < 32  ****************
         文法表示：
		           非终结符形为0x0100--0x2C(十进制 1-45)
				   终结符形为0x0001--0x0033(十进制 1-51)
				   ε 为0x0034             (十进制 52  )
************************************************************************/

unsigned short int grammar[103][32] = {
{0x0100},
/*（0） FunctionGroup                                           */
{0x0200, 0x0100},
/*（1） FunctionGroup  ：：=   Function FunctionGroup           */
{0xFFFF},
/*（2） FunctionGroup  ：：=  ε                                */
{0x0300, 0x0400},
/*（3） Function       ：：=   FunctionHead FunctionBody        */
{0x0500, 0x0013, 0x0026, 0x0600, 0x0027},
/*（4） FunctionHead   ：：=  BaseType ID(ForParList)           */
{0x0011, 0x0013, 0x0026, 0x0600, 0x0027},
/*（5） FunctionHead   ：：=  void ID(ForParList)               */
{0x0031, 0x0800, 0x0032},
/*（6）FunctionBody    ：：=  {Sentence }                       */
{0x000c},
/*（7） BaseType       ：：=  int                               */
{0x0003},
/*（8） BaseType       ：：=  char                              */
{0x0009},
/*（9） BaseType       ：：=  float                             */
{0x0007},
/*（11）BaseType       ：：=  double                            */
{0x0013, 0x0700},
/*（11）ForParList     ：：=  Id FormListMore                   */
{0xFFFF},    /*（12）ForParList     ：：=  ε                   */
{0x002b, 0x0600},
/*（13）FormListMore   ：：=, ForParList                        */
{0xFFFF},
/*（14）FormListMore   ：：=  ε                                */
{0x0900, 0x0800},
/*（15）Sentence       ：：=  Sen Sentence                      */
{0xFFFF},
/*（16）Sentence       ：：=  ε                                */
{0x0a00},
/*（17）Sen            ：：=  DecSentence                       */
{0x1100},
/*（18）Sen            ：：=  ConditionalSen                    */
{0x1d00},
/*（19）Sen            ：：=  LoopSen                           */
{0x1f00},
/*（20）Sen            ：：=  ReturnSen                         */
{0x0e00},
/*（21）Sen            ：：=  FunCallSen                        */
{0x0d00},
/*（22）Sen            ：：=  AssignmentSen                     */
{0x2000},
/*（23）Sen            ：：=  Expression;                       */
{0x0004, 0x002a},
/*（24）Sen            ：：=  continue;                         */
{0x0001, 0x002a},
/*（25）Sen            ：：=  break;                            */
{0x0500, 0x0b00, 0x002a},
/*（26）DecSentence    ：：=  BaseType IdList; DecSentence      */
{0x0013, 0x0c00},
/*（27）IdList         ：：=  Id IdMore                         */
{0x002b, 0x0b00},
/*（28）IdMore         ：：=  ,IdList                           */
{0xFFFF},
/*（29）IdMore         ：：= ε                                 */
{0x0013, 0x001d, 0x2000, 0x002a},
/*（30）AssignmentSen  ：：=  Id=Expression;                    */
{0x3200, 0x0026, 0x2e00 },
/*（31）FunCallSen     ：：=   FunctionName（FunCallSenMore     */
{0x2000, 0x1000},
/*（32）ActParList     ：：=  Expression  ActParListMore        */
{0xFFFF},
/*（33）ActParList     ：：=  ε                                */
{0x002b, 0x0f00},
/*（34）ActParListMore ：：=  , ActParList                      */
{0xFFFF},
/*（35）ActParListMore ：：=  ε                                */
{0x000b, 0x0026, 0x2000, 0x0027, 0x0031, 0x0800, 0x0032},
/*（36）ConditionalSen ：：=  if (Expression ) {Sentence}       */
{0x000f, 0x0026, 0x2000, 0x0027, 0x0031, 0x1200, 0x0032},
/*（37）ConditionalSen ：：=  switch(Expression){SwitchBody}    */
{0x0002, 0x1300, 0x0033, 0x1c00, 0x1b00},
/*（38）SwitchBody     ：：=  case ConsExpression: CaseBody SwiBodyMore */
{0x1500, 0x1400},
/*（39）ConsExpression ：：=  ConsCountExpression ConsConExpressionMore */
{0x2a00, 0x1500, 0x1400},
/*（40）ConsConExpressionMore：：= CmpOp ConsCountExpression ConsConExpressionMore*/
{0xFFFF},
/*（41）ConsConExpressionMore：：= ε                           */
{0x1700, 0x1600},
/*（42）ConsCountExpression  ：：= ConsTerm  ConsTermMore       */
{0x2b00, 0x1700, 0x1600},
/*（43）ConsTermMore   ：：=  AddOp ConsTerm ConsTermMore       */
{0xFFFF},
/*（44）ConsTermMore   ：：=  ε                                */
{0x1900, 0x1800},
/*（45）ConsTerm       ：：=  ConsFactor ConsFactorMore         */
{0x2c00, 0x1900, 0x1800},
/*（46）ConsFactorMore ：：=  MultOp ConsFactor ConsFactorMore  */
{0xFFFF},
/*（47）ConsFactorMore ：：= ε                                 */
{0x2d00, 0x1a00},
/*（48）ConsFactor     ：：=  SingleOp ConsLost                 */
{0x1a00},
/*（49）ConsFactor     ：：=  ConsLost                          */
{0x0014},
/*（50）ConsLost       ：：=  Intc                              */
{0x002e, 0x0016, 0x002e},
/*（51）ConsLost       ：：=  'Charc'                           */
{0x0026, 0x1300, 0x0027},
/*（52）ConsLost       ：：= (ConsExpression)                   */
{0x1200},
/*（53）SwiBodyMore    ：：=  SwitchBody                        */
{0x0005, 0x0033, 0x0800},
/*（54）SwiBodyMore    ：：=  default: Sentence                 */
{0xFFFF},
/*（55）SwiBodyMore    ：：= ε                                 */
{0x0800},
/*（56）CaseBody       ：：=  Sentence                          */
{0x0012, 0x0026, 0x2000, 0x0027, 0x0031, 0x0800, 0x0032},
/*（57）LoopSen        ：：=  while (Expression )  {Sentence}   */
{0x0006, 0x0031, 0x0800, 0x0032, 0x0012, 0x0026, 0x2000, 0x0027, 0x002a},
/*（58）LoopSen        ：：=  do{Sentence}while (Expression );  */
{0x000a, 0x0026, 0x2000, 0x2700, 0x0031, 0x0800, 0x0032},
/*（59）LoopSen        ：：=  for (ForExpression )  {Sentence}  */
{0x2000, 0x002a, 0x2000, 0x002a, 0x2000},
/*（60）ForExpression  ：：=Expression; Expression; Expression  */
{0x000d, 0x0026, 0x2000, 0x0027, 0x002a},
/*（61）ReturnSen      ：：=return(Expression);                 */
{0x2100},
/*（62）Expression     ：：=ConExpression                       */
{0x2300, 0x2200},
/*（63）ConExpression  ：：= CountExpression ConExpressionMore  */
{0x2a00, 0x2300, 0x2200},
/*（64）ConExpressionMore  ：：= CmpOp CountExpression ConExpressionMore */
{0xFFFF},
/*（65）ConExpressionMore  ：：= ε                             */
{0x2500, 0x2400},
/*（66）CountExpression    ：：= Term RemainTerm                */
{0x2b00, 0x2500, 0x2400},
/*（67）RemainTerm     ：：= AddOp Term RemainTerm              */
{0xFFFF},
/*（68）RemainTerm     ：：= ε                                 */
{0x2700, 0x2600},
/*（69）Term           ：：= Factor RemainFactor                */
{0x2c00, 0x2700, 0x2600},
/*（70）RemainFactor   ：：= MultOp Factor RemainFactor         */
{0xFFFF},
/*（71）RemainFactor   ：：= ε                                 */
{0x2d00, 0x2800},
/*（72）Factor         ：：= SingleOp Lost                      */
{0x2800},
/*（73）Factor         ：：= Lost                               */
{0x0026, 0x2000, 0x0027},
/*（74）Lost           ：：= ( Expression)                      */
{0x0014},
/*（75）Lost           ：：= Intc                               */
{0x2900},
/*（76）Lost           ：：= Variable                           */
{0x0015},
/*（77）Lost           ：：= Floatc                             */
{0x002e, 0x0016, 0x002e},
/*（78）Lost           ：：= 'Charc'                            */
{0x0013},
/*（79）Variable       ：：= Id                                 */
{0x001f},
/*（80）CmpOp          ：：= <                                  */
{0x0023},
/*（81）CmpOp          ：：= <=                                 */
{0x001e},
/*（82）CmpOp          ：：= >                                  */
{0x0022},
/*（83）CmpOp          ：：= >=                                 */
{0x0020},
/*（84）CmpOp          ：：= ==                                 */
{0x0021},
/*（85）CmpOp          ：：= !=                                 */
{0x0018},
/*（86）AddOp          ：：= +                                  */
{0x0019},
/*（87）AddOp          ：：= -                                  */
{0x001a},
/*（88）MultOp         ：：= *                                  */
{0x001b},
/*（89）MultOp         ：：= /                                  */
{0x001c},
/*（90）MultOp         ：：= %                                  */
{0x0019},
/*（91）SingleOp       ：：= -                                  */
{0x0024},
/*（92）SingleOp       ：：= ++                                 */
{0x0025},
/*（93）SingleOp       ：：= --                                 */
{0x0f00, 0x0027,0x002a},
/*（94）FunCallSenMore ：：= ActParList);                       */
{0x002f, 0x0017 , 0x002f, 0x2f00, 0x0027, 0x002a },
/*（95）FunCallSenMore ：：= "String" ActParListen);           */
{0x3000},
/*（96）ActParListen   ：：= ActParLisPrintf                    */
{0x3100},
/*（97）ActParListen   ：：= ActParLisScanf                     */
{0x002b, 0x2000, 0x3000},
/*（98）ActParLisPrintf：：=, Expression ActParLisPrintf        */
{0xFFFF},
/*（99）ActParLisPrintf：：=ε                                  */
{0x002b,0x002d, 0x2900, 0x3100 },
/*（100）ActParLisScanf：：=, & Variable ActParLisScanf         */
{0xFFFF},
/*（101）ActParLisScanf：：=  ε                                */
{0x0013},
/*（102）FunctionName        ：：= Id                           */
};


/************************************************
   函数名：SearchLL1Table
   功能：  查找LL1分析表
   参数：  int uterm  非终结符编号
           int term   终结符编号
   返回值：>0 成功
            0 查找失败
           <0 错误码
*************************************************/
int SearchLL1Table(int uterm, int term)
{
  switch(uterm)
  {
	case 1:
        switch(term)
		{
		  case 3:
		  case 7:
		  case 9:
		  case 12:
		  case 17:
			  return 1;
		  default:
			  return -uterm;
		}
		break;

	case 2:
        switch(term)
		{
		  case 3:
		  case 7:
		  case 9:
		  case 12:
		  case 17:
			  return 3;
		  default:
			  return -uterm;
		}
		break;

 	case 3:
        switch(term)
		{
		  case 3:
		  case 7:
		  case 9:
		  case 12:
			  return 4;

		  case 17:
			  return 5;
		  default:
			  return -uterm;
		}
		break;

 	case 4:
        switch(term)
		{
		  case 49:
			  return 6;
		  default:
			  return -uterm;
		}
		break;

	case 5:
        switch(term)
		{
		  case 3:
			  return 8;
		  case 7:
              return 10;
		  case 9:
			  return 9;
		  case 12:
			  return 7;
		  default:
			  return -uterm;

		}
		break;

	case 6:
        switch(term)
		{
		  case 19:
			  return 11;
		  case 39:
			  return 12;

		  default:
			  return -uterm;

		}
		break;

	case 7:
        switch(term)
		{
		  case 39:
			  return 14;
		  case 43:
			  return 13;
		  default:
			  return -uterm;

		}
		break;

	case 8:
        switch(term)
		{
		  case 1:
		  case 3:
		  case 4:
		  case 6:
		  case 7:
		  case 9:
		  case 10:
		  case 11:
		  case 12:
		  case 13:
		  case 15:
		  case 18:
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 15;
		  case 50:
			  return 16;
		  default:
			  return -uterm;
		}
		break;

	case 9:
        switch(term)
		{
		  case 1:
			  return 25;
		  case 3:
		  case 7:
		  case 9:
		  case 12:
			  return 17;
		  case 4:
			  return 24;
		  case 6:
 		  case 10:
		  case 18:
			  return 19;
		  case 11:
		  case 15:
			  return 18;
 		  case 13:
			  return 20;

 		  case 19:
			  return 21; /*返回21时 应判断是否 22 23 的情况*/
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 23;
		  default:
			  return -uterm;
		}
		break;

	case 10:
        switch(term)
		{
		  case 3:
		  case 7:
		  case 9:
		  case 12:
 			  return 26;
		  default:
			  return -uterm;

		}
		break;

	case 11:
        switch(term)
		{
		  case 19:
 			  return 27;
		  default:
			  return -uterm;
		}
		break;

	case 12:
        switch(term)
		{
		  case 42:
			  return 29;
		  case 43:
			  return 28;
		  default:
			  return -uterm;

		}
		break;

	case 13:
        switch(term)
		{
		  case 19:
			  return 30;
		  default:
			  return -uterm;
		}
		break;

	case 14:
        switch(term)
		{
		  case 19:
			  return 31;
		  default:
			  return -uterm;
		}
		break;

	case 15:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 32;
		  case 39:
			  return 33;
		  default:
			  return -uterm;
		}
		break;

	case 16:
        switch(term)
		{
		  case 39:
			  return 35;
		  case 43:
			  return 34;
		  default:
			  return -uterm;
		}
		break;

	case 17:
        switch(term)
		{
		  case 11:
			  return 36;
		  case 15:
			  return 37;
		  default:
			  return -uterm;
		}
		break;

	case 18:
        switch(term)
		{
		  case 2:
			  return 38;
		  default:
			  return -uterm;

		}
		break;

	case 19:
        switch(term)
		{
		  case 20:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 39;
		  case 51:
			  return 41;
		  default:
			  return -uterm;
		}
		break;

	case 20:
        switch(term)
		{
		  case 30:
		  case 31:
		  case 32:
		  case 33:
		  case 34:
		  case 35:
			  return 40;
		  default:
			  return -uterm;
		}
		break;

	case 21:
        switch(term)
		{
		  case 20:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 42;
		  default:
			  return -uterm;
		}
		break;

	case 22:
        switch(term)
		{
		  case 24:
		  case 25:
			  return 43;
		  case 30:
		  case 31:
		  case 32:
		  case 33:
		  case 34:
		  case 35:
		  case 51:
			  return 44;
		  default:
			  return -uterm;
		}
		break;

	case 23:
        switch(term)
		{
		  case 20:
		  case 25:
 		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 45;
		  default:
			  return -uterm;
		}
		break;

	case 24:
        switch(term)
		{
		  case 24:
		  case 25:
		  case 30:
		  case 31:
		  case 32:
		  case 33:
		  case 34:
		  case 35:
		  case 51:
			  return 47;
		  case 26:
		  case 27:
		  case 28:
			  return 46;
		  default:
			  return -uterm;
		}
		break;

	case 25:
        switch(term)
		{
		  case 20:
		  case 38:
		  case 46:
			  return 49;
		  case 25:
		  case 36:
		  case 37:
			  return 48;
		  default:
			  return -uterm;
		}
		break;

	case 26:
        switch(term)
		{
		  case 20:
			  return 50;
		  case 38:
			  return 52;
		  case 46:
			  return 51;
		  default:
			  return -uterm;
		}
		break;

	case 27:
        switch(term)
		{
		  case 2:
			  return 53;
		  case 5:
			  return 54;
		  case 50:
			  return 55;
		  default:
			  return -uterm;
		}
		break;

	case 28:
        switch(term)
		{
		  case 1:
		  case 3:
		  case 4:
		  case 6:
		  case 7:
		  case 9:
		  case 10:
		  case 11:
		  case 12:
		  case 13:
		  case 15:
		  case 18:
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 56;
		  default:
			  return -uterm;
		}
		break;

	case 29:
        switch(term)
		{
		  case 6:
			  return 58;
		  case 10:
			  return 59;
		  case 18:
			  return 57;
		  default:
			  return -uterm;
		}
		break;

	case 30:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
			  return 60;
		  default:
			  return -uterm;

		}
		break;

	case 31:
        switch(term)
		{
		  case 13:
			  return 61;
		  default:
			  return -uterm;
		}
		break;

 	case 32:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
 			  return 62;
		  default:
			  return -uterm;

		}
		break;

	case 33:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
 			  return 63;
		  default:
			  return -uterm;
		}
		break;

	case 34:
        switch(term)
		{
		  case 30:
		  case 31:
		  case 32:
		  case 33:
		  case 34:
		  case 35:
			  return 64;
		  case 39:
		  case 42:
		  case 43:
			  return 65;
		  default:
			  return -uterm;
		}
		break;

	case 35:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
 			  return 66;
		  default:
			  return -uterm;
		}
		break;

	case 36:
        switch(term)
		{
		  case 24:
		  case 25:
			  return 67;
		  case 30:
		  case 31:
		  case 32:
		  case 33:
		  case 34:
		  case 35:
 		  case 39:
		  case 42:
		  case 43:
			  return 68;
		  default:
			  return -uterm;
		}
		break;

	case 37:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 25:
		  case 36:
		  case 37:
		  case 38:
		  case 46:
 			  return 69;
		  default:
			  return -uterm;
		}
		break;

	case 38:
        switch(term)
		{
		  case 24:
		  case 25:
			  return 71;
		  case 26:
		  case 27:
		  case 28:
			  return 70;
	  	  case 30:
		  case 31:
		  case 32:
		  case 33:
		  case 34:
		  case 35:
 		  case 39:
		  case 42:
		  case 43:
			  return 71;
		  default:
			  return -uterm;
		}
		break;

	case 39:
        switch(term)
		{
		  case 19:
		  case 20:
		  case 21:
		  case 38:
		  case 46:
 			  return 73;
		  case 25:
		  case 36:
		  case 37:
 			  return 72;
		  default:
			  return -uterm;
		}
		break;

	case 40:
        switch(term)
		{
		  case 19:
			  return 76;
		  case 20:
			  return 75;
		  case 21:
			  return 77;
		  case 38:
			  return 74;
		  case 46:
			  return 78;
		  default:
			  return -uterm;
		}
		break;

	case 41:
        switch(term)
		{
		  case 19:
			  return 79;
		  default:
			  return -uterm;
		}
		break;

	case 42:
        switch(term)
		{
		  case 30:
			  return 82;
		  case 31:
			  return 80;
		  case 32:
			  return 84;
		  case 33:
			  return 85;
		  case 34:
			  return 83;
		  case 35:
			  return 81;
		  default:
			  return -uterm;
		}
		break;

	case 43:
        switch(term)
		{
		  case 24:
			  return 86;
		  case 25:
			  return 87;
		  default:
			  return -uterm;
		}
		break;

	case 44:
        switch(term)
		{
		  case 26:
			  return 88;
		  case 27:
			  return 89;
		  case 28:
			  return 90;
		  default:
			  return -uterm;
		}
		break;

	case 45:
        switch(term)
		{
		  case 25:
			  return 91;
		  case 36:
			  return 92;
		  case 37:
			  return 93;
		  default:
			  return -uterm;
		}
		break;

	case 46:
        switch(term)
		{
		  case 19:
 		  case 20:
 		  case 21:
 		  case 25:
 		  case 36:
 		  case 37:
 		  case 38:
 		  case 39:
 		  case 46:
 			  return 94;
		  case 47:
			  return 95;
		  default:
			  return -uterm;
		}
		break;

	case 47:
        switch(term)
		{
		  case 39:
 		  case 43:
			  return 96;
		  default:
			  return -uterm;
		}
		break;

	case 48:
        switch(term)
		{
		  case 39:
			  return 99;
 		  case 43:
			  return 98;
		  default:
			  return -uterm;
		}
		break;

	case 49:
        switch(term)
		{
		  case 39:
			  return 101;
		  case 43:
			  return 100;
 		  default:
			  return -uterm;
		}
		break;

 	case 50:
        switch(term)
		{
		  case 19:
 			  return 102;
		  default:
			  return -uterm;
		}
		break;
 }

  return 0;
}

typedef struct ustck
{
   unsigned short int *utermstack; //栈指针
   int stacklen;  //栈空间当前长度
   int top;       //栈顶指针
}USTACK;

USTACK utermstack; //非终结符栈

/***********************************************
   函数名：InitStack()
   功能：  初始化非终结符栈深度为1*1024
   参数：  无
   返回值：1 成功
           0 失败
************************************************/
int InitStack()
{
  utermstack.stacklen = 1;
  utermstack.utermstack = (unsigned short int *)malloc(utermstack.stacklen * 1024 * sizeof(unsigned short int));
  memset(utermstack.utermstack , 0 , sizeof(utermstack.stacklen * 1024 * sizeof(unsigned short int)));
  utermstack.top = 0;
  if(utermstack.utermstack)
     return 1;
  else
     return 0;
}

/***********************************************
   函数名：ResizeStack()
   功能：  扩展非终结符栈深度使之增加1024
   参数：  无
   返回值：1 成功
           0 失败
************************************************/
int ResizeStack()
{
  unsigned short int *p = NULL;
  utermstack.stacklen ++;
  p = (unsigned short int *)malloc(utermstack.stacklen * 1024 * sizeof(unsigned short int));
  memset(p , 0 , sizeof(utermstack.stacklen * 1024 * sizeof(unsigned short int)));

  memcpy( p, utermstack.utermstack ,(utermstack.stacklen-1) * 1024 * sizeof(unsigned short int) );

  if(utermstack.utermstack != NULL)
  	  free(utermstack.utermstack);

  utermstack.utermstack = p;
  if(utermstack.utermstack)
     return 1;
  else
     return 0;
}

/***********************************************
   函数名：DestroyStack()
   功能：  扩展非终结符栈深度使之增加1024
   参数：  无
   返回值：1 成功
           0 失败
************************************************/
int DestroyStack()
{
  if(utermstack.utermstack != NULL)
  	  free(utermstack.utermstack);
  return 1;
}

/***********************************************
   函数名：IsStackEmpty()
   功能：  判断非终结符栈是否为空
   参数：  无
   返回值：1 空
           0 非空
************************************************/
int IsStackEmpty()
{
  if(utermstack.utermstack != NULL && utermstack.top > 0)
    return 1;
  else
    return 0;
}

/***********************************************
   函数名：PrintStack()
   功能：  输出符号栈内容
   参数：  无
   返回值：1 空
           0 非空
************************************************/
int PrintStack()
{
  int i = 0;
  printf("\n");

  for( i = 1 ; utermstack.utermstack != NULL && i <= utermstack.top ; i++ )
  {
    switch(utermstack.utermstack[i])
	{
	    case 0x0100:
		   printf("FunctionGroup ");
		   break;
	    case 0x0200:
		   printf("Function ");
		   break;
	    case 0x0300:
		   printf("FunctionHead ");
		   break;
	    case 0x0400:
		   printf("FunctionBody ");
		   break;
 	    case 0x0500:
		   printf("BaseType ");
		   break;
	    case 0x0600:
		   printf("ForParList ");
		   break;
	    case 0x0700:
		   printf("FormListMore ");
		   break;
	    case 0x0800:
		   printf("Sentence ");
		   break;
	    case 0x0900:
		   printf("Sen ");
		   break;
	    case 0x0a00:
		   printf("DecSentence ");
		   break;

	    case 0x0b00:
		   printf("IdList ");
		   break;
	    case 0x0c00:
		   printf("IdMore ");
		   break;
	    case 0x0d00:
		   printf("AssignmentSen ");
		   break;
	    case 0x0e00:
		   printf("FunCallSen ");
		   break;
 	    case 0x0f00:
		   printf("ActParList ");
		   break;
	    case 0x1000:
		   printf("ActParListMore ");
		   break;
	    case 0x1100:
		   printf("ConditionalSen ");
		   break;
	    case 0x1200:
		   printf("SwitchBody ");
		   break;
	    case 0x1300:
		   printf("ConsExpression ");
		   break;
	    case 0x1400:
		   printf("ConsConExpressionMore ");
		   break;

	    case 0x1500:
		   printf("ConsCountExpression ");
		   break;
	    case 0x1600:
		   printf("ConsTermMore ");
		   break;
	    case 0x1700:
		   printf("ConsTerm ");
		   break;
	    case 0x1800:
		   printf("ConsFactorMore ");
		   break;
 	    case 0x1900:
		   printf("ConsFactor ");
		   break;
	    case 0x1a00:
		   printf("ConsLost ");
		   break;
	    case 0x1b00:
		   printf("SwiBodyMore ");
		   break;
	    case 0x1c00:
		   printf("CaseBody ");
		   break;
	    case 0x1d00:
		   printf("LoopSen ");
		   break;
	    case 0x1e00:
		   printf("ForExpression ");
		   break;

	    case 0x1f00:
		   printf("ReturnSen ");
		   break;
	    case 0x2000:
		   printf("Expression ");
		   break;
	    case 0x2100:
		   printf("ConExpression ");
		   break;
	    case 0x2200:
		   printf("ConExpressionMore ");
		   break;
 	    case 0x2300:
		   printf("CountExpression ");
		   break;
	    case 0x2400:
		   printf("RemainTerm ");
		   break;
	    case 0x2500:
		   printf("Term ");
		   break;
	    case 0x2600:
		   printf("RemainFactor ");
		   break;
	    case 0x2700:
		   printf("Factor ");
		   break;
	    case 0x2800:
		   printf("Lost ");
		   break;

	    case 0x2900:
		   printf("Variable ");
		   break;
	    case 0x2a00:
		   printf("CmpOp ");
		   break;
	    case 0x2b00:
		   printf("AddOp ");
		   break;
	    case 0x2c00:
		   printf("MultOp ");
		   break;
	    case 0x2d00:
		   printf("SingleOp ");
		   break;
	    case 0x2e00:
		   printf("FunCallSenMore ");
		   break;
	    case 0x2f00:
		   printf("ActParListen ");
		   break;
	    case 0x3000:
		   printf("ActParLisPrintf ");
		   break;
	    case 0x3100:
		   printf("ActParLisScanf ");
		   break;
	    case 0x3200:
		   printf("FunctionName ");
		   break;
	    case 0xFFFF:
		   printf("Empty ");
		   break;

		default:
		   if(utermstack.utermstack[i] > 0x0100)
		       printf("Unknow ");
		   else
			   printf("%x ",utermstack.utermstack[i]);
		   break;
	}
  }
  printf("\n");

  return 0;
}

/***********************************************
   函数名：Push()
   功能：  符号入栈
   参数：  unsigned short int uterm 　符号
   返回值：1 成功
           0 失败
************************************************/
int Push(unsigned short int uterm)
{
  if(utermstack.utermstack != NULL)
  {
	  if(utermstack.top == utermstack.stacklen*1024-1 )
	    if( !ResizeStack())
			return 0;
	  utermstack.top ++;
	  utermstack.utermstack[utermstack.top] = uterm ;
	  return 1;
  }
  else
	  return 0;
}

/***********************************************
   函数名：PushGramm()
   功能：  产生式入栈
   参数：  unsigned short int uterm 产生式编号
   返回值：1 成功
           0 失败
************************************************/
int PushGramm(unsigned short int uterm)
{
  //unsigned short int grammar[103][32]
  int i = 0;
  if(uterm >= 103 || utermstack.utermstack == NULL)
	  return 0;
  for(i = 0; i < 32 && grammar[uterm][i] != 0; i++);

  for(i-- ; i >= 0; i--)
     if(!Push(grammar[uterm][i]))
		return 0;

  return 1;
}

/***********************************************
   函数名：Pop()
   功能：  弹出栈顶符号
   参数：  无
   返回值：>0 栈顶符号
           0 栈空
************************************************/
unsigned short int Pop()
{
  unsigned short int t = 0;
  if(utermstack.utermstack != NULL && utermstack.top < utermstack.stacklen*1024 && utermstack.top > 0)
  {
    t = utermstack.utermstack[utermstack.top];
	utermstack.top --;
  }
  return t;
}

/***********************************************
   函数名：ProcessGramm
   功能：  语法分析(处理产生式)
   参数：  int production 产生式编号
   返回值：1 识别成功
           0 失败 错误编号队列中为出错信息
************************************************/
int ProcessGramm()
{
	int i = 0;
    unsigned short term = 0;
	int grammindex = 0;
	int err = 0;

  	if(ResultData.pTokenList != NULL)
	{
	   while( i < ResultData.nTokenListUsedLength )
	   {
		  printf("\n %x" , ResultData.pTokenList[i].typecode);
          PrintStack();
		  term = Pop();  //弹出栈顶符号　
		  if(term == 0)  //非法符号
		      break;
		  else if(term == 0xFFFF) //ε空动作
			  continue;
		  else if( term < 0x0100) //终结符
		  {
			  if( term == ResultData.pTokenList[i].typecode) //当前token字与栈顶符号比较，相同则继续
		         i++;
			  else
			  {
			     //记录出错信息
			     break;
			  }
		  }
		  else
		  {
		    //查找LL(1)分析表
            grammindex = SearchLL1Table(term >> 8 , ResultData.pTokenList[i].typecode);
            //加入冲突消解
			if(grammindex == 21 && i + 1 < ResultData.nTokenListUsedLength)
			{
			   if( ResultData.pTokenList[i+1].typecode == 0x001d) //向右察看一个符号为 = 时 为22
                    grammindex = 22;
			   else if( ResultData.pTokenList[i+1].typecode == 0x0026)//向右察看一个符号不为 ( 时 为23
                    grammindex = 21;
			   else
				    grammindex = 23;
			}
            if(grammindex == 96 && i + 1 < ResultData.nTokenListUsedLength)
			{
			   if( ResultData.pTokenList[i+1].typecode == 0x002d) //向右察看一个符号为 = 时 为22
                    grammindex = 97;

			}

			if(grammindex <=0 || !PushGramm(grammindex))
				break; //非终结符则将相应产生式入栈
		  }
	   }

	   if(i < ResultData.nTokenListUsedLength || !IsStackEmpty())
		   return grammindex;
	   else
		   return 1;
	}
	else
	{
	   return 0;
	}
}

/***********************************************
   函数名：CheckGramm
   功能：  语法分析
   参数：  无
   返回值：1 识别成功
           0 失败 错误编号队列中为出错信息
************************************************/
int CheckGramm()
{
	int errflag = 0;
	InitStack();
	PushGramm(0); //文法开始符号入栈

	errflag = ProcessGramm();

	DestroyStack();

	return errflag;
}

int main(int argc , char* argv[])
{
	char srcname[MAX_PATH] = {0};
	int errcode = 0;
    if( argc > 1)
	{
	  strcpy(srcname, argv[1]);
	}
	else
	{
		printf("Input Source File Name Please:");
		gets(srcname);
	}

	if(!InitResultData())
	{
		printf("Alloc Memory Fail\n");
		return 0;
	}

	if( Scaner(srcname))
		printf("Compile Success!\n");
	else
		printf("Compile Failed!\n");

	errcode = CheckGramm();

	if( errcode > 0)
		printf("Grammar check Success!\n");
	else if(errcode < 0 )
		printf("Grammar &d check Failed!\n" , -errcode);
	else
		printf("Token String is Empty!\n");

	DeleteResultData();
	getchar();

	return 0;
}
