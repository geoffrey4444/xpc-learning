import Darwin
import Dispatch
import OSLog
import XPC

let service_name: String = "com.geoffrey4444.minimalswift"
let logger = Logger(subsystem: service_name, category: "client")

do {
  let session = try XPCSession(machService: service_name)
  logger.info("connect to service")

  var message = XPCDictionary()
  message["command"] = "ping"

  do {
    let response = try session.sendSync(message: message)

    if let reply: String = response["reply"] {
      logger.info("Reply received: \(reply, privacy: .public)")
    } else if let error: String = response["error"] {
      logger.info("Error received: \(error, privacy: .public)")
    } else {
      logger.info("Unexpected reply received")
    }

  } catch {
    logger.error("could not send message to service")
  }

} catch {
  logger.error("could not connect to service")
  exit(EXIT_FAILURE)
}
