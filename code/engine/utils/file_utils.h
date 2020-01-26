#pragma once

#include <stdio.h>
#include <string.h>

// NOTE(insolence): Should it return NULL? Then we can't even debug
// cause the app immediately crashes
char *ReadFile(const char *FilePath)
{
	char Append[130];
	strcpy(Append, "C:/dev/Insosure-Engine/assets/");
	strcat(Append, FilePath);

	FILE *File = fopen(Append, "rt");
	if (File == NULL)
		return "NULL"; //could not open file
	fseek(File, 0, SEEK_END);
	unsigned long Length = ftell(File);

	char *Data = (char *)malloc(sizeof(char) * (Length + 1));
	memset(Data, 0, Length + 1);

	fseek(File, 0, SEEK_SET);
	fread(Data, 1, Length, File);
	fclose(File);

	return Data;
}