# CBattleField
一个基于C语言的在控制台上的战争模拟器。其灵感来源于游戏《战地1》

A field war simulator in console with C language, inspiring from BattleField 1.

你可以在[这里]( https://www.youtube.com/watch?v=490ShC3bjOg )查看演示。

You could watch the demo [here]( https://www.youtube.com/watch?v=490ShC3bjOg ).

Q&A:

Q1:**我该如何编译它?**

Q1:**How could I compile it?**

***在Linux平台上时***

***On Linux***

A1:使用指令 "gcc game.c gametime.c -lm" 来编译它。

A1:use the command "gcc game.c gametime.c -lm" to compile it. 

***在Windows平台上时***

***On Windows***

A1:使用指令 "gcc game.c gametime.c clear_screen.c -lm" 来编译它。

A1:use the command "gcc game.c gametime.c clear_screen.c -lm" to compile it.

提示:

tips:

你可以在源文件"game.c"中修改宏"MAX"的值来修改战争中的AI数量。

You could change the value of the macro "MAX" which is the number of the AI in the battle in the file "game.c".


