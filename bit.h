// ======================================================================
//
// bit.h
// jeff grills
//
// copyright 1997 origin systems inc
//
// ======================================================================

#ifndef BIT_H
#define BIT_H

// ======================================================================

#define BIT(b)         (1u << (b))
#define BIT_NOT(b)     (~BIT(b))

// ======================================================================

#define BINHEX_
#define BINHEX_0    0
#define BINHEX_1    1
#define BINHEX_10   2
#define BINHEX_11   3
#define BINHEX_100  4
#define BINHEX_101  5
#define BINHEX_110  6
#define BINHEX_111  7
#define BINHEX_0000 0
#define BINHEX_0001 1
#define BINHEX_0010 2
#define BINHEX_0011 3
#define BINHEX_0100 4
#define BINHEX_0101 5
#define BINHEX_0110 6
#define BINHEX_0111 7
#define BINHEX_1000 8
#define BINHEX_1001 9
#define BINHEX_1010 A
#define BINHEX_1011 B
#define BINHEX_1100 C
#define BINHEX_1101 D
#define BINHEX_1110 E
#define BINHEX_1111 F

#define BINARY_HEX1(a)               (0x ## a)
#define BINARY_ARG1(a)               BINARY_HEX1(a)
#define BINARY1(a)                   BINARY_ARG1(BINHEX_ ## a)

#define BINARY_HEX2(a,b)             (0x ## a ## b)
#define BINARY_ARG2(a,b)             BINARY_HEX2(a,b)
#define BINARY2(a,b)                 BINARY_ARG2(BINHEX_ ## a, BINHEX_ ## b)

#define BINARY_HEX3(a,b,c)           (0x ## a ## b ## c)
#define BINARY_ARG3(a,b,c)           BINARY_HEX3(a,b,c)
#define BINARY3(a,b,c)               BINARY_ARG3(BINHEX_ ## a, BINHEX_ ## b, BINHEX_ ## c)

#define BINARY_HEX4(a,b,c,d)         (0x ## a ## b ## c ## d)
#define BINARY_ARG4(a,b,c,d)         BINARY_HEX4(a,b,c,d)
#define BINARY4(a,b,c,d)             BINARY_ARG4(BINHEX_ ## a, BINHEX_ ## b, BINHEX_ ## c, BINHEX_ ## d)

#define BINARY_HEX5(a,b,c,d,e)       (0x ## a ## b ## c ## d ## e)
#define BINARY_ARG5(a,b,c,d,e)       BINARY_HEX5(a,b,c,d,e)
#define BINARY5(a,b,c,d,e)           BINARY_ARG5(BINHEX_ ## a, BINHEX_ ## b, BINHEX_ ## c, BINHEX_ ## d, BINHEX_ ## e)

#define BINARY_HEX6(a,b,c,d,e,f)     (0x ## a ## b ## c ## d ## e ## f)
#define BINARY_ARG6(a,b,c,d,e,f)     BINARY_HEX6(a,b,c,d,e,f)
#define BINARY6(a,b,c,d,e,f)         BINARY_ARG6(BINHEX_ ## a, BINHEX_ ## b, BINHEX_ ## c, BINHEX_ ## d, BINHEX_ ## e, BINHEX_ ## f)

#define BINARY_HEX7(a,b,c,d,e,f,g)   (0x ## a ## b ## c ## d ## e ## f ## g)
#define BINARY_ARG7(a,b,c,d,e,f,g)   BINARY_HEX7(a,b,c,d,e,f,g)
#define BINARY7(a,b,c,d,e,f,g)       BINARY_ARG7(BINHEX_ ## a, BINHEX_ ## b, BINHEX_ ## c, BINHEX_ ## d, BINHEX_ ## e, BINHEX_ ## f, BINHEX_ ## g)

#define BINARY_HEX8(a,b,c,d,e,f,g,h) (0x ## a ## b ## c ## d ## e ## f ## g ## h)
#define BINARY_ARG8(a,b,c,d,e,f,g,h) BINARY_HEX8(a,b,c,d,e,f,g,h)
#define BINARY8(a,b,c,d,e,f,g,h)     BINARY_ARG8(BINHEX_ ## a, BINHEX_ ## b, BINHEX_ ## c, BINHEX_ ## d, BINHEX_ ## e, BINHEX_ ## f, BINHEX_ ## g, BINHEX_ ## h)

#define BINARY(a,b,c,d,e,f,g,h)      BINARY8(a,b,c,d,e,f,g,h)

// ======================================================================

#endif
