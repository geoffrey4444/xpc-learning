#include <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xpc/xpc.h>

int main(void) {
  xpc_connection_t listener = xpc_connection_create_mach_service(
      "com.geoffrey4444.minimal", dispatch_get_main_queue(),
      XPC_CONNECTION_MACH_SERVICE_LISTENER);
  if (!listener) {
    fprintf(stderr, "listener could not be created\n");
    exit(EXIT_FAILURE);
  }

  xpc_connection_set_event_handler(listener, ^(xpc_object_t peer) {
    if (xpc_get_type(peer) == XPC_TYPE_ERROR) {
      fprintf(stderr, "listener error: %s\n",
              xpc_dictionary_get_string(peer, XPC_ERROR_KEY_DESCRIPTION));
      return;
    }
    printf("server accepted connection\n");

    xpc_connection_set_event_handler(peer, ^(xpc_object_t event) {
      if (xpc_get_type(event) == XPC_TYPE_ERROR) {
        fprintf(stderr, "peer error: %s\n",
                xpc_dictionary_get_string(event, XPC_ERROR_KEY_DESCRIPTION));
        return;
      }

      if (xpc_get_type(event) != XPC_TYPE_DICTIONARY) {
        fprintf(stderr, "non-dictionary message received.\n");
        return;
      }

      const char *cmd = xpc_dictionary_get_string(event, "command");
      printf("server received command: %s\n", cmd ? cmd : "(null)");

      xpc_object_t reply = xpc_dictionary_create_reply(event);
      if (!reply) {
        fprintf(stderr, "message did not request reply\n");
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
