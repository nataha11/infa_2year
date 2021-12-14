import socket
import sys
import select
#udp client

SVR_ADDR = ("172.27.187.142", 65535)

def main():
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
	s.connect(SVR_ADDR)
	while True:
		try:
			ready_r, _, _ = select.select([sys.stdin, s], [], [])

			for obj in ready_r:
				if obj is sys.stdin:
					line = sys.stdin.readline().strip()
					s.send(line.encode('utf-8'))

				else:
					packet = s.recv(4096)
					try:
						reply = packet.decode('utf-8').strip()
						print('>', reply)

					except UnicodeDecodeError:
						print('>>> (not UTF-8) "%r"', packet)

		except KeyboardInterrupt:
			break

	s.close()

if __name__ == '__main__':
	main()	

