# sirius-client
A unix command like sirius client
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
Configuration file can be found at `~/.sirius.ini`

# Getting started

## Asking a question
To ask sirius a question try this:
```bash
sirius "What is the speed of light"
or
echo What is the speed of light | sirius
```

## Speech recognition
»Speaks with pyton host now«

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
