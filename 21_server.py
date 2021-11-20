import socket
import codecs

PORT = 65535

def main():
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
	s.bind(('', PORT))
	clients = set()
	print('Listeing on port', PORT)

	while True:
		try:
			data, addr_info = s.recvfrom(4096)
			print('Received a packet (len = %d, data = %r) from %s' % (len(data), data, addr_info))

			if addr_info not in clients:
				print('New client:', addr_info)
				clients.add(addr_info)

			for client_addr in clients:
				s.sendto(data, client_addr)
				
		except KeyboardInterrupt:
			break
	
	s.close()

if __name__== '__main__':
	main()	



