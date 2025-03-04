import socket
import pyaudio

UDP_IP = "0.0.0.0"  # Receive from any IP address
UDP_PORT = 12345    # Port to listen for incoming audio
BUFFER_SIZE = 1024  # Size of the buffer to receive audio data

SAMPLE_RATE = 16000     # Sample rate of the audio signal must match the sender
BITS_PER_SAMPLE = 16    # Number of bits per sample must match the sender
CHANNELS = 1            # Number of audio channels (1 for mono, 2 for stereo)

# PyAudio configuration
p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16,
                channels=CHANNELS,
                rate=SAMPLE_RATE,
                output=True)

# UDP socket configuration
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"UDP server waiting audio in {UDP_IP}:{UDP_PORT}...")

try:
    while True:
        data, _ = sock.recvfrom(BUFFER_SIZE)
        stream.write(data)
except KeyboardInterrupt:
    print("\nUser interrupted the program.")

# Close the audio stream and PyAudio
stream.stop_stream()
stream.close()
p.terminate()
sock.close()
print("Program terminated.")
