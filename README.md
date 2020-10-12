# File tree to sqlite
C++ program that reads the file structure into an sqlite database

# Example

Let's consider a folder `/` with two files `a`,`b` and a folder `c` containing `d`

| id | name | size | parent id |
|----|------|------|-----------|
|  1 |  /   | NULL | NULL      |      
|  2 |  a   | 1000 | 1         |    
|  3 |  b   | 3    | 1         |   
|  4 |  c   | NULL | 1         |  
|  5 |  d   | 36   | 4         |

# Usage:
```sh
cmake .
make
```

# Known issues:
* [ ] SQL error when folder/file has quotes '"' in the name
