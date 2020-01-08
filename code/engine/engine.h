#pragma once


#define SUCCESS 1
#define BAD_RESULT -1

#define internal static
#define local_persist static 
#define global_variable static 

#define Assert(Expression) if (!(Expression)) { *(int*)0 = 0; }

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))