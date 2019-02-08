#pragma once
#include <string>
#include <Windows.h>

std::string readFile(const std::string &filename);

std::string zeroPadNumber(unsigned int num, unsigned int padding);


#define SAT_LOG_ERROR(message, ...)			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);\
											printf(message, ##__VA_ARGS__);	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");


#define __SAT_STRINGIFY(x) #x
#define __SAT_TOSTRING(x) __SAT_STRINGIFY(x)
#define __SAT_AT "[" __FILE__ ":" __SAT_TOSTRING(__LINE__) "]"
#define __SAT_FUNCTION __FUNCSIG__ 


#define SAT_FUNCTION_NAME()					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C); printf("%s", __SAT_FUNCTION); \
											SAT_LOG_ERROR("")

#define SAT_ERROR_LOC(message, ...)			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C); printf("%s ", __SAT_AT); \
											SAT_LOG_ERROR(message, ##__VA_ARGS__)

#if _DEBUG

#define SAT_DEBUG_LOG(message, ...)			printf(message, ##__VA_ARGS__); printf("\n");

#define SAT_DEBUG_LOG_ERROR(message, ...)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);\
											printf(message, ##__VA_ARGS__);	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");

#define SAT_DEBUG_LOG_WARNING(message, ...)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0E); printf(message, ##__VA_ARGS__);\
											SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");

#define SAT_DEBUG_LOG_SAFE(message, ...)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0A); printf(message, ##__VA_ARGS__);\
											SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");

#define SAT_ASSERT(expression, message, ...)	if(!(expression)) { SAT_DEBUG_LOG_WARNING(#expression, ##__VA_ARGS__); SAT_FUNCTION_NAME(); SAT_ERROR_LOC(message, ##__VA_ARGS__); system("PAUSE");}




#else

#define SAT_DEBUG_LOG(message, ...)			((void)0)
#define SAT_DEBUG_LOG_ERROR(message, ...)	((void)0)	
#define SAT_DEBUG_LOG_WARNING(message, ...)	((void)0)
#define SAT_DEBUG_LOG_SAFE(message, ...)	((void)0)
#define SAT_ASSERT __noop

#endif