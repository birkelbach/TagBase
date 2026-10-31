# TagBase
Real Time Tag Database Library

In the industrial automation world the word "Tag" is typically used to define a distinct piece of
data.  A pressure, a temperature, the value of a counter etc. could all be called "Tags."  This 
probably came to be, because instruments were identified with an actual metal tag that would
be fixed to the device.  Each device needed to be uniquely identified so the names were called
"Tag Names."  Then DCS, PLC and HMI software started using the term "Tag" to mean the combination
of the name of the data and the data itself.  Since this library is geared more toward industrial automation and testing it makes sense to  use the term that is common in that industry.

The database that this library is meant to manage is a collection of "Tags" that are stored in
a way where they can be accessed via their "Tag Name."  Each tag can be a single (scalar) piece
of data or an array (or vector) and can be declared as any one of several data types.  Even
user defined custom data types are allowed.

The library is thread safe.

