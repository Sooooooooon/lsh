project members
===
Team No.12 B589006 김순, B589025 박양수, B589060 임성묵

Reason for selecting project
===
We study opensource class for 6months. While learning this class, most of Student think hard in command line linux because black background, and first experience is Command environment. And Control command handle stickle. And our team added shell function to make it more intuitive and user-friendly and convenient handle file. Enjoy our added functions.

Additional Options
===
  * Improved readability and more conveniently
  * Anyone easily handle file (move, copy, paste, delete, make directory)

How to use functions?
===
```

Usage: press the key below description

press key [--similar to existing linux command]  description command

  W,                      cursor move up
  A,                      cursor move left
  S,                      cursor move down
  D,                      cursor move right
  E, --excute             excute output file or program file
  Q, --quit               this shell function exit
  Z, --delete             delete the file pointed to the current cursor
  N, --mkdir              make new directory 
  C, --copy               copy the file pointed to the current cursor
  X, --move               move the file pointed to the current cursor
  V, --paste              paste the file by copyed or moved
```


[1]: http://brennan.io/2015/01/16/write-a-shell-in-c/

![screenshot1](./image/1.lsh_execute.PNG)
* excute LSH shell(our team fork LSH shell) 

![screenshot2](./image/2.team12_execute.PNG)
* run added function shell

![screenshot3](./image/3.outfile_excute.PNG)
* move cursor by “W or A or S or D” key pressed and excute file “E” key pressed 

![screenshot4](./image/4.mkdir.PNG)
* make directory by “N” key pressed

![screenshot5](./image/5.folder_check.PNG)
* check new directory  
![screenshot6](./image/6.cut_paste1.PNG)
* Press key “C” to copy the file at the current cursor.
![screenshot7](./image/7.cut_paste2.PNG)
* Press key “V” to paste the copyed file
![screenshot8](./image/8.cut_copy2.PNG)
* Press key “X” to cut the file at the current cursor.
![screenshot9](./image/9.copy_paste.PNG)
* Press key “V” to paste the moved file
![screenshot10](./image/10.copy_paste2.PNG)
![screenshot11](./image/11.delete1.PNG)
![screenshot12](./image/12.delete2.PNG)
![screenshot13](./image/13.q_exit.PNG)
