#include <windows.h>
#include <string.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>


#pragma region buttonsID

#define button_id_1 1
#define button_id_2 2
#define button_id_3 3
#define button_id_4 4
#define button_id_5 5
#define button_id_6 6
#define button_id_7 7
#define button_id_8 8
#define button_id_9 9
#define button_id_0 10

#define button_keyId_sum 11
#define button_keyId_sub 12
#define button_keyId_div 13
#define button_keyId_mult 14
#define button_keyId_pov 15
#define button_keyId_dot 16
#define button_keyId_open 17
#define button_keyId_close 18
#define button_keyId_clr 19
#define button_keyId_ans 20

#pragma endregion

float calculate(WCHAR* buff, int length);
int IndexOfButton(int value);
void ClearScreenText();
LRESULT WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

HWND text;
WCHAR screenText[100];
int counter = 0;


//UI
int buttonsId[] =
{
	button_keyId_mult,	button_keyId_div,	button_keyId_pov,	button_keyId_clr,
	button_keyId_sum,	button_id_1,		button_id_2,		button_id_3,
	button_keyId_sub,	button_id_4,		button_id_5,		button_id_6,
	button_keyId_dot,	button_id_7,		button_id_8,		button_id_9,
	button_keyId_open,	button_keyId_close, button_id_0,		button_keyId_ans
};
const CHAR* buttonsName[] =
{

	"*","/","^","X",
	"+","1","2","3",
	"-","4","5","6",
	".","7","8","9",
	"(",")","0","="
};
int columnsCount = 4;
int rowCount = 5;
int horizontalOffset = 5;
int verticalOffset = 5;
int buttonHeight = 40;
int buttonWidth = 40;
int windowOffset = 30;
int textHeight = 40;
int cornerRadius = 15;


//int main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSA wcl;
	memset(&wcl, 0, sizeof(WNDCLASSA));
	wcl.lpszClassName = "Calculator";
	wcl.lpfnWndProc = (WNDPROC)WndProc;
	RegisterClassA(&wcl);

	HWND hwnd;
	int textWidth = columnsCount * buttonWidth + (columnsCount - 1) * horizontalOffset;
	int windowHeight = windowOffset * 2 + (buttonHeight + verticalOffset) * rowCount + textHeight;
	int windowWidth = windowOffset * 2 + textWidth;
	hwnd = CreateWindowA("Calculator", "Calculator", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, 100, 100, windowWidth + 15, windowHeight + 35, NULL, NULL, NULL, NULL);
	ShowWindow(hwnd, SW_SHOWNORMAL);

	//Buttons
	HRGN rgn = nullptr;
	HWND button = nullptr;
	int x0 = windowOffset;
	int y0 = windowOffset + textHeight + verticalOffset;
	for (int i = 0; i < sizeof(buttonsId) / sizeof(int); i++)
	{
		int x = x0 + (buttonWidth + horizontalOffset) * (i % columnsCount);
		int y = y0 + (buttonHeight + verticalOffset) * ((int)(i / columnsCount));
		button = CreateWindowA("button", (LPCSTR)buttonsName[i], WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, x, y, buttonWidth, buttonHeight, hwnd, (HMENU)buttonsId[i], NULL, NULL);
		rgn = CreateRoundRectRgn(2, 2, buttonWidth - 2, buttonHeight - 2, cornerRadius, cornerRadius);
		SetWindowRgn(button, rgn, true);
	}

	//Text Lable
	rgn = CreateRoundRectRgn(0, 0, textWidth, textHeight, cornerRadius, cornerRadius);
	text = CreateWindowA("static", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, windowOffset, windowOffset, textWidth, textHeight, hwnd, NULL, NULL, NULL);
	SetWindowRgn(text, rgn, true);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}

	return 0;
}

float calculate(WCHAR* expression, int length)
{
	WCHAR* param = expression;
	int len = length;

	bool isTitsCase = false;  // expression like: ()*(), ()/(), ()^()
	bool ignoreExpression = false; // ignore expression in brackets
	int closingBracketIndex = 0;
	int openBracketsCount = 0;

	printf_s("\n\nexpression:  ");
	for(int i = 0; i < len; i++)
	{
		printf_s("%wc",expression[i]);
	}

	/*cleaning */
	for (int i = 0; i < len; i++)
	{
		if (param[i] == '(')
		{
			if (closingBracketIndex != 0 && i > closingBracketIndex)
			{
				isTitsCase = true;
			}
			openBracketsCount++;
		}
		else if (param[i] == ')')
		{
			openBracketsCount--;
			closingBracketIndex = i;
			if (*param == '(' && openBracketsCount == 0 && !isTitsCase && (param[i + 1] < 32 || i == len - 1)) //when whole expression is in brackets
			{
				param = &expression[1];
				len = i - 1;
			}
		}
		
	}

	printf_s("\nexpression after cleaning:  ");
	for (int i = 0; i < len; i++)
	{
		printf_s("%wc", param[i]);
	}
	printf_s("\n");

	/*low priority group (+,-) */
	for (int i = 0; (i < len) && (param[i] != (char)0 && param[i] != (char)13); i++)
	{
		if (param[i] == '(')
		{
			openBracketsCount++;
			ignoreExpression = true;
			continue;
		}
		else if (param[i] == ')')
		{
			openBracketsCount--;
			if (openBracketsCount == 0)
			{
				ignoreExpression = false;
			}
			continue;
		}
		else if (param[i] == '+' && !ignoreExpression)
		{
			float x = calculate(param, i);
			float y = calculate(&param[i + 1], len - i - 1);
			return x + y;
		}
		else if (param[i] == '-' && !ignoreExpression)
		{
			float x = calculate(param, i);
			float y = calculate(&param[i + 1], len - i - 1);
			return x - y;
		}
	}
	ignoreExpression = false;

	/*medium priority group (*,/) */
	for (int i = 0; (i < len) && (param[i] != (char)0 && param[i] != (char)13); i++)
	{
		if (param[i] == '(')
		{
			openBracketsCount++;
			ignoreExpression = true;
			continue;
		}
		else if (param[i] == ')')
		{
			openBracketsCount--;
			if (openBracketsCount == 0)
			{
				ignoreExpression = false;
			}
			continue;
		}
		if (param[i] == '*' && !ignoreExpression)
		{
			float x;
			float y;
			x = calculate(param, i);
			y = calculate(&param[i + 1], len - i - 1);
			return x * y;
		}
		if (param[i] == '/' && !ignoreExpression)
		{
			float x;
			float y;
			x = calculate(param, i);
			y = calculate(&param[i + 1], len - i - 1);
			if (y == 0 || fabsf(y) < 0.000001f) 
			{
				printf_s("\ndevide on 0.\n");
				throw 0;
			} 
			return x / y;
		}
	}
	ignoreExpression = false;

	/*high priority group (^) */
	for (int i = 0; (i < len) && (param[i] != (char)0 && param[i] != (char)13); i++)
	{
		if (param[i] == '(')
		{
			openBracketsCount++;
			ignoreExpression = true;
			continue;
		}
		else if (param[i] == ')')
		{
			openBracketsCount--;
			if (openBracketsCount == 0)
			{
				ignoreExpression = false;
			}
			continue;
		}
		if (param[i] == '^' && !ignoreExpression)
		{
			float x;
			float y;

			x = calculate(param, i);
			y = calculate(&param[i + 1], len - i - 1);
			return powf(x, y);
		}
	}

	/*number*/
	float temp = 0;
	char buffer[100];
	for (int i = 0; i < len; i++)
	{
		buffer[i] = param[i];
	}
	buffer[len] = 0;
	sscanf_s(buffer, "%f", &temp);
	return temp;
}

int IndexOfButton(int value)
{
	for (int i = 0; i < sizeof(buttonsId) / sizeof(int); i++)
	{
		if (buttonsId[i] == value) return i;
	}
	return -1;
}

void ClearScreenText()
{
	for (int i = 0; i < 100; i++)
	{
		screenText[i] = 0;
	}
}

LRESULT WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else if (message == WM_COMMAND)
	{

		printf("\nbutton code = %d \n", LOWORD(wparam));

		switch (LOWORD(wparam))
		{
		case button_keyId_clr:
		{
			if (counter != 0)
			{
				screenText[counter - 1] = 0;
				counter -= 2;
			}
			break;
		}

		case button_keyId_ans:
		{
			float buff;
			try
			{
				buff = calculate(screenText, 100);
				ClearScreenText();
				swprintf_s(screenText, sizeof(screenText), L"%f", buff);
			}
			catch(int)
			{
				swprintf_s(screenText, sizeof(screenText), L"%ws", L"ERROR");
			}
			counter = -1;
			break;
		}
		default:
		{
			int index = IndexOfButton(LOWORD(wparam));
			if (index == -1) break;
			screenText[counter] = (char)buttonsName[index][0];
			break;
		}
		}
		printf("%ws", screenText);
		SetWindowText(text, screenText);
		if (counter == -1)
		{
			ClearScreenText();
		}
		counter++;
	}
	else return DefWindowProcA(hwnd, message, wparam, lparam);
}