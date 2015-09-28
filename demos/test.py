#!/usr/bin/env python3

try:
    from sys import setrwatch, getrwatch
except ImportError:
    try:
        import rwatch
        from sys import setrwatch, getrwatch
    except ImportError:
        from sys import exit; exit('http://')

############ Wouldn't this be (very!) useful for debugging? ############

def debug_demo(x='some data'):
    from inspect import getframeinfo
    def view(frame, obj):
        info = getframeinfo(frame)
        msg = 'Access to {!r} (@{}) at {}:{}:{}'
        print(msg.format(obj, hex(id(obj)), info.filename, info.lineno, info.function))
        return obj

    setrwatch({id(x): view})
    print(getrwatch())

    def f(x):
        x # ← here
    f(x) # ← here

    def f(x):
        def g():
            x # ← here
        return g
    f(x)() # ← here

    class Foo:
        def __init__(self, x):
            self.x = x # ← here
        def __call__(self):
            return self.x # ← here
        @property
        def value(self):
            return self.x # ← here

    foo = Foo(x) # ← here
    foo()
    foo.value

    x = 10 # not here (rebind, no push)

########## But maybe also? ##########

def sandbox_demo(x='some data'):
    def forbid(frame, obj):
        raise SystemError("can't touch this")

    setrwatch({id(x): forbid})

    try:
        # x
        pass
    except:
        pass

########## Some weird things are possible... ##########

def defer_demo():
    def lazy(frame, obj):
        obj.load()
        return obj

    class Lazily:
        def load(self):
            self.value = 10

    x = Lazily()

    setrwatch({id(x): lazy})
    print(x.value)

    def promise(frame, obj):
        return obj.value

    class Promise:
        @property
        def value(self):
            return 10

    x = Promise()

    setrwatch({id(x): promise})
    print(x)

########## But it can get even weirder... ##########

def indirection_demo():
    def pointer(frame, obj):
        return obj()

    def make_pointer(x):
        cell = [x]
        pointer = lambda: cell[0]
        def repoint(x):
            cell[0] = x
        return pointer, repoint

    y, z = 1234, 4567
    x, x_ = make_pointer(y)

    setrwatch({id(x): pointer})

    print(x)
    x_(z)
    print(x)

if __name__ == '__main__':
    debug_demo()
    sandbox_demo()
    defer_demo()
    indirection_demo()

# Interestingly, the above shows the equivalent between the provision
#   of a perfect proxy object (wherein Proxy(x) is in all ways
#   indistiguishable from x) and read watches.
