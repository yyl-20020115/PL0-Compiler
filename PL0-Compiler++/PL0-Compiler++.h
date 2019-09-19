#pragma once
#include <string>
#include <vector>
#include <fstream>

std::wstring Convert(std::string text);

class PL0_Compiler
{
public:
	static const int SYMBOLS_COUNT = 42;
	static const int MNEMONIC_COUNT = 8;
	static const int KEYWORDS_COUNT = 19;
	static const int MAX_ADDRESS = 4095;
	static const int STACK_DEPTH = 1024;

public:
	enum class symbol
	{
		nul = 0, ident, number, plus, minus,
		times, slash, oddsym, eql, neq,
		lss, leq, gtr, geq, lparen,
		rparen, comma, semicolon, period, becomes,
		beginsym, endsym, ifsym, thensym, whilesym,
		writesym, readsym, dosym, callsym, constsym,
		varsym, procsym, elsesym, forsym, tosym, downtosym,
		returnsym, timeseql, slasheql, plusplus, minusminus,
		charsym,
		symbol_count
	};
	enum class otype 
	{
		constant,
		variable,
		procedur,
		charcon,//字符型
		realcon,//实型
	};

	enum class fct 
	{
		lit, 
		opr, 
		lod, 
		sto, 
		cal, 
		inte, 
		jmp, 
		jpc,
		fct_count
	};
	struct instruction
	{
		fct f;
		int l;
		int a;
		//double a;//实型字符型修改
	};
	struct tablestruct
	{
		std::wstring name;                         //名字
		otype kind;                               //类型：const，var，array or procedure*/
		int val;                                  //数值，仅const使用
		int level;                                //所处层，仅const不使用
		int adr;                                  //地址，仅const不使用
		int size;                                 //需要分配的数据区空间，仅procedure使用
	};
protected:
	symbol ssym[256];
	symbol wsym[KEYWORDS_COUNT];
	std::wstring keywords[KEYWORDS_COUNT];
	std::wstring mnemonic[MNEMONIC_COUNT];

	bool declbegsys[SYMBOLS_COUNT];
	bool statbegsys[SYMBOLS_COUNT];
	bool facbegsys[SYMBOLS_COUNT];

	std::vector<tablestruct> table;
	std::vector<instruction> code;
	std::wistream* source_input;
	std::wistream* common_input;
	std::wostream* common_output;
	std::wostream* error_output;
	std::wstring line;
	std::wstring a;
	std::wstring id;
	wchar_t ch;
	symbol sym;

	instruction ci;
	int cx;

public:

	PL0_Compiler();

	virtual ~PL0_Compiler();

public:

	void SetSourceInput(std::wistream* source);
	std::wistream* GetSourceInput();

	void SetCommonInput(std::wistream* common_input);
	std::wistream* GetCommonInput();

	void SetCommonOutput(std::wostream* common_output);
	std::wostream* GetCommonOutput();

	void SetErrorOutput(std::wostream* error_output);
	std::wostream* GetErrorOutput();

public:

	virtual void Reset();
	virtual void Execute();


protected:

	bool inset(int e, bool* s);

	void addset(bool* sr, bool* s1, bool* s2, int n);

	void subset(bool* sr, bool* s1, bool* s2, int n);

	void mulset(bool* sr, bool* s1, bool* s2, int n);
	int test(bool* s1, bool* s2, int n);

protected:
	void error(int n);
	int getch();
	int getsym();
	void common_print(wchar_t c);
	void common_print(const wchar_t* s);
	void common_print(int i);
	void common_input_(int& i);
	void common_input_(std::wstring& text);
	void common_input_(wchar_t& c);

	void error_print(const wchar_t* s);

	int gen(fct x, int y, int z);
	int compile(int lev, int tx, bool* fsys);
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
	int position(std::wstring idt, int tx);
	void enter(otype k, int* ptx, int lev, int* pdx);
	int get_caller(int l, int* s, int b);

protected:
	int error_count;
	int cc;
	int ll;
	int num;
};
