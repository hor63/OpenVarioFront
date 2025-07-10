
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

    .global xcatStr(NAME,_data)
    .type   xcatStr(NAME,_data), @object
    .global xcatStr(NAME,_data_end)
    .type   xcatStr(NAME,_data_end), @object
    .align  8
xcatStr(NAME,_data):
    .incbin xstr(FILE_NAME)
xcatStr(NAME,_data_end):

    .global xcatStr(NAME,_size)
    .type   xcatStr(NAME,_size), @object
    .align  8
xcatStr(NAME,_size):
    .int    xcatStr(NAME,_data_end) - xcatStr(NAME,_data)

    .global xcatStr(NAME,_size)
    .type   xcatStr(NAME,_size), @object
    .align  8

    .global xcatStr(NAME,_filename)
    .type   xcatStr(NAME,_filename), @object
    .align  8
xcatStr(NAME,_filename):
    .string xstr(BASE_FILENAME)
xcatStr(NAME,_filename_end):


    .global xcatStr(NAME,_filename_length)
    .type   xcatStr(NAME,_filename_length), @object
xcatStr(NAME,_filename_length):
    .int xcatStr(NAME,_filename_end) - xcatStr(NAME,_filename) - 1
