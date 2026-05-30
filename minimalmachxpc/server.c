#include <dispatch/dispatch.h>
#include <os/log.h>
#include <stdlib.h>
#include <string.h>
#include <xpc/xpc.h>

static os_log_t log;

int main(void) {
  log = os_log_create("com.geoffrey4444.minimal", "server");

  xpc_connection_t listener = xpc_connection_create_mach_service(
      "com.geoffrey4444.minimal", dispatch_get_main_queue(),
      XPC_CONNECTION_MACH_SERVICE_LISTENER);
  if (!listener) {
    os_log_error(log, "listener could not be created");
    exit(EXIT_FAILURE);
  }

  xpc_connection_set_event_handler(listener, ^(xpc_object_t peer) {
    if (xpc_get_type(peer) == XPC_TYPE_ERROR) {
      os_log_error(log, "listener error: %{public}s",
                   xpc_dictionary_get_string(peer, XPC_ERROR_KEY_DESCRIPTION));
      return;
    }
    os_log_info(log, "server accepted connection");

    xpc_connection_set_event_handler(peer, ^(xpc_object_t event) {
      if (xpc_get_type(event) == XPC_TYPE_ERROR) {
        os_log_error(
            log, "peer error: %{public}s",
            xpc_dictionary_get_string(event, XPC_ERROR_KEY_DESCRIPTION));
        return;
      }

      if (xpc_get_type(event) != XPC_TYPE_DICTIONARY) {
        os_log_error(log, "non-dictionary message received.");
        return;
      }

      const char *cmd = xpc_dictionary_get_string(event, "command");
      os_log_info(log, "server received command: %{public}s",
                  cmd ? cmd : "(null)");

      xpc_object_t reply = xpc_dictionary_create_reply(event);
      if (!reply) {
        os_log_error(log, "message did not request reply");
        return;
      }

      if (cmd && strcmp(cmd, "ping") == 0) {
        xpc_dictionary_set_string(reply, "reply", "pong");
      } else {
        xpc_dictionary_set_string(reply, "error", "unknown command");
      }

      xpc_connection_send_message(peer, reply);
      xpc_release(reply);
    });
    xpc_connection_activate(peer);
  });
  xpc_connection_activate(listener);

  dispatch_main();
  exit(EXIT_FAILURE);
}
