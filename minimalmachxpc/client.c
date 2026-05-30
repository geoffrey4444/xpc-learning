#include <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <xpc/xpc.h>

int main(void) {
  xpc_connection_t connection = xpc_connection_create_mach_service(
      "com.geoffrey4444.minimal", dispatch_get_main_queue(), 0);
  if (!connection) {
    fprintf(stderr, "could not connect to service\n");
    exit(EXIT_FAILURE);
  }

  xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
    if (xpc_get_type(event) == XPC_TYPE_ERROR) {
      fprintf(stderr, "connection error: %s\n",
              xpc_dictionary_get_string(event, XPC_ERROR_KEY_DESCRIPTION));
      return;
    }
  });
  xpc_connection_activate(connection);

  xpc_object_t message = xpc_dictionary_create(NULL, NULL, 0);
  xpc_dictionary_set_string(message, "command", "ping");

  xpc_object_t response =
      xpc_connection_send_message_with_reply_sync(connection, message);

  if (xpc_get_type(response) == XPC_TYPE_ERROR) {
    fprintf(stderr, "error in response: %s\n",
            xpc_dictionary_get_string(response, XPC_ERROR_KEY_DESCRIPTION));
    exit(EXIT_FAILURE);
  }
  if (xpc_get_type(response) != XPC_TYPE_DICTIONARY) {
    fprintf(stderr, "received non-dictionary response\n");
    xpc_release(message);
    xpc_release(response);
    xpc_release(connection);
    exit(EXIT_FAILURE);
  }
  const char *reply = xpc_dictionary_get_string(response, "reply");
  const char *error = xpc_dictionary_get_string(response, "error");
  if (reply) {
    printf("Reply received: %s\n", reply);
  } else if (error) {
    printf("Error received: %s\n", error);
  } else {
    printf("Unexpected reply received\n");
  }

  xpc_release(message);
  xpc_release(response);
  xpc_release(connection);

  exit(EXIT_SUCCESS);
}
