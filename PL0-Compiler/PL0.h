#pragma once

/*PL/0 编译系统C版本头文件 pl0.h*/

# define NKEYWORDS 19                 /*关键字个数*/ //关键字个数数由初始值13增加至19
# define NAME_TABLE_SIZE 100               /*名字表容量*/
# define MAX_DIGITS  14                /*number的最大位数*/
# define MAX_SYMBOL_CHARS 10                   /*符号的最大长度*/
# define MAX_ADDRESS 2047               /*地址上界*/
# define MAX_NEST_CALLING_LEVEL 3                /*最大允许过程嵌套声明层数[0，lexmax]*/
# define MAX_PCODE_LENGTH 200               /*最多的虚拟机代码数*/
/*符号*/
//增加单词:保留字 ELSE，FOR，TO，DOWNTO，RETURN
//运算符 *=，/=，++，--
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
#define NSYMBOLS 42    //符号数初始值由32增加至42
/*-------------*/
enum class object {
	constant,
	variable,
	procedur,
	charcon,//字符型
	realcon,//实型
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
	//double a;//实型字符型修改
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
	char name[MAX_SYMBOL_CHARS];                            /*名字*/
	enum object kind;                         /*类型：const，var，array or procedure*/
	int val;                                  /*数值，仅const使用*/
	int level;                                /*所处层，仅const不使用*/
	int adr;                                  /*地址，仅const不使用*/
	int size;                                 /*需要分配的数据区空间，仅procedure使用*/
};
struct tablestruct table[NAME_TABLE_SIZE];             /*名字表*/
FILE* fin;
FILE* fout;
char fname[MAX_SYMBOL_CHARS];
int err;                                       /*错误计数器*/
/*当函数中会发生fatal error时，返回－1告知调用它的函数，最终退出程序*/
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
int chardeclaration(int* ptx, int lev, int* pdx);//字符型声明
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx);
int base(int l, int* s, int b);
