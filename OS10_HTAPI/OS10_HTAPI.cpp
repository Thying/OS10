#include "pch.h" 
#pragma once
#define WIN32_LEAN_AND_MEAN

#ifdef OS10_HTAPI_EXPORTS
#define OS10_HTAPI_API __declspec(dllexport)
#else
#define OS10_HTAPI_API __declspec(dllimport)
#endif