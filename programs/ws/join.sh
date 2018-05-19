# First argument is output file
# Second argument is C header file
# Remaining arguments are wsa files to join
SCRIPTDIR=$(dirname "$0")
cat ${@:3} > tmp.c
sed -E -i 's/#.+$//g' tmp.c
gcc -E -P -include $2 tmp.c > $1
rm tmp.c
