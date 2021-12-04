import socket
import sys
import select

#SRV_ADDR = ('81.5.88.117', 7500)
SRV_ADDR = ('178.176.75.79', 7654)

def main():
    #Create a TCP socket
    conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    #Establish connection to the server (TODO: configure timeout handling)
    conn.connect(SRV_ADDR)
    conn_fobj = conn.makefile('rw', encoding='utf-8')
    #Create poll object for stdin and socket
    stdin_fd = sys.stdin.fileno()
    poll_array = select.poll()
    poll_array.register(stdin_fd, select.POLLIN)
    poll_array.register(conn, select.POLLIN)
    #Handle input and incoming data:
    running = True
    try:
        while running:
            for fd, event in poll_array.poll():#тут может зависнуть
                if fd == stdin_fd:
                    line = sys.stdin.readline()
                    conn_fobj.write(line)
                    conn_fobj.flush()
                else:
                    if event == select.POLLIN:
                        line = conn_fobj.readline()
                        if len(line) > 0:
                            sys.stdout.write(line)
                            continue
                        # POLLHUP (POLLERR) or 0-length data, server has closed our connection  
                        print('Disconnected')
                        running = False
                        break
    except KeyboardInterrupt:
        conn.shutdown(socket.SHUT_RDWR)
    #We re done, close connection:
    conn.close()


if __name__ == '__main__':
    main()
