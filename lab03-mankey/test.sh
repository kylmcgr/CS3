function do_test() {
    ./main "$1" "$2"
    student=$?
    echo "$2" | grep "^$1$" > /dev/null
    ref=$?
    if [ $student -ne $ref ]; then
        echo "Failed Test: pattern=\"$1\", text=\"$2\"";
        exit 1;
    fi
}

# Write Your Tests Here
# The do_test function tests if your program gives the
# same result as the reference implementation on the pattern
# and text provided.

# tests feature 1
do_test 'adam' 'hello'
do_test 'adam' 'adam'
do_test '' ''
do_test 'hell' 'hello'
do_test 'hello' 'hell'

#tests feature 2
do_test 'ad.m' 'hello'
do_test 'ad.m' 'adam'
do_test '.' ''
do_test 'hel.' 'helll'
do_test 'hell.' 'hell'
do_test '.hello there' 'general kenobi'

#tests feature 3
do_test 'adi*m' 'adaaaaaaaaaaam'
do_test '*' ''
do_test 'ada*m' 'adaaaaaaaaaaam'
do_test '*helo' 'helo'
do_test '*' 'aaaaaaaaaaaaaaaaaa'
do_test 'hea*llo' 'hello'

#tests feature 4
do_test 'ad.*' 'adaaaaaaaaaaam'
do_test 'ad.*m' 'adaaaaaaaaaaam'
do_test 'he.*o there' 'hello ther'
do_test 'gen.*ral .*enobi' 'general kenobi'
do_test 'hel.*o' 'heo'
do_test '.*hell.*' 'hellllllllllo'
do_test '.*hell.*' 'hallhellllllllllo'

do_test '.*..*..*.*' '.*'

echo '-----------------';
echo 'All Tests Passed!';
