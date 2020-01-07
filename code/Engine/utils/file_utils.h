#pragma once
#include <stdio.h>

char *read_file(const char *file_path)
{
	FILE *file = fopen(file_path, "rt");
	if (file == NULL)
		return NULL; //could not open file
	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);

	char *data = new char[length + 1];
	memset(data, 0, length + 1);

	fseek(file, 0, SEEK_SET);
	fread(data, 1, length, file);
	fclose(file);

	return data;
}