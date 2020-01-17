# shellcheck disable=SC2010
set -e
for i in $(ls -l | grep -v "^d" | grep test_ | rev | cut -d" " -f1 | rev); do ./$i; done;
