import rwatch
from sys import getrwatch, setrwatch

def view(f, o):
    print(f,o)
    return o

def f():
    x = object()
    setrwatch({id(x): view})
    print({hex(k): v for k,v in (getrwatch() or {}).items()})
    print('x =', x)

f()
