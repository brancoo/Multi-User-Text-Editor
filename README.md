# Multi-User Text Editor build in C
In server side you can execute the following commands:
 [x] - settings; (e.g , number of lines of editor, full name of user's database, ...);
 [x] - load <filename>; (load content from an existing file)
 [x] - save <filename>; (save the current text to a file)
 [x] - free <linenumber>; (clean the <linenumber> line)
 [x] - statistics; (number of words and total chars of each word)
 [x] - users; (shows the username, pipe's name, age's sessions and if he's editing a line at that moment)
 [x] - text; (shows the exact text that is being shown for the users)
 [x] - shutdown; (exits the server)
 
 In client side you can navegate or edit a line.
  In navigation mode you just can move from right to left, or from up to down;
  In editing mode you can add or delete something to a certain line (can not be currently beeing edited), and you can move from   right to left;
  
  You can edit the number of the maximum clients that can be logged in (enviroment variable MEDIT_MAXUSERS) and the timeout of the editor (environment variable MEDIT_TIMEOUT, if the user takes longer than MEDIT_TIMEOUT seconds the same user is switched to navigation mode and all the changes that he did are discarded).
  
## To change the enviroment variables
   Open the terminal;
   Type the following text: 
    ```
    export MEDIT_TIMEOUT=<value> or export MEDIT_MAXUSERS=<value>
    ```
   The project was supposed, at the beginning, to support this functionality for the number of lines and columns of the editor, but at the middle of it was discarded by the authors;
  
## To compile and execute the program
   Open the terminal inside the server folder;
   Type make run;
   To delete the executable you can type make clean, that deletes the server executable (it's inside out's folder);
   
   Open another terminal inside the client folder;
   Type make run;
   For login you should take a look at the file "medit.db" (inside out's folder) and choose or add a username;
   To delete the executable you can type make clean, that deletes the client executable (it's inside out's folder too);
      
   The program has some bugs, one of them is the user's characters (the number of chars that the user added or deleted, after the program loaded the content of the default .txt file).
