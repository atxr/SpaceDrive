from flask import Flask, render_template, request
import socket
from struct import pack, unpack
import hashlib
import sys
import time
import os

PORT = 8989

files = []

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/upload', methods=['POST'])
def upload():
    if 'file' not in request.files:
        return render_template('upload_status.html', message='No file part', success=False)
    
    file = request.files['file']
    if file.filename == '':
        return render_template('upload_status.html', message='No selected file', success=False)
    
    buf = file.read()
    hash = hashlib.sha256(buf).digest()
    
    # Scan file
    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(10)
            s.connect(("localhost", PORT))
            break

        except:
            print("Restarting mineziper service", file=sys.stdout)
            os.system("pkill mineziperd")
            os.system("~/mineziperd &")
            time.sleep(5)

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
            success = False

        else:
            message = 'File successfully uploaded'
            files.append({'filename': file.filename, 'content':buf})
            success = True

    except:
        message = 'Error: Failed to scan file'
        success = False

    s.close()
    return render_template('upload_status.html', message=message, success=success)

if __name__ == '__main__':
    app.run(debug=True)
