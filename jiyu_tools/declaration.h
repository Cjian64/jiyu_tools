#pragma once
#include <tchar.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <regex>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>
#include <iostream>
#include <Windows.h>

extern char commandData[];
extern char killTeacherData1[];
extern char killTeacherData2[];
extern char broadcastTypeData[];

extern WSAData wsd;
extern SOCKET soSend;
extern WCHAR* wcommandData;
extern SOCKADDR_IN serverAddr;
extern std::string ip;

std::string wstringToString(std::wstring str);
std::wstring getCommand(std::wstring str, int n);

bool initialization();
bool isValidIP(std::string ip);
bool setip(std::wstring command);
bool killTeacher(std::wstring command);
bool executeCommand(std::wstring command);
bool setbc(std::wstring command);
bool setfile(std::wstring command);
bool killStudentMain();
bool restartStudentMain();
int banBroadcastFull();