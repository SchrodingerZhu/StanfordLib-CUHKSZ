# shellcheck disable=SC2010
set -e
./test_locks
./test_queue
./test_strlib
./test_vector

