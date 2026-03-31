
/*
 * Include the file FILE_NAME into a linkable object file.
 *
 * You must set NAME, FILE_NAME and BASE_FILENAME with macro definitions on the command line
 *
 * NAME becomes the base for constant definitions emitted by the generated oject file.
 * FILE_NAME is the path to the file which you want to include in your executable.
 * BASE_FILENAME is intended to be the basename of the file which you are including.
 *  It is however not interpreted in any way. The content is placed into a string constant
 *  for informational use in the program.
 * 
 * Here an example how to compile a file into a linkable object file using this assembler source
gcc -c -o myIcon.o -DNAME=myIcon -DFILE_NAME=/path/myIcon.png -DBASE_FILENAME=myIcon.png \
    "${srcdir}/includeBinary.S"
 *
 * This command generates the equivalent C/C++ declarations:
#if defined __cplusplus
extern "C" {
#endif

// Points to the start of the binary content
extern const char myIcon_data [];
// Points to the address after the memory content.
extern const char myIcon_data_end [];
// Size in bytes of the memory content
extern const int myIcon_size;

extern const char myIcon_filename [];
// The length of myIcon_filename without the terminating \0 
extern int myIcon_filename_length;
	
#if defined __cplusplus
}
#endif
 *
 * You can use the script binary2obj.sh to compile the input file into a
 * linkable ojbect, and to create a header file which contains the 
 * C/C++ declarations.
 *
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

    .global xcatStr(NAME,_filename)
    .type   xcatStr(NAME,_filename), @object
    .align  8
xcatStr(NAME,_filename):
    .string xstr(BASE_FILENAME)
xcatStr(NAME,_filename_end):


    .global xcatStr(NAME,_filename_length)
    .type   xcatStr(NAME,_filename_length), @object
    .align  8
xcatStr(NAME,_filename_length):
    .int xcatStr(NAME,_filename_end) - xcatStr(NAME,_filename) - 1
