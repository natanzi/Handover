import socket
import sys
import time

# Replace with the server's IP address and port
HOST = '127.0.0.1'  # The server's IP address
PORT = 54321        # The port used by the server

def create_connection(host, port, retries=5, delay=2):
    """Attempts to create a socket connection to the server, with retries."""
    attempt = 0
    while attempt < retries:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((host, port))
            print(f"Successfully connected to the server {host} on port {port}")
            return s
        except socket.error as err:
            print(f"Connection failed with error: {err}. Retrying in {delay} seconds...")
            time.sleep(delay)
            attempt += 1
    print("Maximum retries reached. Failed to connect to the server.")
    return None

def send_handover_command(sock, ue_id, target_enb_id):
    """Sends a handover command to the server with a unique message ID and handles network errors."""
    try:
        # Generate a unique message ID for this command
        message_id = str(uuid.uuid4())
        # Construct the handover command with message ID
        message = f"{message_id}:Handover command for UE: {ue_id} to target eNB: {target_enb_id}\n"
        sock.sendall(message.encode())

        # Wait for a response from the server
        response = sock.recv(1024).decode()

        # Extract message ID from response
        response_id, response_message = response.split(':', 1)

        if response_id == message_id:
            print(f"Server response for {message_id}: {response_message}")
        else:
            print(f"Received mismatched response ID {response_id} for message ID {message_id}")

    except socket.error as err:
        print(f"Send/receive failed with error: {err}.")
        # Here you can decide whether to terminate or retry the command
        return False
    except ConnectionResetError:
        print("Connection was closed by the server.")
        return False
    return True

def main():
    sock = create_connection(HOST, PORT)
    if not sock:
        print("Failed to connect to the server. Exiting.")
        sys.exit(1)

    try:
        message_id = 0
        while True:
            # Increment message ID for each command sent
            message_id += 1
            
            # Example UE ID and target eNB ID for handover
            ue_id = '123'
            target_enb_id = '456'
            
            if not send_handover_command(sock, ue_id, target_enb_id, message_id):
                print("Attempting to reconnect to the server...")
                sock = create_connection(HOST, PORT)
                if not sock:
                    print("Reconnection failed. Exiting.")
                    break

            # Here you can implement your "is alive" checks or other commands

            # Example delay between handover commands
            time.sleep(5)

    except KeyboardInterrupt:
        print("Interrupted by the user.")
    finally:
        if sock:
            sock.close()  # Ensure the socket is closed on exit

if __name__ == '__main__':
    main()

