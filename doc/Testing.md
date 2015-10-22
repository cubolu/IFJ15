## Unit Testing

The IFJ15 project uses the [Check](http://check.sourceforge.net/) C Unit Testing library.

Most of what you need to know to add new tests can be found in this [tutorial](http://check.sourceforge.net/doc/check_html/check_3.html).

#### Installing Check

You need to install the Check library on your system to be able to write/run the tests.

On RPM based systems you can do this using:
```
dnf install check check-devel
```

On Debian based systems you can do this using:
```
apt-get install check
```

#### IFJ15 Specific Instructions

- All unit tests are inside the `test` subdirectory.
- You can run the tests by using `make test` in the main directory or `make run` in the `test` directory.
- To successfully compile the tests from the `test` directory, you need to make sure the main project is complied in the parent directory, because the unit testing binary needs parent `*.o` files for linking.
