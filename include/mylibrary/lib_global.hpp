#ifndef C_TEST_LIB_GLOBAL_HPP
#define C_TEST_LIB_GLOBAL_HPP

namespace my_library {

#define lib_printError(message) fprintf(stderr, "Lib Function Error: "#message" at %s[%d]\n", __FILE__, __LINE__)

#define lib_checkArgument(argument, returnValue) \
    if(argument == null) { lib_printError("Argument "#argument" is null!"); return returnValue; }
#define lib_checkArgumentNoRetVal(argument) \
    if(argument == null) { lib_printError("Argument "#argument" is null!"); return; }

#define null nullptr
#undef NULL

#define LIB_ILLEGAL_ARGUMENT (-2)
#define LIB_EXECUTION_FAILED (-1)
#define LIB_FUNCTION_SUCCESS 0

}

#endif //C_TEST_LIB_GLOBAL_HPP
