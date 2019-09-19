#include "PL0-Compiler++.h"
#include <iostream>

#ifdef _WIN32
int wmain(int argc, wchar_t** argv)
{
	std::wstring fn;

#else
int main(int argc, char** argv)
{
	std::string fn;
#endif
	if (argc >= 2) {
		fn = argv[1];
	}
	//fn is the pl0 filename
	if(fn.size() == 0)
	{
		std::wcout << L"Usage: PL0-Compiler++ <FileName.PL0>" << std::endl;
	}
	else 
	{
		PL0_Compiler* compiler = new PL0_Compiler();

		std::wifstream fs(fn);

		if (fs) {
			compiler->SetCommonInput(&std::wcin);
			compiler->SetCommonOutput(&std::wcout);
			compiler->SetErrorOutput(&std::wcerr);
			compiler->SetSourceInput(&fs);

			compiler->Execute();
		}

		delete compiler;
	}
	getchar();
	getchar();

	return 0;
}

