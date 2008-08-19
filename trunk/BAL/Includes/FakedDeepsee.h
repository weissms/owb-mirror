//Faked deepsee.h:
//	generated on: vendredi 22 février 2008, 16:45:07 (UTC+0100)
//	from sources (.*\.cpp$\|.*\.h$\|.*\.cxx$\|.*\.hxx$) found under: /home/fred/src/deepsee/src
//	by script: ./deepsee_generateFakedHeader.sh
//To regenerate this header, just re-run ./deepsee_generateFakedHeader.sh, or delete the file and re-configure cmake if it uses the DeepSee block.
//
//You can get a fresh DeepSee from svn at http://www.sand-labs.org/deepsee/svn/trunk/
//And visit DeepSee project homepage at http://www.sand-labs.org/deepsee/
//

/*
    This header is generated directly from DeepSee sources, so all current (2008-02-22) macros from the API are stubbed here.
    We needed this because we want to offer our users basic logs even if they are not (yet) using DeepSee.
    For exemple, even if DeepSee is disabled, DS_ASS
*/

#ifndef FAKEDDEEPSEE_H
#define FAKEDDEEPSEE_H

//if we don't use DeepSee, but want to redefine few macros instead of stubbing them
#ifdef OWB_FAILSAFE_WITHOUT_DEEPSEE

//the file that defines ASSERT, in JavaScriptCore/wtf/Assertions.h
#include "wtf/Assertions.h"

#define DS_ASS(var1,cond,var2) ASSERT(var1 cond var2)
#define DS_ASS_LOW(var1,cond,var2) ASSERT(var1 cond var2)
#define DS_ASS_PTR(ptr) ASSERT(ptr)

#else

#define DS_ASS(...)
#define DS_ASS_LOW(...)
#define DS_ASS_PTR(...)

#endif

//all other macros are stubbed

#define DS_BACK_TRACE(...)
#define DS_BACK_TRACE_SKIP(...)
#define DS_CLEAN_DEEPSEE_FRAMEWORK(...)
#define DS_CONSTRUCT(...)
#define DS_DEFINE_CONSOLE_PTR(...)
#define DS_DELETE_CONSOLE(...)
#define DS_DESTRUCT(...)
#define DS_ERR(...)
#define DS_GET_USEDMEMORYSIZE(...)
#define DS_INIT_DEEPSEE_FRAMEWORK(...)
#define DS_INSTANCE_COUNT_ALLOCATE(...)
#define DS_INST_CHECK_LEAK(...)
#define DS_INST_CHECK_LEAK_FOR_CLASS(...)
#define DS_INST_DIFF(...)
#define DS_INST_DIFF_TO_SNAPSHOT(...)
#define DS_INST_DUMP_CURRENT(...)
#define DS_INST_DUMP_SNAPSHOT(...)
#define DS_INST_GET_CURRENT_NB(...)
#define DS_INST_GET_SNAPSHOT_NB(...)
#define DS_INST_REMOVE_SNAPSHOT(...)
#define DS_INST_TAKE_SNAPSHOT(...)
#define DS_LOGGER_ADD_CMD_TO_CONSOLE(...)
#define DS_LOGGER_CLEAR_FILTERS(...)
#define DS_LOGGER_DUMP(...)
#define DS_LOGGER_SET_FILENAME_HEAD_TO_REMOVE(...)
#define DS_MEMORY_ADD_CMD_TO_CONSOLE(...)
#define DS_MEMORY_CHECK_LEAK(...)
#define DS_MEMORY_LEAK(...)
#define DS_MEMORY_TAKE_SNAPSHOT_TO_STACK(...)
#define DS_MSG(...)
#define DS_NEW_CONSOLE(...)
#define DS_SIGNAL_CATCH_ALL(...)
#define DS_SIGNAL_CATCH(...)
#define DS_SRR(...)
#define DS_STACK_GET_USEDMEMORYSIZE(...)
#define DS_TIMER_(...)
#define DS_TIMER_DISPLAY(...)
#define DS_TIMER_DISPLAY_DIFF(...)
#define DS_TIMER_STACK_CREATE(...)
#define DS_TIMER_STACK_GET(...)
#define DS_TIMER_STACK_RESET(...)
#define DS_TIMER_STACK_SET(...)
#define DS_TIMER_STATIC_DISPLAY(...)
#define DS_TIMER_STATIC_GET(...)
#define DS_TIMER_STATIC_MAX(...)
#define DS_TIMER_STATIC_RESET(...)
#define DS_TIMER_STATIC_SET(...)
#define DS_TIMER_STATIC_SUM_DISPLAY(...)
#define DS_TIMER_STATIC_SUM_END(...)
#define DS_TIMER_STATIC_SUM_GET(...)
#define DS_TIMER_STATIC_SUM_RESET(...)
#define DS_TIMER_STATIC_SUM_START(...)
#define DS_TRC(...)
#define DS_USE_CONSOLE(...)
#define DS_WAR(...)
#define DS_WAR_IF(...)
#define DS_WAR_IF_novalue(...)
#define DS_WAR_RAW(...)


#endif

