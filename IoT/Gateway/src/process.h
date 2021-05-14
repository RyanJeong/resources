#ifndef __PROCESS_H__
#define __PROCESS_H__

#define LOG_DIRECTORY               "./log/log.txt"

#define __PROCESS__                 "[process.c]"
#define PROCESS_INIT                "[init(void)] "
#define PROCESS_PROCESS             "[process(void)] "
#define PROCESS_FINISH              "[finish(void)] "

#define MSG_NULL                    ""

#define MSG_ERROR_INIT1             "fopen() failed. Check the directory log."
#define MSG_ERROR_INIT2             "UART setting failed."
#define MSG_ERROR_INIT3             "RF open failed."
#define MSG_ERROR_INIT4             "Failed to create thread."
#define MSG_ERROR_INIT5             "Failed to initialization socket"
#define MSG_ERROR_INIT6             "Failed to initialization the structure"
#define MSG_ERROR_INIT7             "Failed to get MAC address"
#define MSG_COMPLETE_INIT           "Init process has been completed."

#define MSG_ERROR_FINISH1           "Failed to destroy thread."
#define MSG_ERROR_FINISH2           "UART close failed."
#define MSG_ERROR_FINISH3           "fclose() failed."
#define MSG_COMPLETE_FINISH         "Final process has been completed."

#define MSG_START_INFO1             "Start! Mode = alpha, Debug message on the screen"
#define MSG_START_INFO2             "Start! Mode = beta, Debug message on the screen"
#define MSG_START_INFO3             "Start! Mode = release, Debug message on the screen"
#define MSG_START_INFO4             "Start! Mode = alpha, Debug message at log.txt"
#define MSG_START_INFO5             "Start! Mode = beta, Debug message at log.txt"
#define MSG_START_INFO6             "Start! Mode = release, Debug message at log.txt"

#define MSG_FINISH_INFO1            "Finish. Mode = alpha, Debug message on the screen"
#define MSG_FINISH_INFO2            "Finish. Mode = beta, Debug message on the screen"
#define MSG_FINISH_INFO3            "Finish. Mode = release, Debug message on the screen"
#define MSG_FINISH_INFO4            "Finish. Mode = alpha, Debug message at log.txt"
#define MSG_FINISH_INFO5            "Finish. Mode = beta, Debug message at log.txt"
#define MSG_FINISH_INFO6            "Finish. Mode = release, Debug message at log.txt"

#define MSG_START_PROC              "Start process()."
#define MSG_FINISH_PROC             "Finish procesS()."

/*
 * size_t init(void);
 * : performs initialization before the program running
 */
size_t init(void);

/*
 * void process(void);
 * : a main routine of the program
 */
void process(void);

/*
 * size_t finish(void);
 * : After the program operation is completed, perform the final action
 */
size_t finish(void);

#endif
