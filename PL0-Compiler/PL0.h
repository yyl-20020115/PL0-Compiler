#pragma once

/*PL/0 ����ϵͳC�汾ͷ�ļ� pl0.h*/

# define NKEYWORDS 19                 /*�ؼ��ָ���*/ //�ؼ��ָ������ɳ�ʼֵ13������19
# define NAME_TABLE_SIZE 100               /*���ֱ�����*/
# define MAX_DIGITS  14                /*number�����λ��*/
# define MAX_SYMBOL_CHARS 10                   /*���ŵ���󳤶�*/
# define MAX_ADDRESS 2047               /*��ַ�Ͻ�*/
# define MAX_NEST_CALLING_LEVEL 3                /*����������Ƕ����������[0��lexmax]*/
# define MAX_PCODE_LENGTH 200               /*���������������*/
/*����*/
//���ӵ���:������ ELSE��FOR��TO��DOWNTO��RETURN
//����� *=��/=��++��--
enum class symbol
{
	nul, ident, number, plus, minus,
	times, slash, oddsym, eql, neq,
	lss, leq, gtr, geq, lparen,
	rparen, comma, semicolon, period, becomes,
	beginsym, endsym, ifsym, thensym, whilesym,
	writesym, readsym, dosym, callsym, constsym,
	varsym, procsym, elsesym, forsym, tosym, downtosym,
	returnsym, timeseql, slasheql, plusplus, minusminus,
	charsym,
};
#define NSYMBOLS 42    //��������ʼֵ��32������42
/*-------------*/
enum class object {
	constant,
	variable,
	procedur,
	charcon,//�ַ���
	realcon,//ʵ��
};
/*--------------*/
enum class fct {
	lit, opr, lod, sto, cal, inte, jmp, jpc,
};
#define fctnum 8
/*--------------*/
struct instruction
{
	enum fct f;
	int l;
	int a;
	//double a;//ʵ���ַ����޸�
};

FILE* f_table;
FILE* f_assembly;
FILE* f_locations;
FILE* f_result;

bool tableswitch;
bool listswitch;
char ch;
enum symbol sym;
char id[MAX_SYMBOL_CHARS + 1];
int  num;
int cc, ll;
int cx;
char line[81];
char a[MAX_SYMBOL_CHARS + 1];
struct instruction code[MAX_PCODE_LENGTH];
char word[NKEYWORDS][MAX_SYMBOL_CHARS];
enum symbol wsym[NKEYWORDS];
enum symbol ssym[256];
char mnemonic[fctnum][5];
bool declbegsys[NSYMBOLS];
bool statbegsys[NSYMBOLS];
bool facbegsys[NSYMBOLS];
/*------------------------------*/
struct tablestruct
{
	char name[MAX_SYMBOL_CHARS];                            /*����*/
	enum object kind;                         /*���ͣ�const��var��array or procedure*/
	int val;                                  /*��ֵ����constʹ��*/
	int level;                                /*�����㣬��const��ʹ��*/
	int adr;                                  /*��ַ����const��ʹ��*/
	int size;                                 /*��Ҫ������������ռ䣬��procedureʹ��*/
};
struct tablestruct table[NAME_TABLE_SIZE];             /*���ֱ�*/
FILE* fin;
FILE* fout;
char fname[MAX_SYMBOL_CHARS];
int err;                                       /*���������*/
/*�������лᷢ��fatal errorʱ�����أ�1��֪�������ĺ����������˳�����*/
#define getsymdo                              if(-1==getsym())return -1
#define getchdo                               if(-1==getch())return -1
#define testdo(a,b,c)                         if(-1==test(a,b,c))return -1
#define gendo(a,b,c)                          if(-1==gen(a,b,c))return -1
#define expressiondo(a,b,c)                   if(-1==expression(a,b,c))return -1
#define factordo(a,b,c)                       if(-1==factor(a,b,c))return -1
#define termdo(a,b,c)                         if(-1==term(a,b,c))return -1
#define conditiondo(a,b,c)                    if(-1==condition(a,b,c))return -1
#define statementdo(a,b,c)                    if(-1==statement(a,b,c))return -1
#define constdeclarationdo(a,b,c)             if(-1==constdeclaration(a,b,c))return -1
#define vardeclarationdo(a,b,c)               if(-1==vardeclaration(a,b,c))return -1
#define chardeclarationdo(a,b,c)              if(-1==chardeclaration(a,b,c))return -1
void error(int n);
int getsym();
int getch();
void init();
int gen(enum fct x, int y, int z);
int test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
int block(int lev, int tx, bool* fsys);
void interpret();
int factor(bool* fsys, int* ptx, int lev);
int term(bool* fsys, int* ptx, int lev);
int condition(bool* fsys, int* ptx, int lev);
int expression(bool* fsys, int* ptx, int lev);
int statement(bool* fsys, int* ptx, int lev);
void listcode(int cx0);
int vardeclaration(int* ptx, int lev, int* pdx);
int constdeclaration(int* ptx, int lev, int* pdx);
int chardeclaration(int* ptx, int lev, int* pdx);//�ַ�������
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx);
int base(int l, int* s, int b);
