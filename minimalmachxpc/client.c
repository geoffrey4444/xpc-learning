#include <dispatch/dispatch.h>
#include <os/log.h>
#include <stdlib.h>
#include <xpc/xpc.h>

static os_log_t log;

int main(void) {
  log = os_log_create("com.geoffrey4444.minimal", "client");

  xpc_connection_t connection = xpc_connection_create_mach_service(
      "com.geoffrey4444.minimal", dispatch_get_main_queue(), 0);
  if (!connection) {
    os_log_error(log, "could not connect to service");
    exit(EXIT_FAILURE);
  }

  xpc_connection_set_event_handler(connection, ^(xpc_object_t event) {
    if (xpc_get_type(event) == XPC_TYPE_ERROR) {
      os_log_error(log, "connection error: %{public}s",
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
    os_log_error(
        log, "error in response: %{public}s",
        xpc_dictionary_get_string(response, XPC_ERROR_KEY_DESCRIPTION));
    exit(EXIT_FAILURE);
  }
  if (xpc_get_type(response) != XPC_TYPE_DICTIONARY) {
    os_log_error(log, "received non-dictionary response");
    xpc_release(message);
    xpc_release(response);
    xpc_release(connection);
    exit(EXIT_FAILURE);
  }
  const char *reply = xpc_dictionary_get_string(response, "reply");
  const char *error = xpc_dictionary_get_string(response, "error");
  if (reply) {
    os_log_info(log, "Reply received: %{public}s", reply);
  } else if (error) {
    os_log_info(log, "Error received: %{public}s", error);
  } else {
    os_log_info(log, "Unexpected reply received");
  }

  xpc_release(message);
  xpc_release(response);
  xpc_release(connection);

  exit(EXIT_SUCCESS);
}
