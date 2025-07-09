
/*
 * Include the file FILE_NAME into the program.
 *
 * It defines the global symbols NAME, NAME_end and NAME_size
 * The symbols are to be declared as follows in C++:

 extern "C" {
 extern const char NAME[];
 extern const char NAME_end[];
 extern const int  NAME_size;
 }

*/

#define catStr(name,suffix) name ## suffix
#define xcatStr(name,suffix) catStr(name,suffix)
#define str(s) #s
#define xstr(s) str(s)

/* Make the GNU chain happy */
.section .note.GNU-stack,"",@progbits

    .section .rodata

    .global NAME
    .type   NAME, @object
    .global xcatStr(NAME,_end)
    .type   xcatStr(NAME,_end), @object
    .align  8
NAME:
    .incbin xstr(FILE_NAME)
xcatStr(NAME,_end):
    .global xcatStr(NAME,_size)
    .type   xcatStr(NAME,_size), @object
    .align  8
xcatStr(NAME,_size):
    .int    xcatStr(NAME,_end) - NAME
