# sirius-client
A unix command like sirius client
# Installing sirius

Requires:

* libpcre
* libcurl

Clone this ropsitory
```bash
git clone https://github.com/d4ndo/sirius-client.git
```
Compile sirius-client:

```bash
cd sirius-client
mkdir build
cd build
cmake ../
make
sudo make install
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
-a : sirius will return the content of `speech.wav`
```bash
sirius -a speech.wav
```

-q : sirius will try to answer `question.wav`
```bash
sirius -qa question.wav
```
Equivalent to :
```bash
sirius -a question.wav | sirius
```
## Image matching

In work
