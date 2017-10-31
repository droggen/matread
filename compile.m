mex -v matread.cpp parse.cpp;

c=clock;
f=['matread-' num2str(c(1)) '_' num2str(c(2)) '_' num2str(c(3)) '.zip'];
zip(f,{'matread.cpp' 'parse.cpp' 'compile.m' 'matread.mexw64' 'test.m' 'a.txt'});

