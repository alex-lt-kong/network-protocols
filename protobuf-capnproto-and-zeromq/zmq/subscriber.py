import logging
import zmq

context = zmq.Context()

#  Socket to talk to server
logging.info("Connecting to publisher endpoint")
socket = context.socket(zmq.SUB)
socket.connect("tcp://127.0.0.1:4242")
logging.info("Connected to endpoint")


while True:
    #  Get the reply.
    message = socket.recv()
    print(f"Image received [{len(message)}]")
    with open("/tmp/test.jpg", "wb") as binary_file:
        # Write bytes to file
        binary_file.write(message.data())