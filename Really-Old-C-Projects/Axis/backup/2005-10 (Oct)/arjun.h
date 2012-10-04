/*****************************
  -------- arjun.h ---------
*****************************/

/*
 *  Include this header file in all
 *  programs authored by Arjun M.
 */

/* aliased forms of the C loops */
#define repeat(count,loop)     for(count=0;count<loop;i++)
#define sigma(count,init,loop) for(count=init;count<=loop;i++)
#define forever                while(1)  /* infinite loop */

/* - an expressive form of protoyping - */
#define PROTOTYPE(function, params)  function params

/* aliases for comm. used datatypes */
#define num unsigned long
#define snum unsigned short

/* sizes of basic qts. */
#define BLOCK_SIZE 512 
#define KB_SIZE 1024
#define MB_SIZE 1048576

/* conditionals */
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
