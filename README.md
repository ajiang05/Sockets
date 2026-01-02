# Echo Client - Bitwise Operation Protocol

## Overview
This is a C-based TCP client that implements a custom protocol (`cs230`) for performing bitwise operations. The client connects to a server, exchanges messages, and performs various bitwise operations on numbers received from the server.

## Protocol Description

### Message Format
- **Initial Handshake**: `cs230 HELLO <identification>\n`
- **Server Request**: `cs230 STATUS <operation> <number>\n`
- **Client Response**: `cs230 <result>\n`
- **Termination**: Server sends `BYE` when complete

### Supported Operations

#### LEFT - Left bit shift
Shifts the number left by `(number % 10)` positions

#### RIGHT - Right bit shift
Shifts the number right by `(number % 10)` positions

#### OR - Bitwise OR with mask
- Extracts the least significant 4 bits of the number
- Creates a mask by repeating this value: `least_sig * 0x11111111`
- Performs OR operation: `number | mask`

#### AND - Bitwise AND with mask
- Extracts the least significant 4 bits of the number
- Creates a mask by repeating this value: `least_sig * 0x11111111`
- Performs AND operation: `number & mask`

## Compilation
```bash
gcc -o echo_client echo_client.c
```

## Usage
```bash
./echo_client <identification> <port> <host_ip>
```

### Parameters
- **identification**: A string identifier for this client
- **port**: The port number of the server
- **host_ip**: The IP address of the server

### Example
```bash
./echo_client myid123 8080 127.0.0.1
```

## Program Flow

1. **Connection**: Connects to the specified server
2. **Handshake**: Sends `HELLO` message with identification
3. **Operation Loop**:
   - Receives operation request from server
   - Parses operation type and number
   - Performs the requested bitwise operation
   - Sends result back to server
   - Repeats until `BYE` is received
4. **Termination**: Closes connection and exits

## Output

The client prints all sent and received messages, along with the operation details:
```
Echo Client is creating a socket.
Echo Client is trying to connect to (127.0.0.1:8080).
Echo Client connected.
SENT: cs230 HELLO myid123
RECEIVED: cs230 STATUS LEFT 42
Operation: LEFT
Number: 42
SENT: cs230 2688
...
```

## Error Handling

- Validates command-line arguments
- Checks socket creation and connection
- Verifies send/recv operations
- Exits with error messages on failure

## Requirements

- POSIX-compliant system (Linux/Unix/macOS)
- Standard C libraries
- Network connectivity to target server
