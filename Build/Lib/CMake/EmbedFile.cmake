#
# cmake -DINPUT=<file> -DNAME=<name> -DOUTPUT=<file> -P EmbedFile.cmake
#
# Writes OUTPUT holding INPUT's bytes as a C array, as xxd -i NAME writes them:
#     unsigned char <C name>[] = {
#       0x.., ...
#     };
#     unsigned int <C name>_len = <size>;
# the C name being NAME with each character but a letter or digit made '_' - so the CMake build needs no xxd.
#
file (READ "${INPUT}" hex_ HEX)
string (LENGTH "${hex_}" hexDigits_)
math (EXPR size_ "${hexDigits_} / 2")
string (REGEX REPLACE "[^A-Za-z0-9]" "_" cName_ "${NAME}")
string (REGEX REPLACE "(..)" "0x\\1, " bytes_ "${hex_}")
# 12 bytes to a line, as xxd: spelled out, as CMake's regular expressions have no {12}
set (line_ "0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., ")
string (REGEX REPLACE "(${line_})" "\\1\n" bytes_ "${bytes_}")
string (REPLACE ", \n" ",\n  " bytes_ "${bytes_}")
string (REGEX REPLACE "[, \n]+$" "" bytes_ "${bytes_}")
# LF line ends, as xxd, on Windows too (where file (WRITE) would write CRLF)
file (CONFIGURE OUTPUT "${OUTPUT}" CONTENT "unsigned char ${cName_}[] = {\n  ${bytes_}\n};\nunsigned int ${cName_}_len = ${size_};\n" @ONLY NEWLINE_STYLE UNIX)
