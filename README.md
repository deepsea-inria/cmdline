% The Deepsea command-line parser
% Deepsea Project
% 23 January 2016

Introduction
============

This library provides a collection of basic utilities for parsing
string data that is read from the comand line.

The documentation is most easily read in html or pdf format. To render
in these formats, you will need the [pandoc](http://pandoc.org/)
tool. When you have pandoc installed, you can render html and pdf
versions simply by running `make README.html5` or `make README.pdf`.

Command-line conventions {#conventions}
========================

The conventions assumed by this library are quite simple. Each command
line is formed by a sequence of key/value pairs, given in the
following format.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-key1 value1 -key2 value2 ... -keyn valuen
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Because simple switches are convenient, the format provides for one
additional form.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--key
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The form above is equivalent to the following command-line string.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-key 1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

C++ Library interface {#cpp-interface}
=====================

The C++ library consists of a single header file that is written using
modern C++ features. We have tested the library on GCC and Clang. In
order to use the header file, the compiler must be configured to use
C++11 standard (e.g., by passing to the compiler the flag
`-std=c++11`). To use the library in your project, you can simply
include the header file `cmdline.hpp`.

Required library initialization
-------------------------------

The library can be used only after the following function is
called. Any call to other functions in the cmdline library that occur
prior to correct initialization will fail, terminating the program.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
namespace deepsea {
namespace cmdline {

void set(int argc, char** argv);

} }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following code snippet shows a correct initialization of the
cmdline library.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
#include "cmdline.hpp"

int main(int argc, char** argv) {
  deepsea::cmdline::set(argc, argv);
  // can use cmdline here, post initialization
  return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parsing functions
-----------------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
// (1)
template <class Item>
Item parse(std::string key);
// (2)
template <class Item>
Item parse_or_default(std::string key, Item d,
                      bool expected=true);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let the string `-k1 v1 -k2 v2 ...` represent the command-line string
of the program.

1. The call `parse(key)` tries to read from the command line the value
   that is associated with a given key. The function either
     a. returns a value that is the result of parsing the string `vi`
        to a value of type `Item`, if `ki == key`, or
     b. terminates the program, printing an error message to `stderr`,
        otherwise.
2. The call `parse_or_default(key, d, expected)` is similar to the one
   above, but instead of failing when the requested key does not
   appear, the call instead returns the value `d`. If `expected ==
   true`, then a warning message is printed to `stderr` to indicate
   that the default value was taken for the key.

In both cases, the first occurrence of the key matching the string
`key` is the one that is parsed by the function.

Currently, the template parameter `Item` can be instantiated for any
one of the following types: `bool`, `int`, `long`, `float`, `double`,
and `std::string`.

### Example program

Contents of the file `myparse.cpp`.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
#include "cmdline.hpp"
#include <iostream>

int main(int argc, char** argv) {
  deepsea::cmdline::set(argc, argv);
  int x = deepsea::cmdline::parse<int>("x");
  int y = deepsea::cmdline::parse_or_default("y", 123);
  std::cout << "x = " << x << ", y = " << y << std::endl;
  return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sample use:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ g++ -std=c++11 myparse.cpp -o myparse
...
$ myparse -x 500
x = 500, y = 123
$ myparse -x 45 -y 9999
x = 45, y = 9999
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dispatcher
----------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
namespace deepsea {
namespace cmdline {

class dispatcher;

} }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A disptacher is an object that stores a set of label/function pairs
and provides a method to select and execute a function whose
corresponding label matches with a certain string value in the
command-line string.

+-----------------------------------------------+------------------------------------+
| Method                                        | Description                        |
+===============================================+====================================+
| [`add`](#dispatcher-add)                      | Add a function to the dispatcher   |
|                                               |                                    |
+-----------------------------------------------+------------------------------------+
| [`dispatch`](#dispatcher-dispatch)            | Dispatch on the command line       |
|                                               |                                    |
+-----------------------------------------------+------------------------------------+
| [`dispatch_or_default`](#dispatcher-dispatch) | Dispatch on the command line, using|
|                                               |a default label                     |
+-----------------------------------------------+------------------------------------+
                       
Table: Methods of the class `disptacher`.

### Add {#dispatcher-add}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
void add(std::string label, std::function<void()> f);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Adds the pair `label` / `f` to the set.

### Dispatch {#dispatcher-dispatch}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
// (1)
void dispatch(std::string key);
// (2)
void dispatch_or_default(std::string key, std::string d);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let the string `-k1 v1 -k2 v2 ...` represent the command-line string
of the program.

Let the set of the dispatcher be (`l1`, `f1`), (`l2`, `f2`), ...

1. The call `dispatch(key)` performs one of the following actions:
    a. calls function `fj`, if `key == ki` for some string `ki` in the
       command-line string and `ki == lj` for some label `lj` in the
       set of the dispatcher.
    b. halts the program and prints an error message to `stderr`,
       otherwise.
2. The call `dispatch_or_default(key, d)` calls the function `fj`,
   such that `lj == d`, or halts the program and prints an error
   message if there is no such `lj`.

### Example program

Contents of the file `mydispatcher.cpp`:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
#include "cmdline.hpp"
#include <iostream>

int main(int argc, char** argv) {
  deepsea::cmdline::set(argc, argv);
  deepsea::cmdline::dispatcher d;
  d.add("foo", [&] {
    std::cout << "Got foo" << std::endl;
  });
  d.add("bar", [&] {
    std::cout << "Got bar" << std::endl;
  });
  d.disptach_or_default("mykey", "foo");
  return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sample use:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ g++ -std=c++11 mydispatcher.cpp -o mydispatcher
...
$ mydispatcher -mykey foo
Got foo
$ mydispatcher -mykey bar
Got bar
$ mydispatcher
Got foo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Credits {#credits}
=======

- [Umut Acar](http://www.umut-acar.org/site/umutacar/)
- [Arthur Charguéraud](http://www.chargueraud.org/)
- [Mike Rainey](http://gallium.inria.fr/~rainey/)
