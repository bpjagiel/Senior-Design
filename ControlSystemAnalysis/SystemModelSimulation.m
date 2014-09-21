%%System Constants%%
k = 10;
lambda = 5;
I = 10;
r = 5;
m = 100;

%%State Space Model Matrices%%
A = [0,1;-(k/((I/r^2)+m)) -(lambda/((I/r^2)+m))];
B = [0;1/((I/r^2)+m)];
C = [1 0];
D = [0:0];

t = [0:.001:.999];
u = [zeros(100,1)' ones(100,1)' zeros(100,1)' ones(100,1)' zeros(100,1)' ones(100,1)' zeros(100,1)' ones(100,1)' zeros(100,1)' ones(100,1)'];

ssmodel = ss(A,B,C,D);

figure(1);
step(ssmodel);
figure(2);
lsim(ssmodel,u,t);