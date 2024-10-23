#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <string.h>
#include <stdio.h>
#include "rclient.h" 

#define BUFFER_SIZE 256

/* Global mock buffer to simulate send/recv data */
char mockBuffer[BUFFER_SIZE];

/* Mock send function */
ssize_t mock_send(int sockfd, const void *buf, size_t len, int flags) {
    strncpy(mockBuffer, (char *)buf, len);
    mockBuffer[len] = '\0';  // Null-terminate for safety
    return len;
}

/* Mock recv function */
ssize_t mock_recv(int sockfd, void *buf, size_t len, int flags) {
    strncpy((char *)buf, mockBuffer, len);
    ((char *)buf)[len - 1] = '\0';  // Ensure null-termination
    return strlen(buf);
}

/* Utility to mock user input (stdin redirection) */
void setMockInput(const char *input) {
    FILE *mockInput = fopen("/tmp/mockInput.txt", "w+");
    fprintf(mockInput, "%s", input);
    fclose(mockInput);
    freopen("/tmp/mockInput.txt", "r", stdin);
}

/* Utility to mock expected server responses */
void setMockResponse(const char *response) {
    strcpy(mockBuffer, response);  // Simulate the server's response
}

/* Test case: registerUser with valid input */
void test_registerUser_valid() {
    setMockInput("testuser\ntestpass\n1234567890\n");
    char expected[] = "REGISTER testuser testpass 1234567890";
    registerUser(1);  // Call registerUser

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);  // Check sent buffer
}

/* Test case: registerUser with invalid phone number */
void test_registerUser_invalidPhoneNumber() {
    setMockInput("testuser\ntestpass\ninvalidPhone\n");
    registerUser(1);  // Call registerUser

    CU_ASSERT_STRING_NOT_EQUAL(mockBuffer, "REGISTER testuser testpass invalidPhone");
}

/* Test case: loginUser with valid input */
void test_loginUser_valid() {
    setMockInput("testuser\n1234567890\ntestpass\n");
    char expected[] = "LOGIN testuser 1234567890 testpass";
    loginUser(1);  // Call loginUser

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);
}

/* Test case: activateCallForwarding with valid input */
void test_activateCallForwarding_valid() {
    setMockInput("testuser\nUnconditional\n1234567890\n0987654321\n");
    char expected[] = "ACTIVATE testuser Unconditional 1234567890 0987654321";
    activateCallForwarding(1);  // Call activateCallForwarding

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);
}

/* Test case: deactivateCallForwarding with valid input */
void test_deactivateCallForwarding_valid() {
    setMockInput("testuser\n1234567890\n");
    char expected[] = "DEACTIVATE testuser 1234567890";
    deactivateCallForwarding(1);  // Call deactivateCallForwarding

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);
}

/* Test case: makeCall with valid input */
void test_makeCall_valid() {
    setMockInput("1234567890\ncalleeUser\n0987654321\n");
    char expected[] = "CALL 1234567890 calleeUser 0987654321";
    makeCall(1);  // Call makeCall

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);
}

/* Test case: displaying call log */
void test_displayCallLog() {
    setMockResponse("Call Log: [caller1, caller2, caller3]");
    displayCallLog(1);

    CU_ASSERT_STRING_EQUAL(mockBuffer, "Call Log: [caller1, caller2, caller3]");
}

/* Test case: special characters in registerUser */
void test_registerUser_specialCharacters() {
    setMockInput("user!@#\npass$%^&\n0987654321\n");
    char expected[] = "REGISTER user!@# pass$%^& 0987654321";
    registerUser(1);

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);
}

/* Test case: registerUser with empty inputs */
void test_registerUser_emptyInputs() {
    setMockInput("\n\n\n");
    char expected[] = "REGISTER   ";
    registerUser(1);

    CU_ASSERT_STRING_EQUAL(mockBuffer, expected);
}

/* Main test runner */
int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("rclient_test_suite", 0, 0);

    /* Add all test cases */
    CU_add_test(suite, "test_registerUser_valid", test_registerUser_valid);
    CU_add_test(suite, "test_registerUser_invalidPhoneNumber", test_registerUser_invalidPhoneNumber);
    CU_add_test(suite, "test_loginUser_valid", test_loginUser_valid);
    CU_add_test(suite, "test_activateCallForwarding_valid", test_activateCallForwarding_valid);
    CU_add_test(suite, "test_deactivateCallForwarding_valid", test_deactivateCallForwarding_valid);
    CU_add_test(suite, "test_makeCall_valid", test_makeCall_valid);
    CU_add_test(suite, "test_displayCallLog", test_displayCallLog);
    CU_add_test(suite, "test_registerUser_specialCharacters", test_registerUser_specialCharacters);
    CU_add_test(suite, "test_registerUser_emptyInputs", test_registerUser_emptyInputs);

    /* Run tests */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}