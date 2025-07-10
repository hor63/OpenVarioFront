#!/bin/bash

# Script to create an object file by including a file as binary data
# 
# Invocation: output_file input_file file_extension
#
# Enviroment variable: srcdir: Directory there Makefile.am resides.

inputFile="$2"
outputFile="$1"
fileStem="$3"

srcdir=`dirname "$0"`
baseStem=`basename "$fileStem"`
baseName=`basename "$inputFile"`
outputDirectory=`dirname "$outputFile"`
outputHeaderFile="${outputDirectory}/${baseStem}.h"

# echo "inputFile = $inputFile"
# echo "outputFile = $outputFile"
# echo "fileStem = $fileStem"
# echo "baseStem = $baseStem"
# echo "srcdir = $srcdir"
# echo "outputDirectory = $outputDirectory"
# echo "outputHeaderFile = $outputHeaderFile"

# echo "${CCAS} ${CCASFLAGS} -c -o ${outputFile} -DNAME=\"${baseStem}\" -DFILE_NAME=\"${inputFile}\" \"${srcdir}/includeBinary.S\""
${CCAS} ${CCASFLAGS} -c -o "${outputFile}" -DNAME="${baseStem}" \
	-DFILE_NAME="${inputFile}" -DBASE_FILENAME="${baseName}"\
	"${srcdir}/includeBinary.S"

echo "/* file: `basename "$outputHeaderFile"`
 *
 * Generated header for access to the in-memory data of file
 * \"$inputFile\"
 *
 * To refer to and use the declarations below you must link object file
 * \"${outputFile}\" 
 *
 */

#if !${baseStem}_H_INCLUDED

#define ${baseStem}_H_INCLUDED 1

#if defined __cplusplus
extern \"C\" {
#endif

/* Points to the start of the binary content */
extern const char ${baseStem}_data [];
/* Points to the address after the memory content. */
extern const char ${baseStem}_data_end [];
/* Size in bytes of the memory content */
extern const int ${baseStem}_size;

extern const char ${baseStem}_filename [];
/* The length of ${baseStem}_filename without the terminating \\0 */
extern int ${baseStem}_filename_length;
	
#if defined __cplusplus
} /* extern \"C\" { */
#endif

#endif /* #if !${baseStem}_H_INCLUDED */
" > ${outputHeaderFile}
