/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include "cmock.h"
#include <stdio.h>
#include <string.h>
#include "Mockperiodic_callbacks.h"
#include "Mocktask.h"

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_periodic_scheduler__initialize(void);


/*=======Mock Management=====*/
static void CMock_Init(void)
{
  Mockperiodic_callbacks_Init();
  Mocktask_Init();
}
static void CMock_Verify(void)
{
  Mockperiodic_callbacks_Verify();
  Mocktask_Verify();
}
static void CMock_Destroy(void)
{
  Mockperiodic_callbacks_Destroy();
  Mocktask_Destroy();
}

/*=======Test Reset Options=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  CMock_Verify();
  CMock_Destroy();
  CMock_Init();
  setUp();
}
void verifyTest(void);
void verifyTest(void)
{
  CMock_Verify();
}
/*=======Test Runner Used To Run Each Test=====*/
static void run_test(UnityTestFunction func, const char* name, int line_num)
{
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = line_num;
#ifdef UNITY_USE_COMMAND_LINE_ARGS
    if (!UnityTestMatches())
        return;
#endif
    Unity.NumberOfTests++;
    UNITY_CLR_DETAILS();
    UNITY_EXEC_TIME_START();
    CMock_Init();
    if (TEST_PROTECT())
    {

            setUp();
            func();

    }
    if (TEST_PROTECT())
    {
        tearDown();
        CMock_Verify();
    }
    CMock_Destroy();
    UNITY_EXEC_TIME_STOP();
    UnityConcludeTest();
}


/*=======MAIN=====*/
int main(void)
{
  UnityBegin("projects/lpc40xx_freertos/l5_application/periodics/test/test_periodic_scheduler.c");
  run_test(test_periodic_scheduler__initialize, "test_periodic_scheduler__initialize", 31);

  CMock_Guts_MemFreeFinal();
  return UnityEnd();
}
