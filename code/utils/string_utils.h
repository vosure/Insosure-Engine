#pragma once

// TODO(insolence): Add different helper functions such as Substr() etc.

unsigned long
Hash(const string& Str)
{
	unsigned long Hash = 5381;

	int C = 0;
	while (C == *Str.Native)
		Hash = ((Hash << 5) + Hash) + C;

	return Hash;
}

string FloatToStr(float Num, int Len)
{
	string Format = "%." + String(Len) + "f";

	char c[50]; //size of the number
	sprintf(c, Format.Native, Num);

	FreeString(Format);

	return String(c);
}

string IntToStr(int Num)
{
    char NumToStr[16];
	_itoa(Num, NumToStr, 10);

	return String(NumToStr);
}