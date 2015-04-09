# sirius-client
A unix command like sirius client

Requires:

* libpcre
* libcurl

# Howto

## Question
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
sirius -a question | sirius
```

