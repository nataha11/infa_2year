#tcp server
import socket
import select

def main():
    srv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    srv_sock.bind(('', 7655))
    srv_sock.listen(10)#очередь и ее размер
    #create poll object for handling multiple sockets
    poll_obj = select.poll()
    poll_obj.register(srv_sock, select.POLLIN)
    #create dict for clients: fd -> (addr, socket, fobj)
    clients = {}
    try:
        while True:
            for fd, event in poll_obj.poll():
                if fd == srv_sock.fileno():
                    # Get one more connection from the aueue
                    client_sock, client_addr = srv_sock.accept()
                    print('Connected from: ', client_addr)
                    # Wrap socket with a file-like object:
                    client_fobj = client_sock.makefile('rw', encoding = 'utf-8')
                    # Add this client to dict and poll_obj for further handlings:
                    clients[client_sock.fileno()] = (client_addr, client_sock, client_fobj)
                    poll_obj.register(client_sock, select.POLLIN)
                    continue
                # New data from some client:
                print('Client event: ', fd, event)
                (client_adr, client_sock, client_fobj) = clients[fd]
                if event == select.POLLIN:
                    #new data from the client, read it and sent to others
                    #TODO: handle misbehaving clients
                    try:
                        line = client_fobj.readline()
                    except OSError as exc:
                        print('\tError while reading', exc)
                        line
                    if len(line) > 0:
                        print('\tClient data: ', line.strip())
                        for other_client_fd in clients:
                            if other_client_fd == fd:
                                continue
                            (other_client_addr, _, other_client_fobj) = clients[other_client_fd]
                            print('\tForwarding to ', other_client_addr)
                            other_client_fobj.write(line)#ВЫЗЫВАЕТ send только если буфер заполнился
                            other_client_fobj.flush() #отправить все что набуферизовалось. не ждать, когда что то еще придет
                        # Successfully forwarded to all other clients, handle newt event
                        continue
                    #POLLIN (or POLLHUP/POLLERR) or POLLIN with 0-length data
                    print('Disconnecting ', client_addr)
                    poll_obj.unregister(fd)
                    client_fobj.close()
                    client_sock.close()
                    del clients[fd]
                    # Handle next event
                    continue

    except KeyboardInterrupt:
        for client_fd in clients:
            (_, client_sock, client_fobj) = clients[client_fd]
            client_fobj.close()
            client_sock.close()
        srv_sock.close()

if __name__ == '__main__':
    main()

