### Disclaimer

dutc = Don't Use This Code (!!)

# Compatibility

* Python 3.5 or higher
* Linux or OS X
* need build toolchain (prob. need `gcc`)

# Read Watches in Python

```python
>>> import rwatch # enable functionality
>>> from sys import setrwatch, getrwatch
>>> x, y, z = object(), object(), object()
>>> def view(frame, obj):
...     print(frame, obj)
...     return obj
...
>>> setrwatch({id(x): view, id(y): view})
>>> getrwatch()
{139825758638208: <function view at 0x7f2bb89a59d8>, 139825758638224: <function view at 0x7f2bb89a59d8>}
>>> x
<frame object at 0x7f2bb8ad9ba8> <object object at 0x7f2bb8ac9080>
<object object at 0x7f2bb8ac9080>
>>> y
<frame object at 0x7f2bb8ad9d48> <object object at 0x7f2bb8ac9090>
<object object at 0x7f2bb8ac9090>
>>> z
<object object at 0x7f2bb8ac90a0>
```
# Lessons:
 * read watches are a very useful tool for debugging
 * it's actually very useful for a language to have a runtime
 * there are some hidden equivalencies between, e.g., read watches and perfect proxy objects
