set(TEST_NAMES
    test_eval
    test_result
    test_tokenizer
    test_parser
    test_interpreter
    )

foreach(TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach(TEST_NAME)
