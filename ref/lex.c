#include "stdio.h"
#include "string.h"

//状态名表
typedef enum
{
  S_state, A_state, B_state, C_state, D_state, E_state, F_state,
  G_state, H_state, I_state, J_state, K_state, L_state, M_state,
  N_state, O_state, P_state, Q_state, R_state, T_state, U_state,
  V_state, W_state, X_state, Y_state, Z_state, END_state

} STATE;

//单词类别标识
typedef enum
{
  KEY_WORD = 0,     IDENTIFIER,    UNSINED_INT,          REAL_CONST,       SINGLE_DELIMITER,      COMMENT,   DIVISION_SIGN,
  ADDSELF_OPERATOR, ADD_OPERATOR,      SUBTRACTIONSELF_OPERATOR, SUBTRACTION_OPERATOR, DOUBLERELATIONAL_OPERATOR, SINGLERELATIONAL_OPERATOR,
  ERROR,  CHAR_CONST,   STRING_CONST , NONE

} WORDTYPE;

//单词类别名称
char WordTypeExplanation[][30] = { "关键字", "标识符", "无符号整数" ,"实型常量" ,"单界限符", "注释",     "除号",
						    "自增运算符" ,  "加法运算符",   "自减运算符",  "减法运算符或取负运算符",   "双界限关系运算符" ,
							"单界限关系运算符", "出错",  "字符常量", "字符串常量"  , "无可处理字符"
						  };

#define KEYWORD_COUNT 18
#define KEYWORD_MAXLENGTH 20
//关键字表 编号由位置隐含
char KeyWordTable[KEYWORD_COUNT][KEYWORD_MAXLENGTH] = { "break", "case", "char" ,"continue" ,"default", "do",     "double",   "else" , "float",
						                                "for" ,  "if",   "int",  "return",   "struct" , "switch" ,"unsigned" ,"void",  "while"
						                              };

/* GetWordType判断是关键字还是自定义标识符
   参数：char* str 待判断标识符
   返回值：KEY_WORD 关键字   IDENTIFIER 自定义标识符
*/
WORDTYPE GetWordType(char* str)
{
  int i=0;
  for(i = 0;i < KEYWORD_COUNT ; i++)
  {
     if(strcmp(str, KeyWordTable[i]) ==0 )
		 break; /*查找关键字表,此处可用折半查找提高效率*/
  }

  if(i < KEYWORD_COUNT)
	  return KEY_WORD;
  else
	  return IDENTIFIER;
}

/* GetCharactorType() 取得当前字符类别
   返回值： 0：未知字符   1：字母  2：数字   3：下划线 4：e或E（科学记数法） 5：小数点  6：星号   7：百分号   8：斜线  9：冒号
           10：左方括号  11：右方括号   12：逗号  13：空格 14：加号  15：减号   16：等号  17：大于号  18：小于号  19：叹号
		   20：单引号  21：双引号 22：左括号 23：右括号 24：左大括号 25：右大括号
*/
int GetCharactorType(char ch)
{
  if(ch == 'e' || ch == 'E') /* 对字母e 和E 单独处理 因其可能用于实数的科学记数法 */
	  return 4;
  else  if( ch >= 'A' && ch<= 'Z' ||  ch >= 'a' && ch<= 'z')
	  return 1;
  else  if( ch >= '0' && ch<= '9')
	  return 2;
  else if(ch == '_')
	  return 3;
  else if(ch == '.')
	  return 5;
  else if(ch == '*')
	  return 6;
  else if(ch == '%')
	  return 7;
  else if(ch == '/')
	  return 8;
  else if(ch == ':')
	  return 9;
  else if(ch == '[')
	  return 10;
  else if(ch == ']')
	  return 11;
  else if(ch == ',')
	  return 12;
  else if(ch == 32 )   /* 空格 ASCII码值为32*/
	  return 13;
  else if(ch == '+')
	  return 14;
  else if(ch == '-')
	  return 15;
  else if(ch == '=')
	  return 16;
  else if(ch == '>')
	  return 17;
  else if(ch == '<')
	  return 18;
  else if(ch == '!')
	  return 19;
  else if(ch == '\'')/* 单引号*/
	  return 20;
  else if(ch == '\"')/* 双引号*/
	  return 21;
  else if(ch == '(')
	  return 22;
  else if(ch == ')')
	  return 23;
  else if(ch == '{')
	  return 24;
  else if(ch == '}')
	  return 25;


  return 0;
}


/*RecogniteWordByDFA() 直接转向法识别单词类别by DFA; 每次可处理一行
  参数： char *straddr 待识别字符串首地址
         int strlen    待识别字符串总长度
         int* pcurpos  当前处理字符在字符串中的位置游标变量地址
  返回值：WORDTYPE 规定的类别码
*/
WORDTYPE RecogniteWordByDFA(char *straddr,int strlength, int* pcurpos )
{

  STATE state = S_state; /* 初始化当前状态值为 S_state */
  WORDTYPE wordtype = ERROR; /* 单词类别判定结果码，初始为ERROR  */

  int  cur_pos = *pcurpos;
  int  ch_type = 0;
  char tmpstr[256] = {0};

  while(state != END_state)
  {

	switch(state)
	{
	   case S_state:
           if(cur_pos >= strlength )
		   {
			   state = Z_state;
			   break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]); /*取得当前字符类别*/
		   switch(ch_type)
		   {
			case 1: /*字母*/
			case 3: /*下划线*/
			case 4: /*字母e或E*/
				state = A_state;
				tmpstr[cur_pos- *pcurpos] = straddr[cur_pos]; /*记录当前字符,用以在B状态判断是标识符还是关键字*/
				break;

			case 2: /*数字*/
                state = C_state;
				break;

			case 6:/*星号*/
			case 7:/*百分号*/
 			case 9:/*分号*/
			case 10:/*左方括号*/
			case 11:/*右方括号*/
			case 12:/*逗号*/
			case 13:/*空格*/
			case 22:/*左括号*/
			case 23:/*右括号*/
			case 24: /*左大括号*/
			case 25: /*右大括号*/
                state = L_state;
				break;

			case 8: /*斜线*/
                state = M_state;
				break;

			case 14: /*加号*/
                state = Q_state;
				break;

			case 15: /*减号*/
                state = R_state;
				break;

			case 16: /*等号*/
			case 17: /*大于号*/
			case 18: /*小于号*/
                state = T_state;
				break;

			case 19: /*叹号*/
                state = U_state;
				break;

			case 20: /*单引号*/
                state = V_state;
				break;

			case 21: /*双引号*/
                state = X_state;
				break;

			default: /* 其它字符 */
                state = Z_state;
				break;
		   }
	       cur_pos ++;
		   break;

	   case A_state:
		   if(cur_pos >= strlength )
		   {
			   state = B_state;
			   break;
		   }
           ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 1 || ch_type == 2 || ch_type == 3 || ch_type == 4)
		   {
			   state = A_state;
			   tmpstr[cur_pos- *pcurpos] = straddr[cur_pos]; /*记录当前字符,用以在B状态判断是标识符还是关键字*/
	           cur_pos ++;
		   }
		   else
		   {
			   state = B_state;
		   }
		   break;

	   case B_state:

		   wordtype = GetWordType(tmpstr); /*判断是关键字还是标识符*/
		   state = END_state;
		   break;
           /*A_state -- B_state 处理关键字和标识符 */

	   case C_state:
           if(cur_pos >= strlength )
		   {
			   state = D_state;
			   break;
		   }
           ch_type = GetCharactorType(straddr[cur_pos]);
		   if(ch_type == 2) /*数字*/
		   {
			  cur_pos ++;
		      state = C_state;
		   }
		   else if(ch_type == 4) /* e或E */
		   {
			  cur_pos ++;
		      state = H_state;

		   }
		   else
		   {
			  state = D_state;
			  break;
		   }

		   break;
	   case D_state:
		   if(cur_pos >= strlength ) /*所有字符处理完毕*/
		   {
			  wordtype = UNSINED_INT;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);/*此句可以省略*/
		   if(ch_type == 5)/*小数点*/
		   {
			  cur_pos ++;
		      state = F_state;
		   }
		   else /*其它字符*/
		   {
		      wordtype = UNSINED_INT;
			  state = END_state;
		   }

		   break;

	   case F_state:
           if(cur_pos >= strlength ) /*所有字符处理完毕*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
		   if(ch_type == 2)/*数字*/
		   {
			  cur_pos ++;
		      state = G_state;
		   }
		   else /*其它字符*/
		   {
		      wordtype = ERROR;
			  state = END_state;
		   }
		   break;

	   case G_state:
           if(cur_pos >= strlength ) /*所有字符处理完毕*/
		   {
			  wordtype = REAL_CONST ;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*数字*/
		   {
		  	  cur_pos ++;
		      state = G_state;
		   }
		   else if(ch_type == 4) /*E 或 e*/
		   {
		  	  cur_pos ++;
		      state = H_state;
		   }
		   else
		   {
			  wordtype = REAL_CONST ;
			  state = END_state;
		   }

		   break;

	   case H_state: /*处理科学记数法表示的实数中 e或E后面的部分 */

           if(cur_pos >= strlength ) /*所有字符处理完毕, e后面没有字符*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*数字*/
		   {
		  	  cur_pos ++;
		      state = J_state;
		   }
		   else if(ch_type == 14 || ch_type == 15 ) /* + 或 - */
		   {
		  	  cur_pos ++;
		      state = I_state;
		   }
		   else  /* 3.4e  或 3e  后面不是数字也不是正负号 */
		   {
			  wordtype = ERROR;
			  state = END_state;
		   }

		   break;

	   case I_state:
           if(cur_pos >= strlength ) /*所有字符处理完毕, e+-后面没有字符*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*数字*/
		   {
		  	  cur_pos ++;
		      state = J_state;
		   }
		   else  /* e+-后面不是数字*/
		   {
			  wordtype = ERROR;
			  state = END_state;
		   }

		   break;

	   case J_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕,是实常数*/
		   {
			  wordtype = REAL_CONST;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*数字*/
		   {
		  	  cur_pos ++;
		      state = J_state;
		   }
		   else  /* 是实常数*/
		   {
			  wordtype = REAL_CONST;
			  state = END_state;
		   }
		   break;
       /*C_state - J_state 数字（包括 正整数 实数 科学记数的实数）*/

	   case L_state:
           wordtype = SINGLE_DELIMITER;
		   state = END_state;
		   break;

	   case M_state:
           if(cur_pos >= strlength ) /*所有字符处理完毕,是除号*/
		   {
			  wordtype = DIVISION_SIGN;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 6 ) /*星号*/
		   {
		  	  cur_pos ++;
		      state = N_state;
		   }
		   else  /*是除号 */
		   {
			  wordtype = DIVISION_SIGN;
			  state = END_state;
		   }

		   break;

	   case N_state:
           if(cur_pos >= strlength ) /*所有字符处理完毕, ,注释不完整*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type != 6 ) /*不是星号*/
		   {
		  	  cur_pos ++;
		      state = N_state;
		   }
		   else  /*是星号 */
		   {
			  cur_pos ++;
			  state =O_state;
		   }

		   break;

	   case O_state:
           if(cur_pos >= strlength ) /*所有字符处理完毕,注释不完整*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type != 8 ) /*不是斜线*/
		   {
		  	  cur_pos ++;
		      state = N_state;
		   }
		   else  /*是斜线 */
		   {
			  cur_pos ++;
			  wordtype = COMMENT;
			  state = END_state;
		   }

		   break;
	   /* 注释和除号处理完毕*/

	   case Q_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，是加号 */
		   {
			  wordtype = ADD_OPERATOR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 14 ) /* 自增++ */
		   {
		  	  cur_pos ++;
			  wordtype = ADDSELF_OPERATOR;
			  state = END_state;

		   }
		   else  /*是加号 */
		   {
			  wordtype = ADD_OPERATOR;
			  state = END_state;
		   }

		   break;
		   /*加号和自增处理完毕*/

	   case R_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，是减号 */
		   {
			  wordtype = SUBTRACTION_OPERATOR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 15 ) /* 自减-- */
		   {
		  	  cur_pos ++;
			  wordtype = SUBTRACTIONSELF_OPERATOR;
			  state = END_state;

		   }
		   else  /*是减号 */
		   {
			  wordtype = SUBTRACTION_OPERATOR;
			  state = END_state;
		   }

		   break;
		   /*减号和自减处理完毕*/

	   case T_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，是 关系单界运算符  */
		   {
			  wordtype = SINGLERELATIONAL_OPERATOR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 16 ) /*  关系双界运算符 == >=  <= */
		   {
		  	  cur_pos ++;
			  wordtype = DOUBLERELATIONAL_OPERATOR ;
			  state = END_state;

		   }
		   else  /*是 关系单界运算符 */
		   {
			  wordtype = SINGLERELATIONAL_OPERATOR;
			  state = END_state;
		   }

		   break;

	   case U_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，出错  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 16 ) /*  关系双界运算符 ！= */
		   {
		  	  cur_pos ++;
			  wordtype = DOUBLERELATIONAL_OPERATOR ;
			  state = END_state;

		   }
		   else  /* 出错 */
		   {
			  wordtype = ERROR;
			  state = END_state;
		   }

		   break;
		   /* 关系 单\双 界运算符处理完毕*/

	   case V_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，字符常量缺少右边的单引号  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 20 ) /* 两个单引号中间没有字符，出错 */
		   {
		  	  cur_pos ++;
			  wordtype = ERROR;
			  state = END_state;

		   }
		   else  /* 其它字符 */
		   {
			  cur_pos ++;
			  state = W_state;
		   }

		   break;

	   case W_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，字符常量缺少右边的单引号  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 20 ) /* 字符型常量  */
		   {
		  	  cur_pos ++;
			  wordtype = CHAR_CONST;
			  state = END_state;

		   }
		   else  /* 出错，字符型常量中只能有一个字符 */
		   {
			  wordtype = ERROR;
		      state = END_state;
		   }

		   break;

		   /*  字符型常量 处理完毕*/

	   case X_state:

           if(cur_pos >= strlength ) /*所有字符处理完毕，字符串常量缺少右边的双引号  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 21 ) /* 字符串型常量  */
		   {
		  	  cur_pos ++;
			  wordtype = STRING_CONST;
			  state = END_state;

		   }
		   else  /* 其它字符 */
		   {
			  cur_pos ++;
			  state = X_state;
		   }

		   break;
		   /*  字符串型常量 处理完毕*/

	   case Z_state:
	   default:
		   wordtype = ERROR;
		   state = END_state;
		   break;

	}

  }

  *pcurpos = cur_pos ; /* 修改当前行待处理位置游标变量的值 */

  return wordtype;
}

int main()
{
  char teststr[80] = {0};
  char outstr[256] = {0};
  int i=0;
  int curpos = 0 , start = 0;
  WORDTYPE wordtype;
  int strlength = 0;

  puts("Input a program line to test please: ");
  gets(teststr);
  strlength = (int)strlen(teststr);

  while(curpos < strlength)
  {
     start = curpos;
     memset(outstr, 0 , sizeof(char)*256);

     wordtype = RecogniteWordByDFA(teststr ,strlength  , &curpos );
     for(i = start;i<curpos; i++)
		outstr[i-start] = teststr[i];
	 printf("(%s: %s)\n", outstr , WordTypeExplanation[wordtype]);

  }
  getchar();
  return 0;
}
