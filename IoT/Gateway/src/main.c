/*
 *  KEM IoT Program for Raspberry Pi
 *  Munseong Jeong
 *  msjeong@daejin.ac.kr
 *  -------------------------------------
 *  mode    : alpha, beta, release
 *  debug   : log, print
 *  alpha   : *1();
 *  beta    : *2();
 *  release : *3();
 */
#include <stdio.h>
#include <sys/types.h>  /*  size_t at process.h */
#include <stdint.h>     /*  uint8_t */

#include "util.h"       /*  NUM_THING   */
#include "process.h"    /*  init(), process(), final()*/

int main(int argc, char *argv[])
{
    if (init()) {

        return 1;
    }
    process();
    if (finish()) {

        return 1;
    }

    return 0;
}
