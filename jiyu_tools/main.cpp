#include <thread>
#include "declaration.h"
#define use_CRT_SECURE_NO_WARNINGS
using namespace std;
#pragma comment(lib, "ws2_32.lib")

wstring command;
bool windowTop = false;

void setWindowTop() {
	HWND hWnd = GetForegroundWindow();
	while (windowTop) {
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

int main(int argc, _TCHAR* argv[])
{
	cout << "极域Tools[v5.0]   (C)初见\n";
	cout << "本工具仅限用于交流学习，请勿用于恶意行为！\n";
	cout << "使用本工具造成的任何后果需自行承担，本作者概不负责！\n";
	cout << "\n";
	initialization();
	while (true) {
		cout << ip << ">";
		getline(wcin, command);
		if (!_wcsicmp(getCommand(command,1).data(), L"")) {
			continue;
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"command")) {
			executeCommand(command);
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"setip")) {
			setip(command);
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"killtc")) {
			killTeacher(command);
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"setbc")) {
			setbc(command);
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"setfile")) {
			setfile(command);
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"killst")) {
			killStudentMain();
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"banbcfull")) {
			banBroadcastFull();
		}
		else if (!_wcsicmp(getCommand(command,1).data(),L"clear")) {
			system("cls");
		}
		else if (!_wcsicmp(getCommand(command, 1).data(),L"setwindow")) {
			if (!_wcsicmp(getCommand(command, 2).data(),L"-t")) {
				windowTop = true;
				thread setwindowTop(setWindowTop);
				setwindowTop.detach();
				cout << "已启用窗口置顶!\n";
			}else if (!_wcsicmp(getCommand(command, 2).data(),L"-n")) {
				windowTop = false;
				cout << "已取消窗口置顶!\n";
			}
		}
		else if (!_wcsicmp(getCommand(command, 1).data(),L"help")) {
			cout << "======================================================================\n";
			cout << "<>:必填参数，[]:选填参数\n";
			cout << "----------------------------------------------------------------------\n";
			cout << "setip <ip>          设置远程执行ip\n";
			cout << "command [启动参数]  学生端远程执行\n";
			cout << "setfile <路径>      设置远程执行文件路径\n";
			cout << "killtc <ip>         干掉教师端\n";
			cout << "killst              干掉本机学生端\n";
			cout << "banbcfull           禁止本机学生端广播全屏\n";
			cout << "setbc <参数> <ip>   设置目标学生端广播窗口。参数：-f全屏，-s取消全屏\n";
			cout << "setwindow <参数>    设置当前窗口置顶。参数：-t顶置，-n取消顶置\n";
			cout << "clear               清除屏幕\n";
			cout << "help                显示帮助\n";
			cout << "======================================================================\n";
		}
		else {
			cout << "未知的命令!输入help查看命令！\n";
		}
		cout << "\n";
	}
	closesocket(soSend);
	WSACleanup();
	return 0;
}
