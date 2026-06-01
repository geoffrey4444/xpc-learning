import Darwin
import Dispatch
import OSLog
import XPC

let service_name: String = "com.geoffrey4444.minimalswift"
let logger = Logger(subsystem: service_name, category: "server")

do {
  let _ = try XPCListener(service: service_name) { request in
    logger.info("server accepted connection")

    let decision: XPCListener.IncomingSessionRequest.Decision = request.accept(
      incomingMessageHandler: { (message: XPCDictionary) -> XPCDictionary? in        
        var reply = XPCDictionary()

        let cmd: String? = message["command"]
        logger.info("server received command: \(cmd ?? "(null)", privacy: .public)")

        if (cmd == "ping") {
          reply["reply"] = "pong"
        } else {
          reply["error"] = "unknown command \(cmd ?? "(null)")"
        }

        return reply
      },
      cancellationHandler: { error in
        logger.error("peer error: \(error, privacy: .public)")
      }
    )
    return decision
  }

  dispatchMain()
} catch {
  logger.error("listener could not be created and activated")
  exit(EXIT_FAILURE)
}
