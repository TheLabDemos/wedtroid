#ifndef BASETYPES
#define BASETYPES
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
#endif  /* !BASETYPES */

#ifndef UNSIGNEDINT
#define UNSIGNEDINT
typedef unsigned int UINT;
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef IMAK_DEFS
#define IMAK_DEFS
enum IMAKenum
{
    IMAK_ADD,
    IMAK_MUL,
    IMAK_SUB,
    IMAK_DIV
};
#endif