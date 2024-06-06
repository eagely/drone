from flask import Flask, request
import os
import re

app = Flask(__name__)

fifo_path = "data_pipe"

if not os.path.exists(fifo_path):
    os.mkfifo(fifo_path)

@app.route('/', methods=['POST'])
def extract_and_send_integers():
    data = request.data.decode()
    print(data)
    integers = [int(match) for match in re.findall(r'-?\d+', data)]

    with open(fifo_path, "w") as fifo:
        for i, integer in enumerate(integers, start=1):
            uuid = f'FFE{i+1}'
            data_to_write = f"{uuid}:{integer}\n"
            fifo.write(data_to_write)
            print("Writing", integer, "to", uuid)
    
    return "Data forwarded to named pipe successfully!"

if __name__ == '__main__':
    app.run(port=8743)
