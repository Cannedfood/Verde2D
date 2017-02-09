#pragma once

bool alCheckErrImpl(const char* file, const char* fun, int line);

#define alCheckErr() ::alCheckErrImpl(__FILE__, __func__, __LINE__)
