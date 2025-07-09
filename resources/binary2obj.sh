#!/bin/bash

# Script to create an object file by including a file as binary data
# 
# Invocation: output_file input_file file_extension
#
# Enviroment variable: srcdir: Directory there Makefile.am resides.

inputFile=$2
outputFile=$1
fileExtension=$3

baseName=`basename "$inputFile" $fileExtension`
outputDirectory=`dirname "$outputFile"`
outputHeaderFile=${outputDirectory}/${baseName}.h

echo "inputFile = $inputFile"
echo "outputFile = $outputFile"
echo "fileExtension = $fileExtension"
echo "baseName = $baseName"
echo "outputDirectory = $outputDirectory"
echo "outputHeaderFile = $outputHeaderFile"

echo "${CCAS} ${CCASFLAGS} -c -o ${outputFile} -DNAME=\"${baseName}\" -DFILE_NAME=\"${inputFile}\" \"${srcdir}/includeBinary.S\""
${CCAS} ${CCASFLAGS} -c -o ${outputFile} -DNAME="${baseName}" -DFILE_NAME="${inputFile}" "${srcdir}/includeBinary.S"

echo "/* file: `basename $outputHeaderFile`
 *
 * Generated header for access to the in-memory data of file
 * \"$inputFile\"
 *
 * To refer to and use the declarations below you must link object file
 * ${outputFile} 
 *
 */

#if !${baseName}_H_INCLUDED

#define ${baseName}_H_INCLUDED 1

#if defined __cplusplus
extern \"C\" {
#endif

/* Points to the start of the binary content */
extern const char $baseName [];
/* Points to the address after the memory content. */
extern const char ${baseName}_end [];
/* Size in bytes of the memory content */
extern const int ${baseName}_size;
	
#if defined __cplusplus
} /* extern \"C\" { */
#endif

#endif /* #if !${baseName}_H_INCLUDED */
" > ${outputHeaderFile}
