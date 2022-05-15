#include "stdio.h"
#include "string.h"

//״̬����
typedef enum
{
  S_state, A_state, B_state, C_state, D_state, E_state, F_state,
  G_state, H_state, I_state, J_state, K_state, L_state, M_state,
  N_state, O_state, P_state, Q_state, R_state, T_state, U_state,
  V_state, W_state, X_state, Y_state, Z_state, END_state

} STATE;

//��������ʶ
typedef enum
{
  KEY_WORD = 0,     IDENTIFIER,    UNSINED_INT,          REAL_CONST,       SINGLE_DELIMITER,      COMMENT,   DIVISION_SIGN,
  ADDSELF_OPERATOR, ADD_OPERATOR,      SUBTRACTIONSELF_OPERATOR, SUBTRACTION_OPERATOR, DOUBLERELATIONAL_OPERATOR, SINGLERELATIONAL_OPERATOR,
  ERROR,  CHAR_CONST,   STRING_CONST , NONE

} WORDTYPE;

//�����������
char WordTypeExplanation[][30] = { "�ؼ���", "��ʶ��", "�޷�������" ,"ʵ�ͳ���" ,"�����޷�", "ע��",     "����",
						    "���������" ,  "�ӷ������",   "�Լ������",  "�����������ȡ�������",   "˫���޹�ϵ�����" ,
							"�����޹�ϵ�����", "����",  "�ַ�����", "�ַ�������"  , "�޿ɴ����ַ�"
						  };

#define KEYWORD_COUNT 18
#define KEYWORD_MAXLENGTH 20
//�ؼ��ֱ� �����λ������
char KeyWordTable[KEYWORD_COUNT][KEYWORD_MAXLENGTH] = { "break", "case", "char" ,"continue" ,"default", "do",     "double",   "else" , "float",
						                                "for" ,  "if",   "int",  "return",   "struct" , "switch" ,"unsigned" ,"void",  "while"
						                              };

/* GetWordType�ж��ǹؼ��ֻ����Զ����ʶ��
   ������char* str ���жϱ�ʶ��
   ����ֵ��KEY_WORD �ؼ���   IDENTIFIER �Զ����ʶ��
*/
WORDTYPE GetWordType(char* str)
{
  int i=0;
  for(i = 0;i < KEYWORD_COUNT ; i++)
  {
     if(strcmp(str, KeyWordTable[i]) ==0 )
		 break; /*���ҹؼ��ֱ�,�˴������۰�������Ч��*/
  }

  if(i < KEYWORD_COUNT)
	  return KEY_WORD;
  else
	  return IDENTIFIER;
}

/* GetCharactorType() ȡ�õ�ǰ�ַ����
   ����ֵ�� 0��δ֪�ַ�   1����ĸ  2������   3���»��� 4��e��E����ѧ�������� 5��С����  6���Ǻ�   7���ٷֺ�   8��б��  9��ð��
           10��������  11���ҷ�����   12������  13���ո� 14���Ӻ�  15������   16���Ⱥ�  17�����ں�  18��С�ں�  19��̾��
		   20��������  21��˫���� 22�������� 23�������� 24��������� 25���Ҵ�����
*/
int GetCharactorType(char ch)
{
  if(ch == 'e' || ch == 'E') /* ����ĸe ��E �������� �����������ʵ���Ŀ�ѧ������ */
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
  else if(ch == 32 )   /* �ո� ASCII��ֵΪ32*/
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
  else if(ch == '\'')/* ������*/
	  return 20;
  else if(ch == '\"')/* ˫����*/
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


/*RecogniteWordByDFA() ֱ��ת��ʶ�𵥴����by DFA; ÿ�οɴ���һ��
  ������ char *straddr ��ʶ���ַ����׵�ַ
         int strlen    ��ʶ���ַ����ܳ���
         int* pcurpos  ��ǰ�����ַ����ַ����е�λ���α������ַ
  ����ֵ��WORDTYPE �涨�������
*/
WORDTYPE RecogniteWordByDFA(char *straddr,int strlength, int* pcurpos )
{

  STATE state = S_state; /* ��ʼ����ǰ״ֵ̬Ϊ S_state */
  WORDTYPE wordtype = ERROR; /* ��������ж�����룬��ʼΪERROR  */

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

		   ch_type = GetCharactorType(straddr[cur_pos]); /*ȡ�õ�ǰ�ַ����*/
		   switch(ch_type)
		   {
			case 1: /*��ĸ*/
			case 3: /*�»���*/
			case 4: /*��ĸe��E*/
				state = A_state;
				tmpstr[cur_pos- *pcurpos] = straddr[cur_pos]; /*��¼��ǰ�ַ�,������B״̬�ж��Ǳ�ʶ�����ǹؼ���*/
				break;

			case 2: /*����*/
                state = C_state;
				break;

			case 6:/*�Ǻ�*/
			case 7:/*�ٷֺ�*/
 			case 9:/*�ֺ�*/
			case 10:/*������*/
			case 11:/*�ҷ�����*/
			case 12:/*����*/
			case 13:/*�ո�*/
			case 22:/*������*/
			case 23:/*������*/
			case 24: /*�������*/
			case 25: /*�Ҵ�����*/
                state = L_state;
				break;

			case 8: /*б��*/
                state = M_state;
				break;

			case 14: /*�Ӻ�*/
                state = Q_state;
				break;

			case 15: /*����*/
                state = R_state;
				break;

			case 16: /*�Ⱥ�*/
			case 17: /*���ں�*/
			case 18: /*С�ں�*/
                state = T_state;
				break;

			case 19: /*̾��*/
                state = U_state;
				break;

			case 20: /*������*/
                state = V_state;
				break;

			case 21: /*˫����*/
                state = X_state;
				break;

			default: /* �����ַ� */
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
			   tmpstr[cur_pos- *pcurpos] = straddr[cur_pos]; /*��¼��ǰ�ַ�,������B״̬�ж��Ǳ�ʶ�����ǹؼ���*/
	           cur_pos ++;
		   }
		   else
		   {
			   state = B_state;
		   }
		   break;

	   case B_state:

		   wordtype = GetWordType(tmpstr); /*�ж��ǹؼ��ֻ��Ǳ�ʶ��*/
		   state = END_state;
		   break;
           /*A_state -- B_state ����ؼ��ֺͱ�ʶ�� */

	   case C_state:
           if(cur_pos >= strlength )
		   {
			   state = D_state;
			   break;
		   }
           ch_type = GetCharactorType(straddr[cur_pos]);
		   if(ch_type == 2) /*����*/
		   {
			  cur_pos ++;
		      state = C_state;
		   }
		   else if(ch_type == 4) /* e��E */
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
		   if(cur_pos >= strlength ) /*�����ַ��������*/
		   {
			  wordtype = UNSINED_INT;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);/*�˾����ʡ��*/
		   if(ch_type == 5)/*С����*/
		   {
			  cur_pos ++;
		      state = F_state;
		   }
		   else /*�����ַ�*/
		   {
		      wordtype = UNSINED_INT;
			  state = END_state;
		   }

		   break;

	   case F_state:
           if(cur_pos >= strlength ) /*�����ַ��������*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
		   if(ch_type == 2)/*����*/
		   {
			  cur_pos ++;
		      state = G_state;
		   }
		   else /*�����ַ�*/
		   {
		      wordtype = ERROR;
			  state = END_state;
		   }
		   break;

	   case G_state:
           if(cur_pos >= strlength ) /*�����ַ��������*/
		   {
			  wordtype = REAL_CONST ;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*����*/
		   {
		  	  cur_pos ++;
		      state = G_state;
		   }
		   else if(ch_type == 4) /*E �� e*/
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

	   case H_state: /*�����ѧ��������ʾ��ʵ���� e��E����Ĳ��� */

           if(cur_pos >= strlength ) /*�����ַ��������, e����û���ַ�*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*����*/
		   {
		  	  cur_pos ++;
		      state = J_state;
		   }
		   else if(ch_type == 14 || ch_type == 15 ) /* + �� - */
		   {
		  	  cur_pos ++;
		      state = I_state;
		   }
		   else  /* 3.4e  �� 3e  ���治������Ҳ���������� */
		   {
			  wordtype = ERROR;
			  state = END_state;
		   }

		   break;

	   case I_state:
           if(cur_pos >= strlength ) /*�����ַ��������, e+-����û���ַ�*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*����*/
		   {
		  	  cur_pos ++;
		      state = J_state;
		   }
		   else  /* e+-���治������*/
		   {
			  wordtype = ERROR;
			  state = END_state;
		   }

		   break;

	   case J_state:

           if(cur_pos >= strlength ) /*�����ַ��������,��ʵ����*/
		   {
			  wordtype = REAL_CONST;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 2 ) /*����*/
		   {
		  	  cur_pos ++;
		      state = J_state;
		   }
		   else  /* ��ʵ����*/
		   {
			  wordtype = REAL_CONST;
			  state = END_state;
		   }
		   break;
       /*C_state - J_state ���֣����� ������ ʵ�� ��ѧ������ʵ����*/

	   case L_state:
           wordtype = SINGLE_DELIMITER;
		   state = END_state;
		   break;

	   case M_state:
           if(cur_pos >= strlength ) /*�����ַ��������,�ǳ���*/
		   {
			  wordtype = DIVISION_SIGN;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 6 ) /*�Ǻ�*/
		   {
		  	  cur_pos ++;
		      state = N_state;
		   }
		   else  /*�ǳ��� */
		   {
			  wordtype = DIVISION_SIGN;
			  state = END_state;
		   }

		   break;

	   case N_state:
           if(cur_pos >= strlength ) /*�����ַ��������, ,ע�Ͳ�����*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type != 6 ) /*�����Ǻ�*/
		   {
		  	  cur_pos ++;
		      state = N_state;
		   }
		   else  /*���Ǻ� */
		   {
			  cur_pos ++;
			  state =O_state;
		   }

		   break;

	   case O_state:
           if(cur_pos >= strlength ) /*�����ַ��������,ע�Ͳ�����*/
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type != 8 ) /*����б��*/
		   {
		  	  cur_pos ++;
		      state = N_state;
		   }
		   else  /*��б�� */
		   {
			  cur_pos ++;
			  wordtype = COMMENT;
			  state = END_state;
		   }

		   break;
	   /* ע�ͺͳ��Ŵ������*/

	   case Q_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ��ǼӺ� */
		   {
			  wordtype = ADD_OPERATOR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 14 ) /* ����++ */
		   {
		  	  cur_pos ++;
			  wordtype = ADDSELF_OPERATOR;
			  state = END_state;

		   }
		   else  /*�ǼӺ� */
		   {
			  wordtype = ADD_OPERATOR;
			  state = END_state;
		   }

		   break;
		   /*�Ӻź������������*/

	   case R_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ��Ǽ��� */
		   {
			  wordtype = SUBTRACTION_OPERATOR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 15 ) /* �Լ�-- */
		   {
		  	  cur_pos ++;
			  wordtype = SUBTRACTIONSELF_OPERATOR;
			  state = END_state;

		   }
		   else  /*�Ǽ��� */
		   {
			  wordtype = SUBTRACTION_OPERATOR;
			  state = END_state;
		   }

		   break;
		   /*���ź��Լ��������*/

	   case T_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ��� ��ϵ���������  */
		   {
			  wordtype = SINGLERELATIONAL_OPERATOR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 16 ) /*  ��ϵ˫������� == >=  <= */
		   {
		  	  cur_pos ++;
			  wordtype = DOUBLERELATIONAL_OPERATOR ;
			  state = END_state;

		   }
		   else  /*�� ��ϵ��������� */
		   {
			  wordtype = SINGLERELATIONAL_OPERATOR;
			  state = END_state;
		   }

		   break;

	   case U_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ�����  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 16 ) /*  ��ϵ˫������� ��= */
		   {
		  	  cur_pos ++;
			  wordtype = DOUBLERELATIONAL_OPERATOR ;
			  state = END_state;

		   }
		   else  /* ���� */
		   {
			  wordtype = ERROR;
			  state = END_state;
		   }

		   break;
		   /* ��ϵ ��\˫ ��������������*/

	   case V_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ��ַ�����ȱ���ұߵĵ�����  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 20 ) /* �����������м�û���ַ������� */
		   {
		  	  cur_pos ++;
			  wordtype = ERROR;
			  state = END_state;

		   }
		   else  /* �����ַ� */
		   {
			  cur_pos ++;
			  state = W_state;
		   }

		   break;

	   case W_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ��ַ�����ȱ���ұߵĵ�����  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 20 ) /* �ַ��ͳ���  */
		   {
		  	  cur_pos ++;
			  wordtype = CHAR_CONST;
			  state = END_state;

		   }
		   else  /* �����ַ��ͳ�����ֻ����һ���ַ� */
		   {
			  wordtype = ERROR;
		      state = END_state;
		   }

		   break;

		   /*  �ַ��ͳ��� �������*/

	   case X_state:

           if(cur_pos >= strlength ) /*�����ַ�������ϣ��ַ�������ȱ���ұߵ�˫����  */
		   {
			  wordtype = ERROR;
			  state = END_state;
			  break;
		   }

		   ch_type = GetCharactorType(straddr[cur_pos]);
           if(ch_type == 21 ) /* �ַ����ͳ���  */
		   {
		  	  cur_pos ++;
			  wordtype = STRING_CONST;
			  state = END_state;

		   }
		   else  /* �����ַ� */
		   {
			  cur_pos ++;
			  state = X_state;
		   }

		   break;
		   /*  �ַ����ͳ��� �������*/

	   case Z_state:
	   default:
		   wordtype = ERROR;
		   state = END_state;
		   break;

	}

  }

  *pcurpos = cur_pos ; /* �޸ĵ�ǰ�д�����λ���α������ֵ */

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
