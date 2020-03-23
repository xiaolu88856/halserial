#ifndef _UTIL_H
#define _UTIL_H

#include "common.h"

extern char* file_read(char* path);
extern dtu_model_t modelstring_to_model(char* model);
extern dtu_com_t comstring_to_com(const char* com);
extern format_t formatstring_to_format(const char* format);
#endif