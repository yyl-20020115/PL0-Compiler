#include "PL0-Compiler++.h"

PL0_Compiler::PL0_Compiler()
	: table()
	, mnemonic()
	, ch(0)
	, source_input(nullptr)
	, sym(symbol::nul)
	, ssym()
	, wsym()
	, declbegsys()
	, statbegsys()
	, facbegsys()
	, error_count(0)
	, cc()
	, ll()
	, line()
	, num()
	, a()
	, ci()
	, cx()
	, common_input()
	, common_output()
	, error_output()
{
	this->Reset();
}
PL0_Compiler::~PL0_Compiler()
{
}
void PL0_Compiler::SetSourceInput(std::wistream* source)
{
	this->source_input = source;
}
std::wistream* PL0_Compiler::GetSourceInput()
{
	return this->source_input;
}
void PL0_Compiler::SetCommonInput(std::wistream* common_input)
{
	this->common_input = common_input;
}
std::wistream* PL0_Compiler::GetCommonInput()
{
	return this->common_input;
}
void PL0_Compiler::SetCommonOutput(std::wostream* common_output)
{
	this->common_output = common_output;
}
std::wostream* PL0_Compiler::GetCommonOutput()
{
	return this->common_output;
}
void PL0_Compiler::SetErrorOutput(std::wostream* error_output)
{
	this->error_output = error_output;
}
std::wostream* PL0_Compiler::GetErrorOutput()
{
	return this->error_output;
}
void PL0_Compiler::Reset()
{
	for (int i = 0; i < SYMBOLS_COUNT; i++) {
		this->ssym[i] = symbol::nul;
	}
	for (int i = 0; i < SYMBOLS_COUNT; i++) {
		this->wsym[i] = symbol::nul;
	}
	this->ssym[L'+'] = symbol::plus;
	this->ssym[L'-'] = symbol::minus;
	this->ssym[L'*'] = symbol::times;
	this->ssym[L'/'] = symbol::slash;
	this->ssym[L'('] = symbol::lparen;
	this->ssym[L')'] = symbol::rparen;
	this->ssym[L'='] = symbol::eql;
	this->ssym[L','] = symbol::comma;
	this->ssym[L'.'] = symbol::period;
	//this->ssym[L'#'] = symbol::neq;  //去掉原先单字符‘#’的设置部分,使用 "<>" 作为不等号
	this->ssym[L';'] = symbol::semicolon;
	//设置保留字符号
	this->wsym[0] = symbol::beginsym;
	this->wsym[1] = symbol::callsym;
	this->wsym[2] = symbol::charsym;//增加保留字符号charsym
	this->wsym[3] = symbol::constsym;
	this->wsym[4] = symbol::dosym;
	this->wsym[5] = symbol::downtosym;//增加保留字符号downtosym
	this->wsym[6] = symbol::elsesym;//增加保留字符号elsesym
	this->wsym[7] = symbol::endsym;
	this->wsym[8] = symbol::forsym;//增加保留字符号forsym
	this->wsym[9] = symbol::ifsym;
	this->wsym[10] = symbol::oddsym;
	this->wsym[11] = symbol::procsym;
	this->wsym[12] = symbol::readsym;
	this->wsym[13] = symbol::returnsym;//增加保留字符号returnsym
	this->wsym[14] = symbol::thensym;
	this->wsym[15] = symbol::tosym;//增加保留字符号tosym
	this->wsym[16] = symbol::varsym;
	this->wsym[17] = symbol::whilesym;
	this->wsym[18] = symbol::writesym;
	this->word[0] = L"begin";
	this->word[1] = L"call";
	this->word[2] = L"char";//增加保留字char
	this->word[3] = L"const";
	this->word[4] = L"do";
	this->word[5] = L"downto";//增加保留字downto
	this->word[6] = L"else";//增加保留字else
	this->word[7] = L"end";
	this->word[8] = L"for";//增加保留字for
	this->word[9] = L"if";
	this->word[10] = L"odd";
	this->word[11] = L"procedure";
	this->word[12] = L"read";
	this->word[13] = L"return";//增加保留字return
	this->word[14] = L"then";
	this->word[15] = L"to";//增加保留字to
	this->word[16] = L"var";
	this->word[17] = L"while";
	this->word[19] = L"write";

	this->mnemonic[(int)fct::lit] = L"lit";
	this->mnemonic[(int)fct::opr] = L"opr";
	this->mnemonic[(int)fct::lod] = L"lod";
	this->mnemonic[(int)fct::sto] = L"sto";
	this->mnemonic[(int)fct::cal] = L"cal";
	this->mnemonic[(int)fct::inte] = L"inte";
	this->mnemonic[(int)fct::jmp] = L"jmp";
	this->mnemonic[(int)fct::jpc] = L"jpc";

	for (int i = 0; i < SYMBOLS_COUNT; i++) {
		this->facbegsys[i] = false;
		this->statbegsys[i] = false;
		this->declbegsys[i] = false;
	}

	//设置声明开始符号集
	this->declbegsys[(int)symbol::constsym] = true;
	this->declbegsys[(int)symbol::varsym] = true;
	this->declbegsys[(int)symbol::procsym] = true;
	this->declbegsys[(int)symbol::charsym] = true;//增加声明开始符号charsym
	//设置语句开始符号集
	this->statbegsys[(int)symbol::beginsym] = true;
	this->statbegsys[(int)symbol::callsym] = true;
	this->statbegsys[(int)symbol::ifsym] = true;
	this->statbegsys[(int)symbol::whilesym] = true;
	//设置因子开始符号集
	this->facbegsys[(int)symbol::ident] = true;
	this->facbegsys[(int)symbol::number] = true;
	this->facbegsys[(int)symbol::lparen] = true;
	this->facbegsys[(int)symbol::plusplus] = true;//增加开始因子plusplus
	this->facbegsys[(int)symbol::minusminus] = true;//增加开始因子minusminus
	this->facbegsys[(int)symbol::charsym] = true;//增加开始因子charsym

	this->line.clear();
	this->ch = 0;
	this->cx = 0;
	this->num = 0;
	this->error_count = 0;
	this->id.clear();
	this->a.clear();
}
void PL0_Compiler::Execute()
{
	this->Reset();

	bool nxtlev[SYMBOLS_COUNT] = { false };

	if (-1 != getsym())
	{
		this->addset(nxtlev, declbegsys, statbegsys, SYMBOLS_COUNT);
		
		nxtlev[(int)symbol::period] = true;

		if (-1 == compile(0, 0, nxtlev))  //compile program
		{
			this->common_print(L"Failed to compile program!\n");
			return;
		}

		if (sym != symbol::period)
		{
			this->error(9);
		}
		if (this->error_count == 0)
		{
			//run program
			this->interpret();
		}
		else
		{
			this->error_print(L"Errors in pl/0 program");
		}
	}
}
bool PL0_Compiler::inset(int e, bool* s)
{
	return s[e];
}
void PL0_Compiler::addset(bool* sr, bool* s1, bool* s2, int n)
{
	for (int i = 0; i < n; i++)
	{
		sr[i] = s1[i] || s2[i];
	}
}
void PL0_Compiler::subset(bool* sr, bool* s1, bool* s2, int n)
{
	for (int i = 0; i < n; i++)
	{
		sr[i] = s1[i] && (!s2[i]);
	}
}
void PL0_Compiler::mulset(bool* sr, bool* s1, bool* s2, int n)
{
	for (int i = 0; i < n; i++)
	{
		sr[i] = s1[i] && s2[i];
	}
}
void PL0_Compiler::error(int n)
{
	if (this->error_output != nullptr)
	{
		*this->error_output << L"error(" << n << L") = '" << ch << L"'" << std::endl;
	}
	this->error_count++;
}
//  漏掉空格，读取一个字符
//  每次读一行，存入line缓冲区，line被getsym取空后再读一行
//  被函数getsym调用
int PL0_Compiler::getch()
{
	if (this->cc == this->ll)
	{
		if (this->source_input->eof())
		{
			this->common_print(L"Program terminated");
			return -1;
		}
		this->ll = 0;
		this->cc = 0;
		this->ch = L' ';
		while (this->ch != L'\n')
		{
			*this->source_input >> ch;

			if (this->ch == -1)
			{
				break;
			}
			this->common_print(ch);
			this->line += this->ch;
			this->ll++;
		}
		this->common_print('\n');
	}
	this->ch = this->line[cc];
	cc++;
	return 0;
}

void PL0_Compiler::common_print(wchar_t c)
{
	if (this->common_output != nullptr) {
		*this->common_output << c;
	}
}
void PL0_Compiler::common_print(const wchar_t* s)
{
	if (this->common_output != nullptr && s != nullptr) {
		*this->common_output << s;
	}
}
void PL0_Compiler::common_print(int i)
{
	if (this->common_output != nullptr) {
		*this->common_output << i;
	}
}
void PL0_Compiler::common_input_(int& i)
{
	if (this->common_input != nullptr) {
		*this->common_input >> i;
	}
}
void PL0_Compiler::common_input_(std::wstring& text)
{
	if (this->common_input != nullptr) {
		*this->common_input >> text;
	}
}
void PL0_Compiler::common_input_(wchar_t& c)
{
	if (this->common_input != nullptr) {
		*this->common_input >> c;
	}
}
void PL0_Compiler::error_print(const wchar_t* s)
{
	if (this->error_output != nullptr) {
		*this->error_output << s;
	}
}
//词法分析，获取一个符号
int PL0_Compiler::getsym()
{
	int i = 0, j = 0, k = 0;
	while (this->ch == L' '
		|| this->ch == 10
		|| this->ch == 9)
	{
		if (-1 == getch())return -1;
	}
	if (ch >= L'a' && ch <= L'z')
	{
		k = 0;
		do {
			a += ch;
			k++;

			if (-1 == getch())return -1;
		} while (ch >= 'a' && ch <= 'z' || ch >= '0' && ch <= '9');
		//
		//a[k] = 0;
		//strcpy(id, a);
		//i = 0;
		//j = NKEYWORDS - 1;
		//do {
		//	k = (i + j) / 2;
		//	if (strcmp(id, word[k]) <= 0)
		//	{
		//		j = k - 1;
		//	}
		//	if (strcmp(id, word[k]) >= 0)
		//	{
		//		i = k + 1;
		//	}
		//} while (i <= j);
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
		if (this->ch >= L'0' && this->ch <= L'9')
		{
			k = 0;
			num = 0;
			sym = symbol::number;
			do {
				num = 10 * num + ch - L'0';
				k++;
				if (-1 == getch())return -1;
			} while (this->ch >= L'0' && this->ch <= L'9'); //获取数字的值
			k--;
		}
		else
		{
			if (ch == L':') //检测赋值符号
			{
				if (-1 == getch())return -1;
				if (ch == L'=')
				{
					sym = symbol::becomes;
					if (-1 == getch())return -1;
				}
				else
				{
					sym = symbol::nul; //不能识别的符号
				}
			}
			else
			{
				if (this->ch == L'<') //检测小于或小于等于符号
				{
					if (-1 == getch())return -1;
					if (this->ch == L'=')
					{
						sym = symbol::leq;
						if (-1 == getch())return -1;
					}
					//增加不等号语法分析
					else if (this->ch == '>') //小于号后面跟着大于号
					{
						sym = symbol::neq; //构成不等号<>
						if (-1 == getch())return -1;
					}
					else
					{
						sym = symbol::lss;
					}
				}
				else
				{
					if (this->ch == L'>')          //检测大于或大于等于符号
					{
						if (-1 == getch())return -1;
						if (this->ch == L'=')
						{
							sym = symbol::geq;
							if (-1 == getch())return -1;
						}
						else
						{
							sym = symbol::gtr;
						}
					}
					else if (this->ch == L'*')
					{
						if (-1 == getch())return -1;
						if (this->ch == L'=')
						{
							sym = symbol::timeseql;//构成*=号
							if (-1 == getch())return -1;
						}
						else
						{
							sym = symbol::times;
						}
					}
					else if (this->ch == '/')
					{
						if (-1 == getch())return -1;
						if (this->ch == '=')
						{
							sym = symbol::slasheql;//构成/=号
							if (-1 == getch())return -1;
						}
						else if (this->ch == '*')//增加注释功能
						{
							int end = 1;
							char a = 0, b = 0;
							if (-1 == getch())return -1;
							this->common_print(ch);
							while (end)
							{
								if (b != 0) { b = a; }
								else b = this->ch;
								if (-1 == getch())return -1;
								a = this->ch;

								this->common_print(a);

								if (b == L'*' && a == L'/')
								{
									end = 0;
									break;
								}
							}
							if (-1 == getch())return -1;
						}
						else
						{
							sym = symbol::slash;
						}
					}
					else if (ch == '+')
					{
						if (-1 == getch())return -1;
						if (ch == '+')
						{
							sym = symbol::plusplus;//
							if (-1 == getch())return -1;
						}
						else
						{
							sym = symbol::plus;
						}
					}
					else if (ch == '-')
					{
						if (-1 == getch())return -1;
						if (ch == '-')
						{
							sym = symbol::minusminus;//
							if (-1 == getch())return -1;
						}
						else
						{
							sym = symbol::minus;
						}
					}
					else if (ch == '\'')//
					{
						if (-1 == getch())return -1;
						if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
						{
							num = (int)ch;
							if (-1 == getch())return -1;
							if (ch == '\'')
								sym = symbol::charsym;
							else
							{
								num = 0;
								sym = symbol::nul;
								error(39);  //                  
							}
						}
						else error(39); //
						if (-1 == getch())return -1;
					}
					else
					{
						sym = ssym[ch];// 当符号不满足上述条件时，全部按照单字符号处理
						//if(-1==getch())return -1;
						//richard
						if (sym != symbol::period)
						{
							if (-1 == getch())return -1;
						}
						//end richard
					}
				}
			}
		}
	}
	return 0;
}

//生成虚拟机代码
//x:instruction.f;
//y:instruction.l;
//z:instruction.a;
int PL0_Compiler::gen(fct x, int y, int z)
{
	this->ci = { x,y,z };
	this->cx++;
	code.push_back(this->ci);
	return 0;
}
//测试当前符号是否合法
//
//在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集合
//（该部分的后跟符号） test 负责这项检测，并且负责当检测不通过时的补救措施
//程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
//符号），以及不通过时的错误号
//
//S1：我们需要的符号
//s2:如果不是我们需要的，则需要一个补救用的集合
//n:错误号
int PL0_Compiler::test(bool* s1, bool* s2, int n)
{
	if (!inset((int)this->sym, s1))
	{
		error(n);
		//当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合
		while ((!inset((int)this->sym, s1)) && (!inset((int)this->sym, s2)))
		{
			if (-1 == getsym())return -1;
		}
	}
	return 0;
}
//编译程序主体
//lev:当前分程序所在层
//tx:名字表当前尾指针
//fsys:当前模块后跟符号集合

int PL0_Compiler::compile(int lev, int tx, bool* fsys)
{
	int i;
	int dx;                         //名字分配到的相对地址
	int tx0;                        //保留初始tx
	int cx0;                        //保留初始cx
	bool nxtlev[SYMBOLS_COUNT] = { false };      //在下级函数的参数中，符号集合均为值参，但由于使用数组
									//实现，传递进来的是指针，为防止下级函数改变上级函数的
									//集合，开辟新的空间传递给下级函数
	dx = 3;
	tx0 = tx;                         //记录本层名字的初始位置
	table[tx].adr = cx;
	if (-1 == gen(fct::jmp, 0, 0)) return -1;
	//if (lev > MAX_NEST_CALLING_LEVEL)
	//{
	//	error(32);
	//}
	do {
		if (sym == symbol::constsym)         //收到常量声明符号，开始处理常量声明
		{
			if (-1 == getsym())return -1;
			do {
				if (-1 == constdeclaration(&tx, lev, &dx))return -1;   //dx的值会被constdeclaration改变，使用指针
				while (sym == symbol::comma)
				{
					if (-1 == getsym())return -1;
					if (-1 == constdeclaration(&tx, lev, &dx))return -1;   //dx的值会被constdeclaration改变，使用指针
				}
				if (sym == symbol::semicolon)
				{
					if (-1 == getsym())return -1;
				}
				else
				{
					error(5); //漏掉了逗号或者分号
				}
			} while (sym == symbol::ident);
		}
		if (sym == symbol::varsym)//收到变量声名符号，开始处理变量声名
		{
			if (-1 == getsym())return -1;
			do {
				if (-1 == vardeclaration(&tx, lev, &dx))return -1;   //dx的值会被vardeclaration改变，使用指针
				while (sym == symbol::comma)
				{
					if (-1 == getsym())return -1;
					if (-1 == vardeclaration(&tx, lev, &dx))return -1;   //dx的值会被vardeclaration改变，使用指针
				}
				if (sym == symbol::semicolon)
				{
					if (-1 == getsym())return -1;
				}
				else
				{
					error(5);
				}
			} while (sym == symbol::ident);
		}
		if (sym == symbol::charsym)//收到符号型声名符号，开始处理符号型声名
		{
			if (-1 == getsym())return -1;
			do {
				if (-1 == chardeclaration(&tx, lev, &dx))return -1;   //dx的值会被vardeclaration改变，使用指针
				while (sym == symbol::comma)
				{
					if (-1 == getsym())return -1;
					if (-1 == chardeclaration(&tx, lev, &dx))return -1;   //dx的值会被vardeclaration改变，使用指针
				}
				if (sym == symbol::semicolon)
				{
					if (-1 == getsym())return -1;
				}
				else
				{
					error(5);
				}
			} while (sym == symbol::ident);
		}
		while (sym == symbol::procsym)//收到过程声名符号，开始处理过程声名
		{
			if (-1 == getsym())return -1;
			if (sym == symbol::ident)
			{
				enter(otype::procedur, &tx, lev, &dx);//记录过程名字
				if (-1 == getsym())return -1;
			}
			else
			{
				error(4);//procedure后应为标识符
			}
			if (sym == symbol::semicolon)
			{
				if (-1 == getsym())return -1;
			}
			else
			{
				error(5);//漏掉了分号
			}
			memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
			nxtlev[(int)symbol::semicolon] = true;
			if (-1 == compile(lev + 1, tx, nxtlev))
			{
				return -1;//递归调用
			}
			if (sym == symbol::semicolon)
			{
				if (-1 == getsym())return -1;
				memcpy(nxtlev, statbegsys, sizeof(bool) * SYMBOLS_COUNT);
				nxtlev[(int)symbol::ident] = true;
				nxtlev[(int)symbol::procsym] = true;
				if (-1 == test(nxtlev, fsys, 6)) return -1;
			}
			else
			{
				error(5);                       //漏掉了分号
			}
		}
		memcpy(nxtlev, statbegsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::ident] = true;
		nxtlev[(int)symbol::period] = true;
		if (-1 == test(nxtlev, declbegsys, 7)) return -1;
	} while (inset((int)sym, declbegsys));					//直到没有声明符号
	code[table[tx0].adr].a = cx;							//开始生成当前过程代码
	table[tx0].adr = cx;									//当前过程代码地址
	table[tx0].size = dx;									//声明部分中每增加一条声明都会给dx增加1,声明部分已经结束,dx就是当前过程数据的size

	cx0 = cx;
	if (-1 == gen(fct::inte, 0, dx)) return -1;                             //生成分配内存代码
	if (this->common_output!=nullptr)									 //输出名字表
	{
		*this->common_output<<L"TABLE:"<<std::endl;
		if (tx0 + 1 > tx)
		{
			*this->common_output << L"NULL:" << std::endl;
		}
		for (i = tx0 + 1; i <= tx; i++)
		{
			switch (table[i].kind)
			{
			case otype::constant:
				*this->common_output<<i<< L" const "<<table[i].name <<L" val= "<<table[i].val<< std::endl;
				break;
			case otype::variable:
				*this->common_output << i << L" var " << table[i].name << L" level= " << table[i].level << L" adr= " << table[i].adr << std::endl;
				break;
			case otype::procedur:
				*this->common_output << i << L" proc " << table[i].name << L" level= " << table[i].level << L" adr= " << table[i].adr<<L" size= "<<table[i].size << std::endl;
				break;
			}
		}	
	}
	//语句后跟符号为分号或end
	memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);//每个后跟符号集和都包含上层后跟符号集和，以便补救
	nxtlev[(int)symbol::semicolon] = true;
	nxtlev[(int)symbol::endsym] = true;
	if (-1 == statement(nxtlev, &tx, lev)) return -1;
	if (-1 == gen(fct::opr, 0, 0)) return -1; //每个过程出口都要使用的释放数据段命令
	memset(nxtlev, 0, sizeof(bool) * SYMBOLS_COUNT); //分程序没有补救集合
	test(fsys, nxtlev, 8);                  //检测后跟符号正确性
	listcode(cx0);                        //输出代码
	return 0;
}
//在名字表中加入一项
//
//k:名字种类const,var or procedure
//ptx:名字表尾指针的指针，为了可以改变名字表尾指针的数值
//lev:名字所在的层次，以后所有的lev都是这样
//pdx:为当前应分配的变量的相对地址，分配后要增加1
void PL0_Compiler::enter(otype k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	table[(*ptx)].name = id;       //全局变量id中已存有当前名字的名字
	table[(*ptx)].kind = k;
	switch (k)
	{
	case otype::constant:                      //常量名字
		if (num > MAX_ADDRESS)
		{
			error(31);
			num = 0;
		}
		table[(*ptx)].val = num;
		break;
	case otype::variable:                     //变量名字
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;                          //过程名字
	case otype::procedur:
		table[(*ptx)].level = lev;
		break;
	case otype::charcon:                       //字符型名字
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	}
}
//
//查找名字的位置
//找到则返回在名字表中的位置，否则返回0
//
//idt: 要查找的名字
//tx:：当前名字表尾指针
int PL0_Compiler::position(std::wstring idt, int  tx)
{
	int i = tx;
	table[0].name = idt;
	while (table[i].name != idt)
	{
		i--;
	}
	return i;
}
//常量声明处理
int PL0_Compiler::constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == symbol::ident)
	{
		if (-1 == getsym())return -1;
		if (sym == symbol::eql || sym == symbol::becomes)
		{
			if (sym == symbol::becomes)
			{
				error(1);                     //把=写出成了：=
			}
			if (-1 == getsym())return -1;
			if (sym == symbol::number)
			{
				enter(otype::constant, ptx, lev, pdx);
				if (-1 == getsym())return -1;
			}
			else
			{
				error(2);                  //常量说明=后应是数字
			}
		}
		else
		{
			error(3);                       //常量说明标识后应是=
		}
	}
	else
	{
		error(4);                        //const后应是标识
	}
	return 0;
}
int PL0_Compiler::vardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == symbol::ident)
	{
		this->enter(otype::variable, ptx, lev, pdx);//填写名字表
		if (-1 == getsym())return -1;
	}
	else
	{
		error(4);
	}
	return 0;
}
//字符型声明
int PL0_Compiler::chardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == symbol::ident)
	{
		this->enter(otype::charcon, ptx, lev, pdx);//填写名字表
		if (-1 == getsym())return -1;
	}
	else
	{
		error(4);
	}
	return 0;
}
//输入目标代码清单  
void PL0_Compiler::listcode(int cx0)
{
	if (this->common_output != nullptr) 
	{
		for (int i = cx0; i < this->cx; i++)
		{		
			*this->common_output << i << L' ' << mnemonic[(int)code[i].f] << L' ' << code[i].l << L' ' << code[i].a << std::endl;
		}
	}

}
//语句处理
int PL0_Compiler::statement(bool* fsys, int* ptx, int lev)
{
	int i = 0, cx1 = 0, cx2 = 0;
	bool nxtlev[SYMBOLS_COUNT] = { false };
	if (sym == symbol::ident)
	{
		i = position(id, *ptx);//在符号表中查到该标识符所在位置
		if (i == 0)
		{
			error(11);
		}
		else
		{
			if ((table[i].kind != otype::variable) && (table[i].kind != otype::charcon))
			{
				error(12);
				i = 0;
			}
			else if (table[i].kind == otype::charcon)
			{
				if (-1 == getsym())return -1;
				if (sym != symbol::becomes) error(13);
				else
				{
					if (-1 == getsym())return -1;
					if (sym != symbol::charsym) error(32);
					else
					{
						if (-1 == gen(fct::lit, 0, num))return -1;
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
					}

				}
				if (-1 == getsym())return -1;//坑：分号
			}
			else
			{
				if (-1 == getsym())return -1;
				if (sym == symbol::becomes)
				{
					if (-1 == getsym())return -1;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					if (-1 == expression(nxtlev, ptx, lev))return -1;
					if (i != 0)//如果不曾出错，i将不为0，i所指为当前语句
							//左部标识符在符号表中的位置 
					{
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
					}
				}
				else if (sym == symbol::timeseql) // *=运算 
				{
					if (-1 == getsym())return -1;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;
					if (-1 == expression(nxtlev, ptx, lev))return -1;
					if (i != 0)
					{
						if (-1 == gen(fct::opr, 0, 4))return -1;
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
					}
				}
				else if (sym == symbol::slasheql) // /=运算 
				{
					if (-1 == getsym())return -1;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;
					if (-1 == expression(nxtlev, ptx, lev))return -1;
					if (i != 0)
					{
						if (-1 == gen(fct::opr, 0, 5))return -1;
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
					}
				}
				else if (sym == symbol::plusplus) // 后++运算 
				{
					if (-1 == getsym())return -1;
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//找到变量地址，将其值入栈
					if (-1 == gen(fct::lit, 0, 1))return -1;//将常数1取到栈顶
					if (i != 0)
					{
						if (-1 == gen(fct::opr, 0, 2))return -1;     //执行加操作
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
					}
				}
				else if (sym == symbol::minusminus) // 后--运算 
				{
					if (-1 == getsym())return -1;
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//找到变量地址，将其值入栈
					if (-1 == gen(fct::lit, 0, 1))return -1;//将常数1取到栈顶
					if (i != 0)
					{
						if (-1 == gen(fct::opr, 0, 3))return -1;     //执行减操作
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
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
		if (-1 == getsym())return -1;
		if (sym == symbol::ident)
		{
			i = position(id, *ptx);
			if (i == 0) error(11);
			else
			{
				if (table[i].kind != otype::variable)
				{
					error(12);
					i = 0;
				}
				else    //++后跟的是变量
				{
					if (-1 == getsym())return -1;
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//找到变量地址，将其值入栈
					if (-1 == gen(fct::lit, 0, 1))return -1;//将常数1取到栈顶
					if (i != 0)
					{
						if (-1 == gen(fct::opr, 0, 2))return -1;     //执行加操作
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
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
		if (-1 == getsym())return -1;
		if (sym == symbol::ident)
		{
			i = position(id, *ptx);
			if (i == 0) error(11);
			else
			{
				if (table[i].kind != otype::variable)
				{
					error(12);
					i = 0;
				}
				else  //--后跟的是变量
				{
					if (-1 == getsym())return -1;
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//找到变量地址，将其值入栈
					if (-1 == gen(fct::lit, 0, 1))return -1;//将常数1取到栈顶
					if (i != 0)
					{
						if (-1 == gen(fct::opr, 0, 3))return -1;     //执行减操作
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
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
		if (-1 == getsym())return -1;
		if (sym == symbol::ident)
		{
			i = position(id, *ptx);
			if (i == 0) error(11);
			else
			{
				if (table[i].kind != otype::variable) //赋值语句中，赋值号左部标识符属性应是变量
				{
					error(12); i = 0;
				}
				else
				{
					if (-1 == getsym())return -1;
					if (sym != symbol::becomes) error(13);             //赋值语句左部标识符后应是赋值号:=
					else if (-1 == getsym())return -1;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					nxtlev[(int)symbol::tosym] = true;                     //后跟符to和downto
					nxtlev[(int)symbol::downtosym] = true;
					if (-1 == expression(nxtlev, ptx, lev))return -1;
					//处理赋值语句右部的表达式E1
					if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;     //保存初值
					switch (sym)
					{
					case symbol::tosym:           //步长为的向上增加
						if (-1 == getsym())return -1;
						cx1 = cx;       //保存循环开始点
						//将循环判断变量取出放到栈顶
						if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);    //处理表达式E2
						nxtlev[(int)symbol::dosym] = true;                         //后跟符do
						if (-1 == expression(nxtlev, ptx, lev))return -1;
						//判断循环变量条件，比如for i:=E1 to E2 do S中，判断i是否小于E2，如小于等于，继续循环，大于的话，跳出循环
						if (-1 == gen(fct::opr, 0, 13))return -1;             //生成比较指令，i是否小于等于E2的值
						cx2 = cx;                      //保存循环结束点
						//生成条件跳转指令，跳出循环，跳出的地址未知
						if (-1 == gen(fct::jpc, 0, 0))return -1;
						if (sym == symbol::dosym)               //处理循环体S
						{
							if (-1 == getsym())return -1;
							statement(fsys, ptx, lev);  //循环体处理
							//增加循环变量步长为
							//将循环变量取出放在栈顶
							if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;
							if (-1 == gen(fct::lit, 0, 1))return -1;                            //将步长取到栈顶
							if (-1 == gen(fct::opr, 0, 2))return -1;                            //循环变量加步长
							//将栈顶的值存入循环变量
							if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
							if (-1 == gen(fct::jmp, 0, cx1))return -1;                 //无条件跳转到循环开始点
							//回填循环结束点的地址，cx为else后语句执行完的位置，它正是前面未定的跳转地址
							code[cx2].a = cx;
						}
						else
						{
							error(29);    //for语句中少了do
						}
						break;

					case symbol::downtosym:            //步长为的向下减少
						if (-1 == getsym())return -1;
						cx1 = cx;            //保存循环开始点
						//将循环判断变量取出放到栈顶                         
						if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);      //处理表达式E2
						nxtlev[(int)symbol::dosym] = true;                           //后跟符do
						if (-1 == expression(nxtlev, ptx, lev))return -1;
						//判断循环变量条件，比如for i:=E1 downto E2 do S中，判断i是否大于等于E2，如大于等于，继续循环， 小于的话，跳出循环
						if (-1 == gen(fct::opr, 0, 11))return -1;   //生成比较指令，i是否大于等于E2的值
						cx2 = cx;            //保存循环结束点
						//生成条件跳转指令，跳出循环，跳出的地址未知             
						if (-1 == gen(fct::jpc, 0, 0))return -1;
						if (sym == symbol::dosym)     //处理循环体S
						{
							if (-1 == getsym())return -1;
							statement(fsys, ptx, lev);   //循环体处理
							//增加循环变量步长为
							//将循环变量取出放在栈顶
							if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;
							if (-1 == gen(fct::lit, 0, 1))return -1;                 //将步长取到栈顶
							if (-1 == gen(fct::opr, 0, 3))return -1;                 //循环变量加步长
							//将栈顶的值存入循环变量
							if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
							if (-1 == gen(fct::jmp, 0, cx1))return -1;     //无条件跳转到循环开始点
							//回填循环结束点的地址，cx为else后语句执行完的位置，它正是前面未定的跳转地址
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
			if (-1 == getsym())return -1;
			if (sym != symbol::lparen)
			{
				error(34);
			}
			else
			{
				do {
					if (-1 == getsym())return -1;
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
					else if (table[i].kind == otype::charcon)//字符型数输入
					{
						if (-1 == gen(fct::opr, 0, 19))return -1;
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;
					}
					else
					{
						if (-1 == gen(fct::opr, 0, 16))return -1;
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1; // 储存到变量
					}
					if (-1 == getsym())return -1;
				} while (sym == symbol::comma);    //一条read语句可读多个变量 
			}
			if (sym != symbol::rparen)
			{
				error(33);          // 格式错误，应是右括号
				while (!inset((int)sym, fsys))// 出错补救，直到收到上层函数的后跟符号
				{
					if (-1 == getsym())return -1;
				}
			}
			else
			{
				if (-1 == getsym())return -1;
			}
		}
		else
		{
			if (sym == symbol::writesym)           // 准备按照write语句处理，与read类似
			{
				if (-1 == getsym())return -1;
				if (sym == symbol::lparen)
				{
					do {
						if (-1 == getsym())return -1;
						if (sym == symbol::ident)
						{
							i = position(id, *ptx);
							if (i == 0)
							{
								error(11);          //过程未找到
							}
						}
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
						nxtlev[(int)symbol::rparen] = true;
						nxtlev[(int)symbol::comma] = true;     // write的后跟符号为）or，
						if (-1 == expression(nxtlev, ptx, lev))return -1;
						// 调用表达式处理，此处与read不同，read为给变量赋值
						if (table[i].kind == otype::charcon) //字符型输出 
						{
							if (-1 == gen(fct::opr, 0, 17))return -1;
						}
						else if (-1 == gen(fct::opr, 0, 14))return -1;// 生成输出指令，输出栈顶的值
					} while (sym == symbol::comma);
					if (sym != symbol::rparen)
					{
						error(33);// write()应为完整表达式
					}
					else
					{
						if (-1 == getsym())return -1;
					}
				}
				if (-1 == gen(fct::opr, 0, 15))return -1;        // 输出换行
			}
			else
			{
				if (sym == symbol::callsym)        // 准备按照call语句处理
				{
					if (-1 == getsym())return -1;
					if (sym != symbol::ident)
					{
						error(14);           //call后应为标识符
					}
					else
					{
						i = position(id, *ptx);
						if (i == 0)
						{
							error(11);          //过程未找到
						}
						else
						{
							if (table[i].kind == otype::procedur)
							{
								if (-1 == gen(fct::cal, lev - table[i].level, table[i].adr))return -1;  //生成call指令
							}
							else
							{
								error(15);      //call后标识符应为过程
							}
						}
						if (-1 == getsym())return -1;
					}
				}
				else
				{
					if (sym == symbol::ifsym)     //准备按照if语句处理
					{
						if (-1 == getsym())return -1;
						memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
						nxtlev[(int)symbol::thensym] = true;
						nxtlev[(int)symbol::dosym] = true;    //后跟符号为then或do
						if (-1 == condition(nxtlev, ptx, lev))return -1;   //调用条件处理（逻辑运算）函数
						if (sym == symbol::thensym)
						{
							if (-1 == getsym())return -1;
						}
						else
						{
							error(16);          //缺少then
						}
						cx1 = cx;                //保存当前指令地址
						if (-1 == gen(fct::jpc, 0, 0))return -1;        //生成条件跳转指令，跳转地址暂写0
						if (-1 == statement(fsys, ptx, lev))return -1;
						//处理then后的语句

						//if(sym==semicolon)
						//{
						//  if(-1==getsym())return -1;
						//}
						if (sym == symbol::elsesym)//than语句后面发现else
						{
							if (-1 == getsym())return -1;
							cx2 = cx;
							code[cx1].a = cx + 1;//cx为当前的指令地址， cx+1即为then语句执行后的else语句的位置，回填地址
							if (-1 == gen(fct::jmp, 0, 0))return -1;
							if (-1 == statement(fsys, ptx, lev))return -1;
							code[cx2].a = cx;     //经statement处理后，cx为else后语句执行完的位置，它正是前面未定的跳转地址，回填地址
						}
						else
						{

							code[cx1].a = cx;      //经statement处理后，cx为then后语句执行完的位置，它正是前面未定的跳转地址
						}
					}
					else
					{
						if (sym == symbol::beginsym)   //准备按照复合语句处理
						{
							if (-1 == getsym())return -1;
							memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
							nxtlev[(int)symbol::semicolon] = true;
							nxtlev[(int)symbol::endsym] = true;//后跟符号为分号或end
							//循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end
							if (-1 == statement(nxtlev, ptx, lev))return -1;
							while (inset((int)sym, statbegsys) || sym == symbol::semicolon)
							{
								if (sym == symbol::semicolon)
								{
									if (-1 == getsym())return -1;
								}
								else
								{
									error(10);//缺少分号
								}
								if (-1 == statement(nxtlev, ptx, lev))return -1;
							}
							if (sym == symbol::endsym)
							{
								if (-1 == getsym())return -1;
							}
							else
							{
								error(17); //缺少end或分号
							}
						}
						else
						{
							if (sym == symbol::whilesym)//准备按照while语句处理
							{
								cx1 = cx;        //保存判断条件超作的位置
								if (-1 == getsym())return -1;
								memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
								nxtlev[(int)symbol::dosym] = true;//后跟符号为do
								if (-1 == condition(nxtlev, ptx, lev))return -1;  //调用条件处理
								cx2 = cx;       //保存循环体的结束的下一个位置
								if (-1 == gen(fct::jpc, 0, 0))return -1;//生成条件跳转，但跳出循环的地址未知
								if (sym == symbol::dosym)
								{
									if (-1 == getsym())return -1;
								}
								else
								{
									error(18);      //缺少do
								}
								if (-1 == statement(fsys, ptx, lev))return -1; //循环体
								if (-1 == gen(fct::jmp, 0, cx1))return -1;//回头重新判断条件
								code[cx2].a = cx;   //反填跳出循环的地址，与if类似
							}
							else
							{
								memset(nxtlev, 0, sizeof(bool) * SYMBOLS_COUNT);//语句结束无补救集合
								if (-1 == test(fsys, nxtlev, 19))return -1;//检测语句结束的正确性
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
//表达式处理
int PL0_Compiler::expression(bool* fsys, int* ptx, int lev)
{
	symbol addop = symbol::nul;                    //用于保存正负号
	bool nxtlev[SYMBOLS_COUNT] = { false };
	if (sym == symbol::plus || sym == symbol::minus)             //开头的正负号，此时当前表达式被看作一个正的或负的项
	{
		addop = sym;                    //保存开头的正负号
		if (-1 == getsym())return -1;
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::plus] = true;
		nxtlev[(int)symbol::minus] = true;
		if (-1 == term(nxtlev, ptx, lev))return -1;              //处理项
		if (addop == symbol::minus)
		{
			if (-1 == gen(fct::opr, 0, 1)) return -1;                   //如果开头为负号生成取负指令
		}
	}
	else                             //此时表达式被看作项的加减
	{
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::plus] = true;
		nxtlev[(int)symbol::minus] = true;
		if (-1 == term(nxtlev, ptx, lev))return -1;              //处理项
	}
	while (sym == symbol::plus || sym == symbol::minus)
	{
		addop = sym;
		if (-1 == getsym())return -1;
		memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
		nxtlev[(int)symbol::plus] = true;
		nxtlev[(int)symbol::minus] = true;
		if (-1 == term(nxtlev, ptx, lev))return -1;              //处理项
		if (addop == symbol::plus)
		{
			if (-1 == gen(fct::opr, 0, 2)) return -1;             //生成加法指令
		}
		else
		{
			if (-1 == gen(fct::opr, 0, 3)) return -1;             //生成减法指令
		}
	}
	return 0;
}
//项处理
int PL0_Compiler::term(bool* fsys, int* ptx, int lev)
{
	symbol mulop = symbol::nul;               //用于保存乘除法符号
	bool nxtlev[SYMBOLS_COUNT] = { false };
	memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
	nxtlev[(int)symbol::times] = true;
	nxtlev[(int)symbol::slash] = true;
	if (-1 == factor(nxtlev, ptx, lev))return -1;       //处理因子
	while (sym == symbol::times || sym == symbol::slash)
	{
		mulop = sym;
		if (-1 == getsym())return -1;
		if (-1 == factor(nxtlev, ptx, lev))return -1;       //处理因子
		if (mulop == symbol::times)
		{
			if (-1 == gen(fct::opr, 0, 4)) return -1;          //生成乘法指令
		}
		else
		{
			if (-1 == gen(fct::opr, 0, 5)) return -1;           //生成除法指令
		}
	}
	return 0;
}
//因子处理
int PL0_Compiler::factor(bool* fsys, int* ptx, int lev)
{
	int i = 0;
	bool nxtlev[SYMBOLS_COUNT] = { false };
	if (-1 == (facbegsys, fsys, 24)) return -1;           //检测因子的开始符好号
	while (inset((int)sym, facbegsys))          //循环直到不是因子开始符号
	{
		if (sym == symbol::ident)                   //因子为常量或者变量
		{
			i = position(id, *ptx);        //查找名字
			if (i == 0)
			{
				error(11);               //标识符未声明
			}
			else
			{
				switch (table[i].kind)
				{
				case otype::constant:                                      //名字为常量
					if (-1 == gen(fct::lit, 0, table[i].val))return -1;                       //直接把常量的值入栈
					break;
				case otype::variable:                                      //名字为变量
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;      //找到变量地址并将其值入栈
					break;
				case otype::procedur:                                      //名字为过程
					error(21);                                       //不能为过程
					break;
				case otype::charcon:                                      //名字为字符型
					if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;      //找到字符型地址并将其值入栈
					break;
				}
			}
			if (-1 == getsym())return -1;

			if (sym == symbol::plusplus)
			{
				if (-1 == gen(fct::lit, lev - table[i].level, 1))return -1;//将值为入栈
				if (-1 == gen(fct::opr, lev - table[i].level, 2))return -1;//+1,栈顶加次栈顶
				if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;//出栈取值到内存
				if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//取值到栈顶
				if (-1 == gen(fct::lit, 0, 1))return -1;
				if (-1 == gen(fct::opr, 0, 3))return -1;//栈顶值减
				if (-1 == getsym())return -1;
			}
			else if (sym == symbol::minusminus)
			{
				if (-1 == gen(fct::lit, lev - table[i].level, 1))return -1;//将值为入栈
				if (-1 == gen(fct::opr, lev - table[i].level, 3))return -1;//-1,栈顶加次栈顶
				if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;//出栈取值到内存
				if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//取值到栈顶
				if (-1 == gen(fct::lit, 0, 1))return -1;
				if (-1 == gen(fct::opr, 0, 2))return -1;//栈顶值加
				if (-1 == getsym())return -1;
			}
		}
		else if (sym == symbol::plusplus)
		{
			if (-1 == getsym())return -1;
			if (sym == symbol::ident)
			{
				if (-1 == getsym())return -1;
				i = position(id, *ptx);
				if (i == 0) error(11);
				else
				{
					if (table[i].kind == otype::variable)
					{
						if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//找到变量地址，将其值入栈
						if (-1 == gen(fct::lit, 0, 1))return -1;//将常数1取到栈顶
						if (-1 == gen(fct::opr, 0, 2))return -1;     //执行加操作
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;//出栈取值到内存
						if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//取值到栈顶
					}
				}
			}
		}
		else if (sym == symbol::minusminus)
		{
			if (-1 == getsym())return -1;
			if (sym == symbol::ident)
			{
				if (-1 == getsym())return -1;
				i = position(id, *ptx);
				if (i == 0) error(11);
				else
				{
					if (table[i].kind == otype::variable)
					{
						if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//找到变量地址，将其值入栈
						if (-1 == gen(fct::lit, 0, 1))return -1;//将常数1取到栈顶
						if (-1 == gen(fct::opr, 0, 3))return -1;     //执行减操作
						if (-1 == gen(fct::sto, lev - table[i].level, table[i].adr))return -1;//出栈取值到内存
						if (-1 == gen(fct::lod, lev - table[i].level, table[i].adr))return -1;//取值到栈顶
					}
				}
			}
		}
		else
		{
			if (sym == symbol::number)                                             //因子为数
			{
				if (num > MAX_ADDRESS)
				{
					error(31);
					num = 0;
				}
				if (-1 == gen(fct::lit, 0, num))return -1;
				if (-1 == getsym())return -1;
			}
			else
			{
				if (sym == symbol::lparen)                                           //因子为表达式
				{
					if (-1 == getsym())return -1;
					memcpy(nxtlev, fsys, sizeof(bool) * SYMBOLS_COUNT);
					nxtlev[(int)symbol::rparen] = true;
					if (-1 == expression(nxtlev, ptx, lev)) return -1;
					if (sym == symbol::rparen)
					{
						if (-1 == getsym())return -1;
					}
					else
					{
						error(22);                                       //缺少右括号
					}
				}
				if (-1 == test(fsys, facbegsys, 23)) return -1;                        //银子后有非法符号
			}
		}
	}
	return 0;
}
//条件处理
int PL0_Compiler::condition(bool* fsys, int* ptx, int lev)
{
	symbol relop = symbol::nul;
	bool nxtlev[SYMBOLS_COUNT] = { false };
	if (sym == symbol::oddsym)                        //准备按照odd运算处理
	{
		if (-1 == getsym())return -1;
		if (-1 == expression(fsys, ptx, lev))return -1;
		if (-1 == gen(fct::opr, 0, 6))return -1;  //生成odd指令
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
		if (-1 == expression(nxtlev, ptx, lev)) return -1;
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
			if (-1 == getsym())return -1;
			if (-1 == expression(fsys, ptx, lev)) return -1;
			switch (relop)
			{
			case symbol::eql:
				if (-1 == gen(fct::opr, 0, 8))return -1;
				break;
			case symbol::neq:
				if (-1 == gen(fct::opr, 0, 9))return -1;
				break;
			case symbol::lss:
				if (-1 == gen(fct::opr, 0, 10))return -1;
				break;
			case symbol::geq:
				if (-1 == gen(fct::opr, 0, 11))return -1;
				break;
			case symbol::gtr:
				if (-1 == gen(fct::opr, 0, 12))return -1;
				break;
			case symbol::leq:
				if (-1 == gen(fct::opr, 0, 13))return -1;
				break;
			}

		}
	}
	return 0;
}
//解释程序
void PL0_Compiler::interpret()
{
	wchar_t cm;
	int p, b, t;             //指令指针，指令基址，栈顶指针
	struct instruction i;  //存放当前指令
	int s[STACK_DEPTH] = { 0 };      //栈

	this->common_print(L"start pl0\n");
	t = 0;
	b = 0;
	p = 0;
	s[0] = s[1] = s[2] = 0;
	do {
		i = code[p];         //读当前指令
		p++;
		switch (i.f)
		{
		case fct::lit:        //将a的值取到栈顶
			s[t] = i.a;
			t++;
			break;
		case fct::opr:        //数字、逻辑运算
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
				this->common_print(s[t - 1]);//未修改成实型字符型型前
				t--;//栈顶下移
				break;
			case 15://15号操作为输出换行操作
				this->common_print(L"\n");
				break;
			case 16://input from common_input
				this->common_print(L"?");
				this->common_input_(s[t]);

				t++;
				break;

			case 17://17号操作为输出栈顶值操作
				this->common_print((wchar_t)s[t - 1]);//输出栈顶值
				t--;//栈顶下移
				break;

			case 19://19号操作是接受键盘值输入到栈顶
				this->common_print(L"?");//屏显问号
				this->common_input_(cm);
				s[t] = cm;
				t++;//栈顶上移，分配空间
				break;
			}
			break;
		case fct::lod:       //取相对当前过程的数据基地址为ａ的内存的值到栈顶
			s[t] = s[get_caller(i.l, s, b) + i.a];
			t++;
			break;
		case fct::sto:       //栈顶的值存到相对当前过程的数据基地址为ａ的内存
			t--;
			s[get_caller(i.l, s, b) + i.a] = s[t];
			break;
		case fct::cal:              //调用子程序
			s[t] = get_caller(i.l, s, b); //将父过程基地址入栈
			s[t + 1] = b;           //将本过程基地址入栈，此两项用于base函数
			s[t + 2] = p;           //将当前指令指针入栈
			b = t;                //改变基地址指针值为新过程的基地址
			p = i.a;              //跳转
			break;
		case fct::inte:             //分配内存
			t += i.a;
			break;
		case fct::jmp:             //直接跳转
			p = i.a;
			break;
		case fct::jpc:              //条件跳转
			t--;
			if (s[t] == 0)
			{
				p = i.a;
			}
			break;
		}
	} while (p != 0);
}
//通过过程基址求上1层过程的基址
int PL0_Compiler::get_caller(int l, int* s, int b)
{
	int b1 = b;
	while (l > 0)
	{
		b1 = s[b1];
		l--;
	}
	return b1;
}

std::wstring Convert(std::string text)
{
	std::wstring ret;

	wchar_t *pw = new wchar_t[text.length()+1];
	
	wmemset(pw, 0, text.length()+1);

	int r = mbstowcs(pw, text.c_str(), (int)text.length());
	if (r < 0) {
		setlocale(LC_ALL, "");
		r = mbstowcs(pw, text.c_str(), (int)text.length());
	}
	if (r >= 0) {
		ret = pw;
	}
	delete[] pw;

	return ret;
}
