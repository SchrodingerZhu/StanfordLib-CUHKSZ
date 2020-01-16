for i in $(ls -l | grep -v "^d" | grep test_ | rev | cut -d" " -f1 | rev); do xvfb-run ./$i; done;
