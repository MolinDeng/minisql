# How to Build

* Linux

  ```shell
  > make
  > ./minisql
  ```

* Windows

  ```shell
  > mingw32-make
  > minisql.exe
  ```

**<u>Please make sure that executable file and 'data/' in the same file directory</u>**

**<u>*If you want to re-generate lex.cpp, yacc.cpp, yacc.hpp, PLEASE run them in ==Linux==.*</u>**

```shell
# in src\Interpreter\LEXYACC
> ./gen
```

or

```shell
> bison -o yacc.cpp -d yacc.y
> flex -o lex.cpp lex.l
```