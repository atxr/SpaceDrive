from flask import Flask, render_template, request
import socket
from struct import pack, unpack
import hashlib
import sys

PORT = 8989

files = []

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/upload', methods=['POST'])
def upload():
    if 'file' not in request.files:
        return 'No file part'
    file = request.files['file']
    if file.filename == '':
        return 'No selected file'
    
    buf = file.read()
    hash = hashlib.sha256(buf).digest()
    
    # Scan file
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(10)
    s.connect(("localhost", PORT))

    s.send(pack("I", len(buf)))
    s.send(buf)

    try:
        recv_hash = s.recv(32)
        status = unpack("B", s.recv(1))[0]

        print(f"{status=}", file=sys.stdout)

        if (hash != recv_hash):
            message = 'Error: Mismatching sha256.'
        
        elif (status == 1):
            message = "Error: Potential virus found, cannot upload."

        else:
            message = 'File successfully uploaded'
            files.append({'filename': file.filename, 'content':buf})

    except:
        message = 'Error: Failed to scan file'

    s.close()
    return message

if __name__ == '__main__':
    app.run(debug=False)
