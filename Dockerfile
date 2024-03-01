FROM ubuntu:22.04

RUN apt-get update

RUN useradd -m -s /bin/bash user

USER user
WORKDIR /home/user

COPY dist/mineziperd .
COPY webapp .
COPY flag.txt .

RUN ./mineziperd &

WORKDIR /home/user/webapp
RUN pip install -r requirements.txt

ENTRYPOINT [ "python3" ]
CMD [ "app.py" ]
