//TEST_SERVER.C
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "server_code.h"  // This would include the prototypes for your server functions

void test_register_user() {
    // Reset global variables for a clean test environment
    userCount = 0;

    // Simulate client socket (use any integer, as it's not actually used in unit tests)
    int client_socket = 1;
    registerUser("testUser", "testPassword", "1234567890", client_socket);

    // Verify user was registered correctly
    CU_ASSERT_EQUAL(userCount, 1);
    CU_ASSERT_STRING_EQUAL(users[0].username, "testUser");
    CU_ASSERT_STRING_EQUAL(users[0].password, "testPassword");
    CU_ASSERT_STRING_EQUAL(users[0].phone_no, "1234567890");
    CU_ASSERT_EQUAL(users[0].is_registered, 1);
}

void test_authenticate_user() {
    // Simulate a user already in the system
    userCount = 0;
    strcpy(users[0].username, "testUser");
    strcpy(users[0].password, "testPassword");
    strcpy(users[0].phone_no, "1234567890");
    users[0].is_registered = 1;
    userCount = 1;

    // Simulate client socket
    int client_socket = 1;

    // Test successful authentication
    authenticateUser("testUser", "1234567890", "testPassword", client_socket);

    // Here you could mock the socket send function to verify what is sent back to the client

    // Test failed authentication
    authenticateUser("wrongUser", "1234567890", "wrongPassword", client_socket);
    // Again, verify the expected failure message
}

void test_activate_call_forwarding() {
    // Simulate a registered user with forwarding details
    forwardingCount = 0;
    strcpy(userForwardings[0].username, "testUser");
    userForwardings[0].is_forwarding_active = 0;
    forwardingCount = 1;

    int client_socket = 1;
    activateCallForwarding("testUser", "Unanswered", "1234567890", "9876543210", client_socket);

    CU_ASSERT_EQUAL(userForwardings[0].is_forwarding_active, 1);
    CU_ASSERT_STRING_EQUAL(userForwardings[0].forwarding_type, "Unanswered");
    CU_ASSERT_STRING_EQUAL(userForwardings[0].phone_no, "1234567890");
    CU_ASSERT_STRING_EQUAL(userForwardings[0].destination_number, "9876543210");
}

void test_deactivate_call_forwarding() {
    // Simulate a registered user with forwarding activated
    forwardingCount = 0;
    strcpy(userForwardings[0].username, "testUser");
    userForwardings[0].is_forwarding_active = 1;
    strcpy(userForwardings[0].forwarding_type, "Unanswered");
    strcpy(userForwardings[0].destination_number, "9876543210");
    forwardingCount = 1;

    int client_socket = 1;
    deactivateCallForwarding("testUser", "1234567890", client_socket);

    CU_ASSERT_EQUAL(userForwardings[0].is_forwarding_active, 0);
    CU_ASSERT_STRING_EQUAL(userForwardings[0].forwarding_type, "");
    CU_ASSERT_STRING_EQUAL(userForwardings[0].destination_number, "");
}

int main() {
    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Create a test suite
    CU_pSuite suite = CU_add_suite("Server Function Tests", 0, 0);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add tests to the suite
    if (!CU_add_test(suite, "test_register_user", test_register_user) ||
        !CU_add_test(suite, "test_authenticate_user", test_authenticate_user) ||
        !CU_add_test(suite, "test_activate_call_forwarding", test_activate_call_forwarding) ||
        !CU_add_test(suite, "test_deactivate_call_forwarding", test_deactivate_call_forwarding)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Run the tests and show the results
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}