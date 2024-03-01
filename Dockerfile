FROM ubuntu:22.04

RUN apt update && apt install python3-pip ncat -y

RUN useradd -m -s /bin/bash user

# USER user
WORKDIR /home/user

COPY dist/mineziperd .
COPY src/webapp webapp
COPY flag.txt .

RUN ./mineziperd &

WORKDIR /home/user/webapp

RUN pip install -r requirements.txt

EXPOSE 5000
ENV FLASK_APP=app.py
CMD ["flask", "run", "--host", "0.0.0.0"]
