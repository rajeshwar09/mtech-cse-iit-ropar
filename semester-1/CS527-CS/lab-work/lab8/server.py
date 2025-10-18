import socket
import sys

HOST = ""            # listen on all interfaces (or set to "0.0.0.0")
PORT = 9998

def create_and_bind():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5)
        print(f"[OK] Listening on port {PORT}")
        return s
    except Exception as e:
        print(f"[ERROR] create/bind: {e}")
        sys.exit(1)

def accept_loop(server_socket):
    while True:
        print("[TASK] Waiting for connection...")
        conn, addr = server_socket.accept()
        print(f"[TASK] Connection established from {addr[0]}:{addr[1]}")
        try:
            handle_client(conn)
        except Exception as e:
            print(f"[ERROR] during client handling: {e}")
        finally:
            conn.close()
            print("[TASK] Connection closed")

def handle_client(conn):
    # interactive loop: send commands to client and print responses
    while True:
        try:
            cmd = input("shell> ").strip()
        except (KeyboardInterrupt, EOFError):
            print("\n[INFO] Shutting down server")
            conn.close()
            sys.exit(0)

        if cmd == "":
            continue

        if cmd.lower() == "quit":
            conn.sendall(cmd.encode())
            conn.close()
            sys.exit(0)

        # send command
        try:
            conn.sendall(cmd.encode())
        except BrokenPipeError:
            print("[ERROR] Connection broken while sending")
            break

        # recv response (loop until no more data or small timeout)
        try:
            resp = conn.recv(4096)
            if not resp:
                print("[INFO] Client closed connection")
                break
            print(resp.decode("utf-8"), end="")
        except Exception as e:
            print(f"[ERROR] receiving response: {e}")
            break

def main():
    server_socket = create_and_bind()
    accept_loop(server_socket)

if __name__ == "__main__":
    main()
