# sirius-client
<a href="https://scan.coverity.com/projects/5136">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/5136/badge.svg"/>
</a>

A unix command like sirius client

Deprecated. Whole design of sirius changed. Now called lucida (https://github.com/claritylab/lucida)
# Installing

Requires:

* libpcre
* libcurl

Clone this repsitory: `git clone https://github.com/d4ndo/sirius-client.git`

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

# Configuration

The configuration file can be found at `~/.sirius.ini`

```bash
[host]                          ;host configuration
url = localhost                 ;ip address or domain
question_port = 8080            ;question daemon PORT
speech_recognition_port = 8081  ;speech recognition daemon PORT
image_matching_port = 8082      ;image matching daemon PORT
```

# Getting started

## Asking a question
To ask sirius a question try this:
```bash
sirius "What is the speed of light"
or
echo What is the speed of light | sirius
```

## Speech recognition

-a : sirius will return the content of `question.wav`
```bash
sirius -a question.wav
what is the speed of light
```

-q : sirius will try to answer `question.wav`
```bash
sirius -qa question.wav
299,792,458 meters per second
```
is equivalent to :
```bash
sirius -a question.wav | sirius
299,792,458 meters per second
```
## Image matching

-i: sirius will return information about the image

```bash
sirius -i towerOFpisa.jpg
tower pisa
```

Some batch stuff:
```bash
ls -1 *.jpg | while read line; do sirius -i $line; done;
```
