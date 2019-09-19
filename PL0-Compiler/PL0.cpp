﻿/*
A.2 C 版   本
*使用方法:
*运行后输入PL/0 源程序文件名
*回答是否输出虚拟机代码
*回答是否输出名字表
*assembly.txt 输出虚拟机代码
*locations.txt  输出源文件及其各行对应的首地址
*result.txt  输出结果
*table.txt  输出名字表
*     @author:zzh
*/
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include "pl0.h"
#include "string.h"
/*解释执行时使用的栈*/

#define STACK_SIZE 500

int main()
{
	bool nxtlev[SYMBOLS_COUNT] = { false };
	printf("Input pl/0 file ?");
	scanf("%s", fname);                                     /*输入文件名*/
	fin = fopen(fname, "r");
	if (fin)
	{
		//printf("List object code ?(Y/N)");                /*是否输出虚拟机代码*/
		//scanf("%s", fname);
		//listswitch = (fname[0] == 'y' || fname[0] == 'Y');
		//printf("List symbol table ? (Y/N)");             /*是否输出名字表*/
		//scanf("%s", fname);
		//tableswitch = (fname[0] == 'y' || fname[0] == 'Y');
		listswitch = true;
		tableswitch = true;
		f_locations = fopen("locations.txt", "w");
		fprintf(f_locations, "%s\n", fname);
		init(); /*初始化*/
		err = 0;
		cc = cx = ll = 0;
		ch = ' ';
		if (-1 != getsym())
		{
			f_assembly = fopen("assembly.txt", "w");
			f_table = fopen("table.txt", "w");
			addset(nxtlev, declbegsys, statbegsys, SYMBOLS_COUNT);
			nxtlev[(int)symbol::period] = true;
			if (-1 == block(0, 0, nxtlev))  //compile program
			{
				fclose(f_assembly);
				fclose(f_locations);
				fclose(f_table);
				fclose(fin);
				printf("\n");
				return 0;
			}
			fclose(f_assembly);
			fclose(f_locations);
			fclose(f_table);
			if (sym != symbol::period)
			{
				error(9);
			}
			if (err == 0)
			{
				f_result = fopen("result.txt", "w");
				//run program
				interpret();
				fclose(f_result);
			}
			else
			{
				printf("Errors in pl/0 program");
			}
		}
		fclose(fin);
	}
	else
	{
		printf("Can't open file! \n");
	}
	printf("\n");
	getchar();
	getchar();
	return 0;
}
/*
*初始化
*/
void init()
{
	int i;
	for (i = 0; i <= 255; i++)
	{
		ssym[i] = symbol::nul;
	}
	ssym['+'] = symbol::plus;
	ssym['-'] = symbol::minus;
	ssym['*'] = symbol::times;
	ssym['/'] = symbol::slash;
	ssym['('] = symbol::lparen;
	ssym[')'] = symbol::rparen;
	ssym['='] = symbol::eql;
	ssym[','] = symbol::comma;
	ssym['.'] = symbol::period;
	//ssym['#'] = symbol::neq;  //去掉原先单字符‘#’的设置部分,使用 "<>" 作为不等号
	ssym[';'] = symbol::semicolon;
	/*设置保留字名字,按照字母顺序,便于折半查找*/
	strcpy(&(word[0][0]), "begin");
	strcpy(&(word[1][0]), "call");
	strcpy(&(word[2][0]), "char");/*增加保留字char*/
	strcpy(&(word[3][0]), "const");
	strcpy(&(word[4][0]), "do");
	strcpy(&(word[5][0]), "downto");/*增加保留字downto*/
	strcpy(&(word[6][0]), "else");/*增加保留字else*/
	strcpy(&(word[7][0]), "end");
	strcpy(&(word[8][0]), "for");/**增加保留字for*/
	strcpy(&(word[9][0]), "if");
	strcpy(&(word[10][0]), "odd");
	strcpy(&(word[11][0]), "procedure");
	strcpy(&(word[12][0]), "read");
	strcpy(&(word[13][0]), "return");/*增加保留字return*/
	strcpy(&(word[14][0]), "then");
	strcpy(&(word[15][0]), "to");/*增加保留字to*/
	strcpy(&(word[16][0]), "var");
	strcpy(&(word[17][0]), "while");
	strcpy(&(word[18][0]), "write");
	/*设置保留字符号*/
	wsym[0] = symbol::beginsym;
	wsym[1] = symbol::callsym;
	wsym[2] = symbol::charsym;/*增加保留字符号charsym*/
	wsym[3] = symbol::constsym;
	wsym[4] = symbol::dosym;
	wsym[5] = symbol::downtosym;/*增加保留字符号downtosym*/
	wsym[6] = symbol::elsesym;/*增加保留字符号elsesym*/
	wsym[7] = symbol::endsym;
	wsym[8] = symbol::forsym;/*增加保留字符号forsym*/
	wsym[9] = symbol::ifsym;
	wsym[10] = symbol::oddsym;
	wsym[11] = symbol::procsym;
	wsym[12] = symbol::readsym;
	wsym[13] = symbol::returnsym;/*增加保留字符号returnsym*/
	wsym[14] = symbol::thensym;
	wsym[15] = symbol::tosym;/*增加保留字符号tosym*/
	wsym[16] = symbol::varsym;
	wsym[17] = symbol::whilesym;
	wsym[18] = symbol::writesym;

	/*设置指令名称*/
	strcpy(&(mnemonic[(int)fct::lit][0]), "lit");
	strcpy(&(mnemonic[(int)fct::opr][0]), "opr");
	strcpy(&(mnemonic[(int)fct::lod][0]), "lod");
	strcpy(&(mnemonic[(int)fct::sto][0]), "sto");
	strcpy(&(mnemonic[(int)fct::cal][0]), "cal");
	strcpy(&(mnemonic[(int)fct::inte][0]), "int");
	strcpy(&(mnemonic[(int)fct::jmp][0]), "jmp");
	strcpy(&(mnemonic[(int)fct::jpc][0]), "jpc");

	/*设置符号集*/
	for (i = 0; i < SYMBOLS_COUNT; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
	}

	/*设置声明开始符号集*/
	declbegsys[(int)symbol::constsym] = true;
	declbegsys[(int)symbol::varsym] = true;
	declbegsys[(int)symbol::procsym] = true;
	declbegsys[(int)symbol::charsym] = true;//增加声明开始符号charsym
	/*设置语句开始符号集*/
	statbegsys[(int)symbol::beginsym] = true;
	statbegsys[(int)symbol::callsym] = true;
	statbegsys[(int)symbol::ifsym] = true;
	statbegsys[(int)symbol::whilesym] = true;
	/*设置因子开始符号集*/
	facbegsys[(int)symbol::ident] = true;
	facbegsys[(int)symbol::number] = true;
	facbegsys[(int)symbol::lparen] = true;
	facbegsys[(int)symbol::plusplus] = true;//增加开始因子plusplus
	facbegsys[(int)symbol::minusminus] = true;//增加开始因子minusminus
	facbegsys[(int)symbol::charsym] = true;//增加开始因子charsym
}
/*
 *用数组实现集合的集合运算
 */
int inset(int e, bool* s)
{
	return s[e];
}
int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] || s2[i];
	}
	return 0;
}
int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] && (!s2[i]);
	}
	return 0;
}
int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] && s2[i];
	}
	return 0;
}
/*
 *出错处理，打印出错位置和错误编码
 */
void error(int n)
{
	char space[81];
	memset(space, 32, 81); printf("-------%c\n", ch);
	space[cc - 1] = 0;//出错时当前符号已经读完，所以cc-1
	printf("ERROR FOUND:****%s!%d\n", space, n);
	err++;
}
/*
 *  漏掉空格，读取一个字符
 *
 *  每次读一行，存入line缓冲区，line被getsym取空后再读一行
 *
 *  被函数getsym调用
 */
int getch()
{
	if (cc == ll)
	{
		if (feof(fin))
		{
			printf("program incomplete");
			return -1;
		}
		ll = 0;
		cc = 0;
		printf("%d ", cx);
		fprintf(f_locations, "%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
			//fscanf(fin,"%c",&ch)
			if (EOF == fscanf(fin, "%c", &ch))
			{
				line[ll] = 0;
				break;
			}
			printf("%c", ch);
			fprintf(f_locations, "%c", ch);
			line[ll] = ch;
			ll++;
		}
		printf("\n");
		fprintf(f_locations, "\n");
	}
	ch = line[cc];
	cc++;
	return 0;
}
/*词法分析，获取一个符号
*/

int getsym()
{
	int i, j, k;
	while (ch == ' ' || ch == 10 || ch == 9)
	{
		if(-1==getch())return -1;
	}
	if (ch >= 'a' && ch <= 'z')
	{
		k = 0;
		do {
			if (k < MAX_SYMBOL_CHARS)
			{
				a[k] = ch;
				k++;
			}
			if(-1==getch())return -1;
		} while (ch >= 'a' && ch <= 'z' || ch >= '0' && ch <= '9');
		a[k] = 0;
		strcpy(id, a);
		i = 0;
		j = NKEYWORDS - 1;
		do {
			k = (i + j) / 2;
			if (strcmp(id, word[k]) <= 0)
			{
				j = k - 1;
			}
			if (strcmp(id, word[k]) >= 0)
			{
				i = k + 1;
			}

		} while (i <= j);
		if (i - 1 > j)
		{
			sym = wsym[k];
		}
		else
		{
			sym = symbol::ident;
		}
	}
	else
	{
		if (ch >= '0' && ch <= '9')
		{
			k = 0;
			num = 0;
			sym = symbol::number;
			do {
				num = 10 * num + ch - '0';
				k++;
				if(-1==getch())return -1;
			} while (ch >= '0' && ch <= '9'); /*获取数字的值*/
			k--;
			if (k > MAX_DIGITS)
			{
				error(30);
			}
		}
		else
		{
			if (ch == ':')             /*检测赋值符号*/
			{
				if(-1==getch())return -1;
				if (ch == '=')
				{
					sym = symbol::becomes;
					if(-1==getch())return -1;
				}
				else
				{
					sym = symbol::nul;            /*不能识别的符号*/
				}
			}
			else
			{
				if (ch == '<')         /*检测小于或小于等于符号*/
				{
					if(-1==getch())return -1;
					if (ch == '=')
					{
						sym = symbol::leq;
						if(-1==getch())return -1;
					}
					//增加不等号语法分析
					else if (ch == '>')//小于号后面跟着大于号
					{
						sym = symbol::neq;//构成不等号<>
						printf("识别出<>号");
						if(-1==getch())return -1;
					}
					else
					{
						sym = symbol::lss;
					}
				}
				else
				{
					if (ch == '>')          /*检测大于或大于等于符号*/
					{
						if(-1==getch())return -1;
						if (ch == '=')
						{
							sym = symbol::geq;
							if(-1==getch())return -1;
						}
						else
						{
							sym = symbol::gtr;
						}
					}
					else if (ch == '*')
					{
						if(-1==getch())return -1;
						if (ch == '=')
						{
							sym = symbol::timeseql;//构成*=号
							printf("识别出*=号\n");
							if(-1==getch())return -1;
						}
						else
						{
							sym = symbol::times;
						}
					}
					else if (ch == '/')
					{
						if(-1==getch())return -1;
						if (ch == '=')
						{
							sym = symbol::slasheql;//构成/=号
							printf("识别出/=号\n");
							if(-1==getch())return -1;
						}
						else if (ch == '*')//增加注释功能
						{
							int end = 1;
							char a = 0, b = 0;
							if(-1==getch())return -1;
							printf("识别出注释/*%c", ch);
							while (end)
							{
								if (b != 0) { b = a; }
								else b = ch;
								if(-1==getch())return -1;
								a = ch;

								printf("%c", a);
								if (b == '*' && a == '/')
								{
									printf("注释到此结束\n");
									end = 0;
									break;
								}
							}
							if(-1==getch())return -1;
						}
						else
						{
							sym = symbol::slash;
						}
					}
					else if (ch == '+')
					{
						if(-1==getch())return -1;
						if (ch == '+')
						{
							sym = symbol::plusplus;//构成++号
							printf("识别出++号\n");
							if(-1==getch())return -1;
						}
						else
						{
							sym = symbol::plus;
						}
					}
					else if (ch == '-')
					{
						if(-1==getch())return -1;
						if (ch == '-')
						{
							sym = symbol::minusminus;//构成--号
							printf("识别出--号\n");
							if(-1==getch())return -1;
						}
						else
						{
							sym = symbol::minus;
						}
					}
					else if (ch == '\'')//增加字符型
					{
						if(-1==getch())return -1;
						if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
						{
							num = (int)ch;
							if(-1==getch())return -1;
							if (ch == '\'')
								sym = symbol::charsym;
							else
							{
								num = 0;
								sym = symbol::nul;
								error(39);  //类型错误                  
							}
						}
						else error(39); //类型不匹配
						if(-1==getch())return -1;
					}
					else
					{
						sym = ssym[ch];/* 当符号不满足上述条件时，全部按照单字符号处理*/
						//if(-1==getch())return -1;
						//richard
						if (sym != symbol::period)
						{
							if(-1==getch())return -1;
						}
						//end richard
					}
				}
			}
		}
	}
	return 0;
}
/*
*生成虚拟机代码
*
*x:instruction.f;
*y:instruction.l;
*z:instruction.a;
*/
int gen(enum fct x, int y, int z)
{
	if (cx >= MAX_PCODE_LENGTH)
	{
		printf("Program too long"); /*程序过长*/
		return -1;
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx++;
	return 0;
}
/*
*测试当前符号是否合法
*
*在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集合
*（该部分的后跟符号） test 负责这项检测，并且负责当检测不通过时的补救措施
*程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
*符号），以及不通过时的错误号
*
*S1：我们需要的符号
*s2:如果不是我们需要的，则需要一个补救用的集合
*n:错误号
*/


int test(bool* s1, bool* s2, int n)
{
	if (!inset((int)sym, s1))
	{
		error(n);
		/*当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合*/
		while ((!inset((int)sym, s1)) && (!inset((int)sym, s2)))
		{
			getsymdo;
		}
	}
	return 0;
}
/*
 *编译程序主体
 *
 *lev:当前分程序所在层
 *tx:名字表当前尾指针
 *fsys:当前模块后跟符号集合
 */
int block(int lev, int tx, bool* fsys)
{
	int i;
	int dx;                         /*名字分配到的相对地址*/
	int tx0;                        /*保留初始tx*/
	int cx0;                        /*保留初始cx*/
	bool nxtlev[SYMBOLS_COUNT];            /*在下级函数的参数中，符号集合均为值参，但由于使用数组
									  实现，传递进来的是指针，为防止下级函数改变上级函数的
									  集合，开辟新的空间传递给下级函数*/
	dx = 3;
	tx0 = tx;                         /*记录本层名字的初始位置*/
	table[tx].adr = cx;
	gendo(fct::jmp, 0, 0);
	if (lev > MAX_NEST_CALLING_LEVEL)
	{
		error(32);
	}
	do {
		if (sym == symbol::constsym)         /*收到常量声明符号，开始处理常量声明*/
		{
			getsymdo;
			do {
				constdeclarationdo(&tx, lev, &dx);   /*dx的值会被constdeclaration改变，使用
													 指针*/
				while (sym == symbol::comma)
				{
					getsymdo;
					constdeclarationdo(&tx, lev, &dx);
				}
				if (sym == symbol::semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5); /*漏掉了逗号或者分号*/
				}
			} while (sym == symbol::ident);
		}
		if (sym == symbol::varsym)/*收到变量声名符号，开始处理变量声名*/
		{
			getsymdo;
			do {
				vardeclarationdo(&tx, lev, &dx);
				while (sym == symbol::comma)
				{
					getsymdo;
					vardeclarationdo(&tx, lev, &dx);
				}
				if (sym == symbol::semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			} while (sym == symbol::ident);
		}
		if (sym == symbol::charsym)/*收到符号型声名符号，开始处理符号型声名*/
		{
			getsymdo;
			do {
				chardeclarationdo(&tx, lev, &dx);
				while (sym == symbol::comma)
				{
					getsymdo;
					chardeclarationdo(&tx, lev, &dx);
				}
				if (sym == symbol::semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			} while (sym == symbol::ident);
		}
		while (sym == symbol::procsym)/*收到过程声名符号，开始处理过程声名*/
		{
			getsymdo;
			if (sym == symbol::ident)
			{
				enter(object::procedur, &tx, lev, &dx);/*记录过程名字*/
				getsymdo;
			}
			else
			{
				error(4);/*procedure后应为标识符*/
			}
			if (sym == symbol::semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);/*漏掉了分号*/
			}
			memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
			nxtlev[(int)symbol::semicolon] = true;
			if (-1 == block(lev + 1, tx, nxtlev))
			{
				return -1;/*递归调用*/
			}
			if (sym == symbol::semicolon)
			{
				getsymdo;
				memcpy(nxtlev, statbegsys, sizeof(bool) * SYMBOLS_COUNT);
				nxtlev[(int)symbol::ident] = true;
				nxtlev[(int)symbol::procsym] = true;
				testdo(nxtlev, fsys, 6);
			}
			else
			{
				error(5);                       /*漏掉了分号*/
			}
		}
		memcpy(nxtlev, statbegsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::ident] = true;
		nxtlev[(int)symbol::period] = true;
		testdo(nxtlev, declbegsys, 7);
	} while (inset((int)sym, declbegsys));                /*直到没有声明符号*/
	code[table[tx0].adr].a = cx;                    /*开始生成当前过程代码*/
	table[tx0].adr = cx;                            /*当前过程代码地址*/
	table[tx0].size = dx;                           /*声明部分中每增加一条声明都会给dx增加1,声明部分已经结束,dx就是当前过程数据的size*/

	cx0 = cx;
	gendo(fct::inte, 0, dx);                             /*生成分配内存代码*/
	if (tableswitch)                               /*输出名字表*/
	{
		printf("TABLE:\n");
		if (tx0 + 1 > tx)
		{
			printf("NULL\n");
		}
		for (i = tx0 + 1; i <= tx; i++)
		{
			switch (table[i].kind)
			{
			case object::constant:
				printf("%d const %s", i, table[i].name);
				printf("val=%d\n", table[i].val);
				fprintf(f_table, "%d const %s ", i, table[i].name);
				fprintf(f_table, "val=%d\n", table[i].val);
				break;
			case object::variable:
				printf("%d var %s ", i, table[i].name);
				printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
				fprintf(f_table, "%d var %s ", i, table[i].name);
				fprintf(f_table, "lev=%d addr=%d\n", table[i].level, table[i].adr);
				break;
			case object::procedur:
				printf("%d proc %s ", i, table[i].name);
				printf("lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
				fprintf(f_table, "%d proc %s ", i, table[i].name);
				fprintf(f_table, "lev=%d adr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
				break;
			}
		}
		printf("\n");
	}
	/*语句后跟符号为分号或end*/
	memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);/*每个后跟符号集和都包含上层后跟符号集和，以便补救*/
	nxtlev[(int)symbol::semicolon] = true;
	nxtlev[(int)symbol::endsym] = true;
	statementdo(nxtlev, &tx, lev);
	gendo(fct::opr, 0, 0); /*每个过程出口都要使用的释放数据段命令*/
	memset(nxtlev, 0, sizeof(bool) * SYMBOLS_COUNT); /*分程序没有补救集合*/
	test(fsys, nxtlev, 8);                  /*检测后跟符号正确性*/
	listcode(cx0);                        /*输出代码*/
	return 0;
}
/*
*在名字表中加入一项
*
*k:名字种类const,var or procedure
*ptx:名字表尾指针的指针，为了可以改变名字表尾指针的数值
*lev:名字所在的层次，以后所有的lev都是这样
*pdx:为当前应分配的变量的相对地址，分配后要增加1
*/
void enter(enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id);       /*全局变量id中已存有当前名字的名字*/
	table[(*ptx)].kind = k;
	switch (k)
	{
	case object::constant:                      /*常量名字*/
		if (num > MAX_ADDRESS)
		{
			error(31);
			num = 0;
		}
		table[(*ptx)].val = num;
		break;
	case object::variable:                     /*变量名字*/
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;                          /*过程名字*/
	case object::procedur:
		table[(*ptx)].level = lev;
		break;
	case object::charcon:                       /*字符型名字*/
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;


	}

}
/*
 *查找名字的位置
 *找到则返回在名字表中的位置，否则返回0
 *
 *idt: 要查找的名字
 *tx:：当前名字表尾指针
 */
int position(char* idt, int  tx)
{
	int i;
	strcpy(table[0].name, idt);
	i = tx;
	while (strcmp(table[i].name, idt) != 0)
	{
		i--;
	}
	return i;
}
/*
 *常量声明处理
 */
int constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == symbol::ident)
	{
		getsymdo;
		if (sym == symbol::eql || sym == symbol::becomes)
		{
			if (sym == symbol::becomes)
			{
				error(1);                     /*把=写出成了：=*/
			}
			getsymdo;
			if (sym == symbol::number)
			{
				enter(object::constant, ptx, lev, pdx);
				getsymdo;
			}
			else
			{
				error(2);                  /*常量说明=后应是数字*/
			}
		}
		else
		{
			error(3);                       /*常量说明标识后应是=*/
		}
	}
	else
	{
		error(4);                        /*const后应是标识*/
	}
	return 0;
}
/*
 *
 */
int vardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == symbol::ident)
	{
		enter(object::variable, ptx, lev, pdx);//填写名字表
		getsymdo;
	}
	else
	{
		error(4);
	}
	return 0;
}
/*
 *字符型声明
 */
int chardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == symbol::ident)
	{
		enter(object::charcon, ptx, lev, pdx);//填写名字表
		getsymdo;
	}
	else
	{
		error(4);
	}
	return 0;
}
/*
 *实型声明
 */
 /*int realdeclaration(int * ptx,int lev,int * pdx)
 {
	 if(sym==ident)
	 {
		 enter(realcon,ptx,lev,pdx);//填写名字表
		 getsymdo;
	 }
	 else
	 {
		 error(4);
	 }
	 return 0;
 }
 */


 /*
  *输入目标代码清单
  */
void listcode(int cx0)
{
	int i;
	if (listswitch)
	{
		for (i = cx0; i < cx; i++)
		{
			printf("%d %s %d %d\n", i, mnemonic[(int)code[i].f], code[i].l, code[i].a);
			fprintf(f_assembly, "%d %s %d %d\n", i, mnemonic[(int)code[i].f], code[i].l, code[i].a);
		}
	}
}
/*
*语句处理
*/
int statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2;
	bool nxtlev[SYMBOLS_COUNT];
	if (sym == symbol::ident)
	{
		i = position(id, *ptx);//在符号表中查到该标识符所在位置
		if (i == 0)
		{
			error(11);
		}
		else
		{
			if ((table[i].kind != object::variable) && (table[i].kind != object::charcon))
			{
				error(12);
				i = 0;
			}
			else if (table[i].kind == object::charcon)
			{
				getsymdo;
				if (sym != symbol::becomes) error(13);
				else
				{
					getsymdo;
					if (sym != symbol::charsym) error(32);
					else
					{
						gendo(fct::lit, 0, num);
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}

				}
				getsymdo;//坑：分号
			}
			else
			{
				getsymdo;
				if (sym == symbol::becomes)
				{
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					expressiondo(nxtlev, ptx, lev);
					if (i != 0)//如果不曾出错，i将不为0，i所指为当前语句
							//左部标识符在符号表中的位置 
					{
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == symbol::timeseql) // *=运算 
				{
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					gendo(fct::lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev);
					if (i != 0)
					{
						gendo(fct::opr, 0, 4);
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == symbol::slasheql) // /=运算 
				{
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					gendo(fct::lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev);
					if (i != 0)
					{
						gendo(fct::opr, 0, 5);
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == symbol::plusplus) // 后++运算 
				{
					getsymdo;
					gendo(fct::lod, lev - table[i].level, table[i].adr);//找到变量地址，将其值入栈
					gendo(fct::lit, 0, 1);//将常数1取到栈顶
					if (i != 0)
					{
						gendo(fct::opr, 0, 2);     //执行加操作
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == symbol::minusminus) // 后--运算 
				{
					getsymdo;
					gendo(fct::lod, lev - table[i].level, table[i].adr);//找到变量地址，将其值入栈
					gendo(fct::lit, 0, 1);//将常数1取到栈顶
					if (i != 0)
					{
						gendo(fct::opr, 0, 3);     //执行减操作
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}
				else
				{
					error(13);
				}

			}
		}
	}
	else if (sym == symbol::plusplus) // 前++运算 
	{
		getsymdo;
		if (sym == symbol::ident)
		{
			i = position(id, *ptx);
			if (i == 0) error(11);
			else
			{
				if (table[i].kind != object::variable)
				{
					error(12);
					i = 0;
				}
				else    //++后跟的是变量
				{
					getsymdo;
					gendo(fct::lod, lev - table[i].level, table[i].adr);//找到变量地址，将其值入栈
					gendo(fct::lit, 0, 1);//将常数1取到栈顶
					if (i != 0)
					{
						gendo(fct::opr, 0, 2);     //执行加操作
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}

			}
		}
		else
		{
			error(19);
		}
	}
	else if (sym == symbol::minusminus) // 前--运算 
	{
		getsymdo;
		if (sym == symbol::ident)
		{
			i = position(id, *ptx);
			if (i == 0) error(11);
			else
			{
				if (table[i].kind != object::variable)
				{
					error(12);
					i = 0;
				}
				else  //--后跟的是变量
				{
					getsymdo;
					gendo(fct::lod, lev - table[i].level, table[i].adr);//找到变量地址，将其值入栈
					gendo(fct::lit, 0, 1);//将常数1取到栈顶
					if (i != 0)
					{
						gendo(fct::opr, 0, 3);     //执行减操作
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
				}

			}
		}
		else
		{
			error(19);
		}
	}
	else if (sym == symbol::forsym)      //检测到for语句
	{
		getsymdo;
		if (sym == symbol::ident)
		{
			i = position(id, *ptx);
			if (i == 0) error(11);
			else
			{
				if (table[i].kind != object::variable) //赋值语句中，赋值号左部标识符属性应是变量
				{
					error(12); i = 0;
				}
				else
				{
					getsymdo;
					if (sym != symbol::becomes) error(13);             //赋值语句左部标识符后应是赋值号:=
					else getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					nxtlev[(int)symbol::tosym] = true;                     //后跟符to和downto
					nxtlev[(int)symbol::downtosym] = true;
					expressiondo(nxtlev, ptx, lev);           //处理赋值语句右部的表达式E1
					gendo(fct::sto, lev - table[i].level, table[i].adr);     //保存初值
					switch (sym)
					{
					case symbol::tosym:           //步长为的向上增加
						getsymdo;
						cx1 = cx;       //保存循环开始点
						//将循环判断变量取出放到栈顶
						gendo(fct::lod, lev - table[i].level, table[i].adr);
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);    //处理表达式E2
						nxtlev[(int)symbol::dosym] = true;                         //后跟符do
						expressiondo(nxtlev, ptx, lev);
						/*判断循环变量条件，比如for i:=E1 to E2 do S中，判断i是否小于E2，如小于等于，继续循环，大于的话，跳出循环*/
						gendo(fct::opr, 0, 13);             //生成比较指令，i是否小于等于E2的值
						cx2 = cx;                      //保存循环结束点
						//生成条件跳转指令，跳出循环，跳出的地址未知
						gendo(fct::jpc, 0, 0);
						if (sym == symbol::dosym)               //处理循环体S
						{
							getsymdo;
							statement(fsys, ptx, lev);  //循环体处理
							//增加循环变量步长为
							//将循环变量取出放在栈顶
							gendo(fct::lod, lev - table[i].level, table[i].adr);
							gendo(fct::lit, 0, 1);                            //将步长取到栈顶
							gendo(fct::opr, 0, 2);                            //循环变量加步长
							//将栈顶的值存入循环变量
							gendo(fct::sto, lev - table[i].level, table[i].adr);
							gendo(fct::jmp, 0, cx1);                 //无条件跳转到循环开始点
							/*回填循环结束点的地址，cx为else后语句执行完的位置，它正是前面未定的跳转地址*/
							code[cx2].a = cx;
						}
						else
						{
							error(29);    //for语句中少了do
						}
						break;

					case symbol::downtosym:            //步长为的向下减少
						getsymdo;
						cx1 = cx;            //保存循环开始点
						//将循环判断变量取出放到栈顶                         
						gendo(fct::lod, lev - table[i].level, table[i].adr);
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);      //处理表达式E2
						nxtlev[(int)symbol::dosym] = true;                           //后跟符do
						expressiondo(nxtlev, ptx, lev);
						/*判断循环变量条件，比如for i:=E1 downto E2 do S中，判断i是否大于等于E2，如大于等于，继续循环， 小于的话，跳出循环*/
						gendo(fct::opr, 0, 11);   //生成比较指令，i是否大于等于E2的值
						cx2 = cx;            //保存循环结束点
						//生成条件跳转指令，跳出循环，跳出的地址未知             
						gendo(fct::jpc, 0, 0);
						if (sym == symbol::dosym)     //处理循环体S
						{
							getsymdo;
							statement(fsys, ptx, lev);   //循环体处理
							//增加循环变量步长为
							//将循环变量取出放在栈顶
							gendo(fct::lod, lev - table[i].level, table[i].adr);
							gendo(fct::lit, 0, 1);                 //将步长取到栈顶
							gendo(fct::opr, 0, 3);                 //循环变量加步长
							//将栈顶的值存入循环变量
							gendo(fct::sto, lev - table[i].level, table[i].adr);
							gendo(fct::jmp, 0, cx1);     //无条件跳转到循环开始点
							/*回填循环结束点的地址，cx为else后语句执行完的位置，它正是前面未定的跳转地址*/
							code[cx2].a = cx;
						}
						else
						{
							error(29);//for语句中少了do
						}
						break;
					}
				}
			}
		}
		else
		{
			error(19);         //for语句后跟赋值语句，赋值语句左部是变量，缺少变量
		}
	}

	else
	{
		if (sym == symbol::readsym)
		{
			getsymdo;
			if (sym != symbol::lparen)
			{
				error(34);
			}
			else
			{
				do {
					getsymdo;
					if (sym == symbol::ident)
					{
						i = position(id, *ptx);
					}
					else
					{
						i = 0;
					}
					if (i == 0)
					{
						error(35);
					}
					else if (table[i].kind == object::charcon)//字符型数输入
					{
						gendo(fct::opr, 0, 19);
						gendo(fct::sto, lev - table[i].level, table[i].adr);
					}
					else
					{
						gendo(fct::opr, 0, 16);
						gendo(fct::sto, lev - table[i].level, table[i].adr); /* 储存到变量*/
					}
					getsymdo;
				} while (sym == symbol::comma);    /*一条read语句可读多个变量 */
			}
			if (sym != symbol::rparen)
			{
				error(33);          /* 格式错误，应是右括号*/
				while (!inset((int)sym, fsys))/* 出错补救，直到收到上层函数的后跟符号*/
				{
					getsymdo;
				}
			}
			else
			{
				getsymdo;
			}
		}
		else
		{
			if (sym == symbol::writesym)           /* 准备按照write语句处理，与read类似*/
			{
				getsymdo;
				if (sym == symbol::lparen)
				{
					do {
						getsymdo;
						if (sym == symbol::ident)
						{
							i = position(id, *ptx);
							if (i == 0)
							{
								error(11);          /*过程未找到*/
							}
						}
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
						nxtlev[(int)symbol::rparen] = true;
						nxtlev[(int)symbol::comma] = true;     /* write的后跟符号为）or，*/
						expressiondo(nxtlev, ptx, lev);/* 调用表达式处理，此处与read不同，read为给变量赋值*/
						if (table[i].kind == object::charcon) //字符型输出 
						{
							gendo(fct::opr, 0, 17);
						}
						else gendo(fct::opr, 0, 14);/* 生成输出指令，输出栈顶的值*/
					} while (sym == symbol::comma);
					if (sym != symbol::rparen)
					{
						error(33);/* write()应为完整表达式*/
					}
					else
					{
						getsymdo;
					}
				}
				gendo(fct::opr, 0, 15);        /* 输出换行*/
			}
			else
			{
				if (sym == symbol::callsym)        /* 准备按照call语句处理*/
				{
					getsymdo;
					if (sym != symbol::ident)
					{
						error(14);           /*call后应为标识符*/
					}
					else
					{
						i = position(id, *ptx);
						if (i == 0)
						{
							error(11);          /*过程未找到*/
						}
						else
						{
							if (table[i].kind == object::procedur)
							{
								gendo(fct::cal, lev - table[i].level, table[i].adr);  /*生成call指令*/
							}
							else
							{
								error(15);      /*call后标识符应为过程*/
							}
						}
						getsymdo;
					}
				}
				else
				{
					if (sym == symbol::ifsym)     /*准备按照if语句处理*/
					{
						getsymdo;
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
						nxtlev[(int)symbol::thensym] = true;
						nxtlev[(int)symbol::dosym] = true;    /*后跟符号为then或do*/
						conditiondo(nxtlev, ptx, lev);   /*调用条件处理（逻辑运算）函数*/
						if (sym == symbol::thensym)
						{
							getsymdo;
						}
						else
						{
							error(16);          /*缺少then*/
						}
						cx1 = cx;                /*保存当前指令地址*/
						gendo(fct::jpc, 0, 0);        /*生成条件跳转指令，跳转地址暂写0*/
						statementdo(fsys, ptx, lev);   /*处理then后的语句*/

						//if(sym==semicolon)
						//{
						//  getsymdo;
						//}
						if (sym == symbol::elsesym)/*than语句后面发现else*/
						{
							getsymdo;
							cx2 = cx;
							code[cx1].a = cx + 1;/*cx为当前的指令地址，
											  cx+1即为then语句执行后的else语句的位置，回填地址*/
							gendo(fct::jmp, 0, 0);
							statementdo(fsys, ptx, lev);
							code[cx2].a = cx;     /*经statement处理后，cx为else后语句执行
												完的位置，它正是前面未定的跳转地址，回填地址*/
						}
						else
						{

							code[cx1].a = cx;      /*经statement处理后，cx为then后语句执行
												   完的位置，它正是前面未定的跳转地址*/
						}
					}
					else
					{
						if (sym == symbol::beginsym)   /*准备按照复合语句处理*/
						{
							getsymdo;
							memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
							nxtlev[(int)symbol::semicolon] = true;
							nxtlev[(int)symbol::endsym] = true;/*后跟符号为分号或end*/
							/*循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end*/
							statementdo(nxtlev, ptx, lev);
							while (inset((int)sym, statbegsys) || sym == symbol::semicolon)
							{
								if (sym == symbol::semicolon)
								{
									getsymdo;
								}
								else
								{
									error(10);/*缺少分号*/
								}
								statementdo(nxtlev, ptx, lev);
							}
							if (sym == symbol::endsym)
							{
								getsymdo;
							}
							else
							{
								error(17); /*缺少end或分号*/
							}
						}
						else
						{
							if (sym == symbol::whilesym)/*准备按照while语句处理*/
							{
								cx1 = cx;        /*保存判断条件超作的位置*/
								getsymdo;
								memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
								nxtlev[(int)symbol::dosym] = true;/*后跟符号为do*/
								conditiondo(nxtlev, ptx, lev);  /*调用条件处理*/
								cx2 = cx;       /*保存循环体的结束的下一个位置*/
								gendo(fct::jpc, 0, 0);/*生成条件跳转，但跳出循环的地址未知*/
								if (sym == symbol::dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);      /*缺少do*/
								}
								statementdo(fsys, ptx, lev); /*循环体*/
								gendo(fct::jmp, 0, cx1);/*回头重新判断条件*/
								code[cx2].a = cx;   /*反填跳出循环的地址，与if类似*/
							}
							else
							{
								memset(nxtlev, 0, sizeof(bool) * SYMBOLS_COUNT);/*语句结束无补救集合*/
								testdo(fsys, nxtlev, 19);/*检测语句结束的正确性*/
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
/*
*表达式处理
*/
int expression(bool* fsys, int* ptx, int lev)
{
	enum symbol addop;                    /*用于保存正负号*/
	bool nxtlev[SYMBOLS_COUNT];
	if (sym == symbol::plus || sym == symbol::minus)             /*开头的正负号，此时当前表达式被看作一个正的或负的项*/
	{
		addop = sym;                    /*保存开头的正负号*/
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::plus] = true;
		nxtlev[(int)symbol::minus] = true;
		termdo(nxtlev, ptx, lev);                /*处理项*/
		if (addop == symbol::minus)
		{
			gendo(fct::opr, 0, 1);                   /*如果开头为负号生成取负指令*/
		}
	}
	else                             /*此时表达式被看作项的加减*/
	{
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::plus] = true;
		nxtlev[(int)symbol::minus] = true;
		termdo(nxtlev, ptx, lev);            /*处理项*/
	}
	while (sym == symbol::plus || sym == symbol::minus)
	{
		addop = sym;
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::plus] = true;
		nxtlev[(int)symbol::minus] = true;
		termdo(nxtlev, ptx, lev);              /*处理项*/
		if (addop == symbol::plus)
		{
			gendo(fct::opr, 0, 2);                    /*生成加法指令*/
		}
		else
		{
			gendo(fct::opr, 0, 3);                /*生成减法指令*/
		}
	}
	return 0;
}
/*
*项处理
*/
int term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop;               /*用于保存乘除法符号*/
	bool nxtlev[SYMBOLS_COUNT];
	memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
	nxtlev[(int)symbol::times] = true;
	nxtlev[(int)symbol::slash] = true;
	factordo(nxtlev, ptx, lev);       /*处理因子*/
	while (sym == symbol::times || sym == symbol::slash)
	{
		mulop = sym;
		getsymdo;
		factordo(nxtlev, ptx, lev);
		if (mulop == symbol::times)
		{
			gendo(fct::opr, 0, 4);          /*生成乘法指令*/
		}
		else
		{
			gendo(fct::opr, 0, 5);           /*生成除法指令*/
		}
	}
	return 0;
}
/*
*因子处理
*/
int factor(bool* fsys, int* ptx, int lev)
{
	int i;
	bool nxtlev[SYMBOLS_COUNT];
	testdo(facbegsys, fsys, 24);           /*检测因子的开始符好号*/
	while (inset((int)sym, facbegsys))          /*循环直到不是因子开始符号*/
	{
		if (sym == symbol::ident)                   /*因子为常量或者变量*/
		{
			i = position(id, *ptx);        /*查找名字*/
			if (i == 0)
			{
				error(11);               /*标识符未声明*/
			}
			else
			{
				switch (table[i].kind)
				{
				case object::constant:                                      /*名字为常量*/
					gendo(fct::lit, 0, table[i].val);                       /*直接把常量的值入栈*/
					break;
				case object::variable:                                      /*名字为变量*/
					gendo(fct::lod, lev - table[i].level, table[i].adr);      /*找到变量地址并将其值入栈*/
					break;
				case object::procedur:                                      /*名字为过程*/
					error(21);                                       /*不能为过程*/
					break;
				case object::charcon:                                      /*名字为字符型*/
					gendo(fct::lod, lev - table[i].level, table[i].adr);      /*找到字符型地址并将其值入栈*/
					break;
				}
			}
			getsymdo;

			if (sym == symbol::plusplus)
			{
				gendo(fct::lit, lev - table[i].level, 1);//将值为入栈
				gendo(fct::opr, lev - table[i].level, 2);//+1,栈顶加次栈顶
				gendo(fct::sto, lev - table[i].level, table[i].adr);//出栈取值到内存
				gendo(fct::lod, lev - table[i].level, table[i].adr);//取值到栈顶
				gendo(fct::lit, 0, 1);
				gendo(fct::opr, 0, 3);//栈顶值减
				getsymdo;
			}
			else if (sym == symbol::minusminus)
			{
				gendo(fct::lit, lev - table[i].level, 1);//将值为入栈
				gendo(fct::opr, lev - table[i].level, 3);//-1,栈顶加次栈顶
				gendo(fct::sto, lev - table[i].level, table[i].adr);//出栈取值到内存
				gendo(fct::lod, lev - table[i].level, table[i].adr);//取值到栈顶
				gendo(fct::lit, 0, 1);
				gendo(fct::opr, 0, 2);//栈顶值加
				getsymdo;
			}
		}
		else if (sym == symbol::plusplus)
		{
			getsymdo;
			if (sym == symbol::ident)
			{
				getsymdo;
				i = position(id, *ptx);
				if (i == 0) error(11);
				else
				{
					if (table[i].kind == object::variable)
					{
						gendo(fct::lod, lev - table[i].level, table[i].adr);//找到变量地址，将其值入栈
						gendo(fct::lit, 0, 1);//将常数1取到栈顶
						gendo(fct::opr, 0, 2);     //执行加操作
						gendo(fct::sto, lev - table[i].level, table[i].adr);//出栈取值到内存
						gendo(fct::lod, lev - table[i].level, table[i].adr);//取值到栈顶
					}
				}
			}
		}
		else if (sym == symbol::minusminus)
		{
			getsymdo;
			if (sym == symbol::ident)
			{
				getsymdo;
				i = position(id, *ptx);
				if (i == 0) error(11);
				else
				{
					if (table[i].kind == object::variable)
					{
						gendo(fct::lod, lev - table[i].level, table[i].adr);//找到变量地址，将其值入栈
						gendo(fct::lit, 0, 1);//将常数1取到栈顶
						gendo(fct::opr, 0, 3);     //执行减操作
						gendo(fct::sto, lev - table[i].level, table[i].adr);//出栈取值到内存
						gendo(fct::lod, lev - table[i].level, table[i].adr);//取值到栈顶
					}
				}
			}
		}
		else
		{
			if (sym == symbol::number)                                             /*因子为数*/
			{
				if (num > MAX_ADDRESS)
				{
					error(31);
					num = 0;
				}
				gendo(fct::lit, 0, num);
				getsymdo;
			}
			else
			{
				if (sym == symbol::lparen)                                           /*因子为表达式*/
				{
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					nxtlev[(int)symbol::rparen] = true;
					expressiondo(nxtlev, ptx, lev);
					if (sym == symbol::rparen)
					{
						getsymdo;
					}
					else
					{
						error(22);                                       /*缺少右括号*/
					}
				}
				testdo(fsys, facbegsys, 23);                        /*银子后有非法符号*/
			}
		}
	}
	return 0;
}
/*
 条件处理*/
int condition(bool* fsys, int* ptx, int lev)
{
	enum symbol relop;
	bool nxtlev[SYMBOLS_COUNT];
	if (sym == symbol::oddsym)                        /*准备按照odd运算处理*/
	{
		getsymdo;
		expressiondo(fsys, ptx, lev);
		gendo(fct::opr, 0, 6);                              /*生成odd指令*/
	}
	else
	{
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::eql] = true;
		nxtlev[(int)symbol::neq] = true;
		nxtlev[(int)symbol::lss] = true;
		nxtlev[(int)symbol::leq] = true;
		nxtlev[(int)symbol::gtr] = true;
		nxtlev[(int)symbol::geq] = true;
		expressiondo(nxtlev, ptx, lev);
		if (sym != symbol::eql
			&& sym != symbol::neq
			&& sym != symbol::lss
			&& sym != symbol::leq
			&& sym != symbol::gtr
			&& sym != symbol::geq)
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsymdo;
			expressiondo(fsys, ptx, lev);
			switch (relop)
			{
			case symbol::eql:
				gendo(fct::opr, 0, 8);
				break;
			case symbol::neq:
				gendo(fct::opr, 0, 9);
				break;
			case symbol::lss:
				gendo(fct::opr, 0, 10);
				break;
			case symbol::geq:
				gendo(fct::opr, 0, 11);
				break;
			case symbol::gtr:
				gendo(fct::opr, 0, 12);
				break;
			case symbol::leq:
				gendo(fct::opr, 0, 13);
				break;
			}

		}
	}
	return 0;
}
/*解释程序*/

void interpret()
{
	char cm;
	int p, b, t;             /*指令指针，指令基址，栈顶指针*/
	struct instruction i;  /*存放当前指令*/
	int s[STACK_SIZE];      /*栈*/
	//double s[stacksize];  //改成doubule型栈
	printf("start pl0\n");
	t = 0;
	b = 0;
	p = 0;
	s[0] = s[1] = s[2] = 0;
	do {
		i = code[p];         /*读当前指令*/
		p++;
		switch (i.f)
		{
		case fct::lit:        /*将a的值取到栈顶*/
			s[t] = i.a;
			t++;
			break;
		case fct::opr:        /*数字、逻辑运算*/
			switch (i.a)
			{
			case 0:
				t = b;
				p = s[t + 2];
				b = s[t + 1];
				break;
			case 1:
				s[t - 1] = -(s[t - 1]);
				break;
			case 2:
				t--;
				s[t - 1] = s[t - 1] + s[t];
				break;
			case 3:
				t--;
				s[t - 1] = s[t - 1] - s[t];
				break;
			case 4:
				t--;
				s[t - 1] = s[t - 1] * s[t];
				break;
			case 5:
				t--;
				s[t - 1] = s[t - 1] / s[t];
				break;
			case 6:
				s[t - 1] = s[t - 1] % 2;
				break;
			case 8:
				t--;
				s[t - 1] = (s[t - 1] == s[t]);
				break;
			case 9:
				t--;
				s[t - 1] = (s[t - 1] != s[t]);
				break;
			case 10:
				t--;
				s[t - 1] = (s[t - 1] < s[t]);
				break;
			case 11:
				t--;
				s[t - 1] = (s[t - 1] >= s[t]);
				break;
			case 12:
				t--;
				s[t - 1] = (s[t - 1] > s[t]);
				break;
			case 13:
				t--;
				s[t - 1] = (s[t - 1] <= s[t]);
				break;
			case 14://14号操作为输出栈顶值操作
				printf("%d", s[t - 1]);//未修改成实型字符型型前
				fprintf(f_result, "%d", s[t - 1]);

				//printf("%d",(int)s[t-1]);//输出栈顶值，强制转换类型
				//fprintf(fa2,"%lf\n",s[t-1]);//同时打印到文件

				t--;//栈顶下移
				break;
			case 15://15号操作为输出换行操作
				printf("\n");
				fprintf(f_result, "\n");
				break;
			case 16:
				printf("?");
				fprintf(f_result, "?");
				scanf("%d", &(s[t]));
				fprintf(f_result, "%d\n", s[t]);

				//printf("输入整型数:");
				//fprintf(fa2,"输入整型数:");
				//scanf("%lf",&(s[t]));
				//fprintf(fa2,"%lf\n",s[t]);

				t++;
				break;

			case 17://17号操作为输出栈顶值操作
				printf("输出字符:");
				printf("%c\n", s[t - 1]);//输出栈顶值
				fprintf(f_result, "%c\n", s[t - 1]);//同时打印到文件
				t--;//栈顶下移
				break;

				/*case 18://18号操作为输出栈顶值操作
					printf("%lf",s[t-1]);//输出栈顶值
					fprintf(fa2,"%lf\n",s[t-1]);//同时打印到文件
					t--;//栈顶下移?
					break;*/

			case 19://19号操作是接受键盘值输入到栈顶
				printf("输入单字符:");//屏显问号
				fprintf(f_result, "输入单字符:");//同时输出到文件
				getchar();//消除输入的enter
				scanf("%c", &cm);
				s[t] = cm;
				//scanf("%s",sta);
				//s[t]=sta[0];//获得输入

				fprintf(f_result, "%c\n", s[t]);//把用户输入值打印到文件
				t++;//栈顶上移，分配空间
				break;

				/*case 20://20号操作是接受键盘值输入到栈顶
					printf("输入双精度型数：");//屏显问号
					fprintf(fa2,"输入双精度型数：");//同时输出到文件
					scanf("%lf",&(s[t]));//获得输入
					fprintf(fa2,"%lf\n",s[t]);//把用户输入值打印到文件
					t++;//栈顶上移，分配空间
					break;*/


			}
			break;
		case fct::lod:       /*取相对当前过程的数据基地址为ａ的内存的值到栈顶*/
			s[t] = s[base(i.l, s, b) + i.a];
			t++;
			break;
		case fct::sto:       /*栈顶的值存到相对当前过程的数据基地址为ａ的内存*/
			t--;
			s[base(i.l, s, b) + i.a] = s[t];
			break;
		case fct::cal:              /*调用子程序*/
			s[t] = base(i.l, s, b); /*将父过程基地址入栈*/
			s[t + 1] = b;           /*将本过程基地址入栈，此两项用于base函数*/
			s[t + 2] = p;           /*将当前指令指针入栈*/
			b = t;                /*改变基地址指针值为新过程的基地址*/
			p = i.a;              /*跳转*/
			break;
		case fct::inte:             /*分配内存*/
			t += i.a;
			break;
		case fct::jmp:             /*直接跳转*/
			p = i.a;
			break;
		case fct::jpc:              /*条件跳转*/
			t--;
			if (s[t] == 0)
			{
				p = i.a;
			}
			break;
		}
	} while (p != 0);
}
/*通过过程基址求上1层过程的基址*/
int base(int l, int* s, int b)
{
	int b1;
	b1 = b;
	while (l > 0)
	{
		b1 = s[b1];
		l--;
	}
	return b1;
}
