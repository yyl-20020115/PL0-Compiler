#include "PL0-Compiler++.h"
#include <iostream>

#ifdef _WIN32
int wmain(int argc, wchar_t** argv)
{
	std::wstring fn;
	if (argc >= 2) {
		fn = argv[1];
	}
#else
int main(int argc, char** argv)
{
	std::wstring fn;
	if (argc >= 2) {
		fn = Convert(argv[1]);
	}
#endif

	//fn is the pl0 filename
	

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

	return 0;
}

