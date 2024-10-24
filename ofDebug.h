#ifndef __OPEN_FLOW_DEBUG_H__
    #define __OPEN_FLOW_DEBUG_H__
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
	#include <cstdlib>
    #include <cstdint>
	#include <cstdarg>
	#include <cstdio>
	#include <iostream>
	#include <iomanip>
	#include <cstring>
	#include <string>
	#include <cstdint>
	#include <cassert>
	#include <list>
	#include <array>

	#define THIS_FILE ((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)

    // Debugging parameters & macros
    extern uint32_t gDbgMask; // mask to control debugging level
    /// Need to be implemented
    std::string CofaLog(const char* format,...)  __attribute__ ((format (printf, 1, 2)));

    typedef enum {
        DBG_NONE_OF    =         0x0,          // debug printouts are disabled
        DBG_MGR     =        0x00000001,   // mgr
        DBG_DP      =         0x00000002,   // Datapath entry singleton
        DBG_LOGICAL_SW   =   0x00000004,   // logical sw
        DBG_FLOW_TABLE   =   0x00000008,   // flow table
        DBG_FLOW_ENTRY   =   0x00000010,   // flow entry
        DBG_GROUP_TABLE  =   0x00000020,   // group table
        DBG_GROUP_ENTRY  =   0x00000040,   // group entry
        DBG_RSRC_TABLE   =   0x00000080,   // resource table
        DBG_RSRC_ENTRY   =   0x00000100,   // resource entry
        DBG_OBJ          =   0x00000200,   // obj singleton
        DBG_OBJ_TABLE    =   0x00000400,   // obj table
        DBG_OBJ_ENTRY    =   0x00000800,   // obj entry
        DBG_FACTORY      =   0x00001000,   // factory
        DBG_PARSER_PARSE =   0x00002000,   // parser log through Receive()
        DBG_PARSER_SOCKET=   0x00004000,   // parser log through SslClient
        DBG_PARSER_STATE =   0x00008000,   // parser log through StateContext and StateObject
        DBG_PARSER_ALL   =   (DBG_PARSER_PARSE|DBG_PARSER_SOCKET|DBG_PARSER_STATE),
        DBG_DETAIL       =   0x20000000,   // detail debug enable
        DBG_TRACE        =   0x40000000,   // tracing debug enable
        DBG_ERR_OF       =   0x80000000,   // error debug enable
        DBG_ALL_OF       =   0xFFFFFFFF   // all debugging enable
    }  of_debug_level;

    typedef enum {
        DBG_PERF_PARSER     = 0x00000000,
        DBG_PERF_SEARCH     = 0x00000001,
        DBG_PERF_AGENT_DB   = 0x00000002,
    } perf_module;

/// generic debug macro
/// for SendLogToCofa printData must be (const char* msg)
/// for the time being
	#ifdef STANDALONE
		#define DBG(debugMask, printData)                     \
		{                                                     \
		  if ((gDbgMask & (debugMask)) ==  (debugMask))       \
		  {                                                   \
			 std::string data = CofaLog printData;			  \
			 printf(" OFP(%s:%s:%i): [%s]\n", THIS_FILE, __PRETTY_FUNCTION__, __LINE__, data.c_str()); \
		  }                                                   \
		}

		#define DBG_SIMPLE(debugMask, printData)                  \
		{                                                         \
			if ((gDbgMask & (debugMask)) ==  (debugMask))         \
			{                                                     \
				 std::string data = CofaLog printData;			  \
				 printf(" OFP[%s] ", data.c_str());               \
			}                                                     \
		}
	#else
		#define DBG(debugMask, printData)                     \
		{                                                     \
		  if ((gDbgMask & (debugMask)) ==  (debugMask))       \
		  {                                                   \
			 std::string data = CofaLog printData;			  \
		  }                                                   \
		}

		#define DBG_SIMPLE(debugMask, printData)                        \
		{                                                              	\
			if ((gDbgMask & (debugMask)) ==  (debugMask))               \
			{                                                           \
				std::string data = CofaLog printData;			  		\
			}                                                          	\
		}
	#endif
// displays detail information that does not prepend function and line information
	#define DBG_DETAILS(printData) DBG(DBG_DETAIL, printData)

// displays tracing information while entering a function
// this is not for remote logger
	#define DBG_TRACE_ENTER DBG(DBG_TRACE,("ENTER ->\n"))

	#define DBG_TRACE(debugMask) DBG(debugMask,("\n"))

// displays tracing information while exiting a function without a return value
	#define DBG_TRACE_EXIT DBG(DBG_TRACE,("EXIT <-\n"))

// displays tracing information while exiting a function with a return value
	#define DBG_TRACE_RET(s)                               \
	  DBG(DBG_TRACE,("EXIT: %s <-\n", to_string((s)))); \
	  return ((s))

/// Synchronous response - previously DBG_TRACE_RET_BOOL
/// For Async return - just simply return false
/// For Sync return - it will always return true
/// Used in the parser mainly
	#define DBG_TRACE_RET_SYNC(s, ret_val)                  \
	  (ret_val) = (s);                                     \
	  DBG(DBG_TRACE,("EXIT: %s <-\n", to_string((s)))); \
	  return (true)

// displays tracing information while exiting a function with an OfStatus enum return
	#define DBG_TRACE_STATUS_RET(s)                               \
	  DBG(DBG_TRACE,("EXIT: %s <-\n", to_string((s)))); \
	  return ((s))

// displays error information
	#define DBG_ERROR(printData) DBG(DBG_ERR_OF, printData)

#endif
