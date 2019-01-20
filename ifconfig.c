#include <Locator.h>
#include <TextTool.h>
#include <tcpip.h>

#include <string.h>

/*
 * ifconfig [ up | down | status ]
 *
 */

/*
 * callback by Marinetti to display a connect message
 *
 */
#pragma databank 1
void display(const char *pstr) { WriteLine(pstr); }
#pragma databank 0

// todo - support flags
// -s: be silent (ie - no output, return value only
// -f: force disconnect
//

int main(int argc, char **argv) {
    LongWord address;
    char buffer[16];

    if (argc != 2) {
        WriteCString("Usage: ifconfig [ up | down | status ]\n\r");
        WriteCString("  up      Connects to network\n\r");
        WriteCString("  down    Disconnects from network\n\r");
        WriteCString("  status  Queries status\n\r");
        exit(1);
    }

    if (!strcmp(argv[1], "up")) {

        TCPIPStatus();
        if (_toolErr) {
            LoadOneTool(54, 0x0200);
            if (_toolErr) {
                ErrWriteCString("Unable to load Marinetti\r\n");
                exit(1);
            }
        }
        // acedemic only - TCPStartup() just for show.
        if (!TCPIPStatus()) {
            TCPIPStartUp();
        }
        if (TCPIPGetConnectStatus()) {
            ErrWriteCString("Already connected to network\r\n");
            exit(0);
        }
        TCPIPConnect(display);
        if (_toolErr) {
            ErrWriteCString("Error connecting to network\r\n");
            exit(1);
        }
        WriteCString("Connected to network\r\n");
        WriteCString("IP Address is: ");
        address = TCPIPGetMyIPAddress();
        TCPIPConvertIPToCASCII(address, buffer, 0);
        WriteCString(buffer);
        WriteCString("\r\n");
        exit(0);
    }

    if (!strcmp(argv[1], "down")) {
        // if tool isn't loaded, we're already down...
        TCPIPStatus();
        if (_toolErr) {
            ErrWriteCString("Marinetti not loaded\r\n");
            exit(0);
        }
        if (!TCPIPGetConnectStatus()) {
            ErrWriteCString("Not connected to network\r\n");
            exit(0);
        }
        // todo - flag to force up & down
        TCPIPDisconnect(true, display);

        WriteCString("Disconnected from network\r\n");
        // UnloadOneTool(54);
        exit(0);
    }

    if (!strcmp(argv[1], "status")) {
        // if tool isn't loaded, we're already down...
        TCPIPStatus();
        if (_toolErr) {
            WriteCString("Network is down\r\n");
            exit(1);
        }
        if (!TCPIPGetConnectStatus()) {
            WriteCString("Network is down\r\n");
            exit(1);
        }
        WriteCString("Network is up\r\n");

        WriteCString("IP Address is: ");
        address = TCPIPGetMyIPAddress();
        TCPIPConvertIPToCASCII(address, buffer, 0);
        WriteCString(buffer);
        WriteCString("\r\n");
        exit(0);
    }

    ErrWriteCString("illegal option -- ");
    ErrWriteCString(argv[1]);
    ErrWriteCString("\r\n");
    exit(1);
}
