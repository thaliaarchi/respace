# ./preprocess.sh <input> <output>
for i in {$1,$(find . -type f -name "*.wsa")}; do
    cp "$i" "$i.tmp.c"
    # Remove comments and make preprocessor directives start with #
    sed -E -i '
        s/\s*#.+$//g
        s/^\s*@/#/g
        s/^#include\s*"(.+?)"/#include "\1.tmp.c"/g
    ' "$i.tmp.c"
done
gcc -E -P "$1.tmp.c" > "$2"
# Expand semicolon into newline
sed -E -i "s/\s*;\s*/\n/g" "$2"
find . -type f -name '*.tmp.c' -exec rm {} +
