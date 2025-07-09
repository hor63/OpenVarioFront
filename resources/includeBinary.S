
/*
 * Include the file FILE_NAME into the program.
 *
 * It defines the global symbols NAME, NAME_end and NAME_size
 * The symbols are to be declared as follows in C++:
 *
 * You can adjust NAME and FILE_NAME with macro definitions on the command line
 * when you are using gcc as assembler driver like:
 gcc -c -o myIcon.o -DFILE_NAME=/path/myIcon.png -DNAME=myIcon includeBinary.S
 * This command generates the equivalent C++ declarations:
 extern "C" {
 extern const char myIcon[];
 extern const char myIcon_end[];
 extern const int  myIcon_size;
 }
 * To resolve the referenced symbols you must link myIcon.o to your program.
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
