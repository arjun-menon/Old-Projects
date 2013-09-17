/*****************************
  -------- arjun.h ---------
*****************************/

/*
 *  Include this header file in all
 *  programs written by Arjun Menon.
 */

/* aliased forms of C loops */
#define repeat(count,loop)     for(count=0;count<loop;i++)
#define sigma(count,init,loop) for(count=init;count<=loop;i++)
#define forever                while(1)  /* infinite loop */

/* an expressive way to prototype functions */
#define PROTOTYPE(function, params) function params

/* aliases for basic datatypes */
#define int16 signed short
#define int32 signed long
#define int64 signed __int64
#define num16 unsigned short
#define num32 unsigned long
#define num64 unsigned __int64
#define uchar unsigned char

/* sizes of basic qts. */
#define KB_SIZE 1024
#define MB_SIZE 1048576

/* conditionals */
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
