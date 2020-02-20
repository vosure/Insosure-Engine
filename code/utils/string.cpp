#include "string.h"
#include "string_utils.h"

string string::operator=(const char *Text)
{
	if (this->Length < strlen(Text))
	{
		if (this->Length == 0)
		{
			this->Native = (char*)malloc(strlen(Text) + 1);
		}
		else
		{
			this->Native = (char*)realloc(this->Native, strlen(Text) + 1);
		}
		this->Length = strlen(Text);
	}
	strncpy_s(this->Native, this->Length + 1, Text, strlen(Text));

	return *this;
}

string string::operator=(const string& Str)
{
	if (this->Length < Str.Length)
	{
		if (this->Length == 0)
		{
			this->Native = (char*)malloc(Str.Length + 1);
		}
		else
		{
			this->Native = (char*)realloc(this->Native, Str.Length + 1);
		}
		this->Length = Str.Length;
	}
	strncpy_s(this->Native, this->Length + 1, Str.Native, Str.Length);

	return *this;
}

char string::operator[](int Index)
{
	return this->Native[Index];
}

string String(string& Str)
{
	string Result;
	Result.Length = Str.Length;
	Result.Native = (char*)malloc(Result.Length + 1);
	strncpy_s(Result.Native, Result.Length + 1, Str.Native, Str.Length);

	return Result;
}

string String(const char *Text)
{
	string Result;
	Result.Length = strlen(Text);
	Result.Native = (char*)malloc(sizeof(char) * (Result.Length + 1));
	strncpy_s(Result.Native, Result.Length + 1, Text, strlen(Text));

	return Result;
}

string String(int Number)
{
	char NumToStr[16];
	_itoa(Number, NumToStr, 10);

	return String(NumToStr);
}

void FreeString(string String)
{
	if (String.Native)
		free(String.Native);
}

string& operator+=(string &String, const char *Text)
{
	String.Length += strlen(Text);
	String.Native = (char*)realloc(String.Native, String.Length + 1);
	strcat_s(String.Native, String.Length + 1, Text);

	return String;
}

string& operator+=(string &String, string Right)
{
	String.Length += Right.Length;
	String.Native = (char*)realloc(String.Native, String.Length + 1);
	strcat_s(String.Native, String.Length + 1, Right.Native);

	return String;
}

string operator+(string String, const char *Text)
{
	string Result;
	Result.Length = String.Length + strlen(Text);
	Result.Native = (char*)malloc(Result.Length + 1);
	strncpy_s(Result.Native, Result.Length + 1, String.Native, String.Length);
	strcat_s(Result.Native, Result.Length + 1, Text);

	return Result;
}

string operator+(const string& Left, const string& Right)
{
	string Result = String(Left.Native);
	Result += Right;

	return Result;
}

string operator+(const char *Text, const string& Str)
{
	string Result = String(Text);
	Result += Str;

	return Result;
}

bool operator==(const string& Left, const string& Right)
{
	return (strcmp(Left.Native, Right.Native) == 0);
}

bool operator==(const string& Left, const char *Right)
{
	return (strcmp(Left.Native, Right) == 0);
}

bool operator==(const char *Left, const string& Right)
{
	return (strcmp(Left, Right.Native) == 0);
}

bool operator!=(const string& Left, const string& Right)
{
	return (strcmp(Left.Native, Right.Native) != 0);
}

bool operator!=(const string& Left, const char *Right)
{
	return (strcmp(Left.Native, Right) != 0);
}

bool operator!=(const char *Left, const string& Right)
{
	return (strcmp(Left, Right.Native) != 0);
}

int Size(const string& Str)
{
	return Str.Length;
}