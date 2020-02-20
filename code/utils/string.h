#pragma once

struct string
{
	int Length = 0;
	char *Native;

	string operator=(const char *Text);
	string operator=(const string& Str);
	char operator[](int Index);
};

string String(string& Str);
string String(const char *Text);
string String(int Number);

void FreeString(string String);

string& operator+=(string &String, const char *Text);
string& operator+=(string &String, string Right);
string operator+(string String, const char *Text);
string operator+(const string& Left, const string& Right);
string operator+(const char *Text, const string& Str);
bool operator==(const string& Left, const string& Right);
bool operator==(const string& Left, const char *Right);
bool operator==(const char *Left, const string& Right);
bool operator!=(const string& Left, const string& Right);
bool operator!=(const string& Left, const char *Right);
bool operator!=(const char *Left, const string& Right);

int Size(const string& Str);