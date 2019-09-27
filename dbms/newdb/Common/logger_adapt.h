#pragma once


#include <Poco/Logger.h>


#define LOG_TRACE(logger, fmt, args...) do { \
    if ((logger)->trace()) {\
	char buf[1024]; \
	snprintf(buf, sizeof(buf) - 1, fmt, ##args); \
    (logger)->trace(buf); }} while(false)


#define LOG_DEBUG(logger, fmt, args...) do { \
    if ((logger)->debug()) {\
	char buf[1024]; \
	snprintf(buf, sizeof(buf) - 1, fmt, ##args); \
    (logger)->debug(buf); }} while(false)
	    
	    
#define LOG_INFO(logger, fmt, args...) do { \
    if ((logger)->information()) {\
	char buf[1024]; \
	snprintf(buf, sizeof(buf) - 1, fmt, ##args); \
    (logger)->information(buf); }} while(false)	    
	    
	    
#define LOG_WARNING(logger, fmt, args...) do { \
    if ((logger)->warning()) {\
	char buf[1024]; \
	snprintf(buf, sizeof(buf) - 1, fmt, ##args); \
    (logger)->warning(buf); }} while(false)	
	    
	    
	    
#define LOG_ERROR(logger, fmt, args...) do { \
    if ((logger)->error()) {\
	char buf[1024]; \
	snprintf(buf, sizeof(buf) - 1, fmt, ##args); \
    (logger)->error(buf); }} while(false)	
	    
	    
	    