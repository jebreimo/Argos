area
====

*area* calculates the area of an arbitrary triangle or quadrilateral
defined by the coordinates (x,y) of their corners.

Examples:

~~~
~/Argos/examples/area/build $ ./area 1,1 5.5,2 2,6
10.75
~/Argos/examples/area/build $ ./area 1,1 5.5,2 6,4.75 2,6 
16.5625
~~~

Help text:
~~~
USAGE
  example2 --help
  example2 <X,Y> <X,Y> <X,Y> [<X,Y>]

Computes the area of a triangle or quadrilateral.

ARGUMENTS
  <X,Y> <X,Y> <X,Y> [<X,Y>]
        The coordinates of the corners in a triangle or quadrilateral.

OPTIONS
  -h, --help
        Display the help text.
~~~
