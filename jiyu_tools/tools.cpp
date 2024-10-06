#include "declaration.h"

std::string wstringToString(std::wstring str)
{
	if (str.empty()) {
		return "";
	}
	unsigned len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char* p = new char[len];
	wcstombs_s(NULL, p, len, str.c_str(), len);
	std::string str1(p);
	delete[] p;
	return str1;
}

std::wstring getCommand(std::wstring str, int n) {
	std::wstring result = L"";
	int count = 0;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != L' ') {
			result += str[i];
			continue;
		}
		else {
			count++;
			if (!result.empty()) {
				n--;
			}
			if (n < 1) {
				return result;
			}
		}
		result.clear();
	}
	if (n > 1) {
		result = L"";
	}
	return result;
}