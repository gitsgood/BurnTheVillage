// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
* @brief 
* Below macros are there simply to grant us slightly more control over the logging process.
* BTV_LOG should be used in place of UE_LOG for most situations.
* 
* BTV_VERBOSE_LOG is there for excessive debugging that is not meant to be on unless a problem needs to be pinpointed.
* It is meant to be turned off by default.
*/
#define LOGGING 1			// Or 0 to disable
#define EXCESSIVE_LOGGING 0

#if !UE_BUILD_SHIPPING && LOGGING == 1
#define BTV_LOG(Category, Verbosity, Format, ...) \
	        UE_LOG(Category, Verbosity, Format, ##__VA_ARGS__)
#else
#define BTV_LOG(Category, Verbosity, Format, ...)
#endif

#if !UE_BUILD_SHIPPING && EXCESSIVE_LOGGING == 1
#define BTV_VERBOSE_LOG(Category, Verbosity, Format, ...) \
	        UE_LOG(Category, Verbosity, Format, ##__VA_ARGS__)
#else
#define BTV_VERBOSE_LOG(Category, Verbosity, Format, ...)
#endif
