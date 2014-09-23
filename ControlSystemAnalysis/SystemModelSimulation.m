%%System Constants%%
k = 550;
lambda = 1;
I = 0.016363148;
r = .050;
m = .01;

%%State Space Model Matrices%%
A = [0,1;-(k/I) -(lambda/I)];
B = [0;1/(I)];
C = [0 1];
D = [0:0];

ssmodel = ss(A,B,C,D);

figure(1);
step(ssmodel);
figure(2);
h = bodeplot(ssmodel);
% Change units to Hz and make phase plot invisible
setoptions(h,'FreqUnits','Hz','PhaseVisible','off');

t = [0:.001:.25];
u = [zeros(50,1)' ones(50,1)' zeros(50,1)' ones(50,1)' zeros(50,1)' 0];

figure(3);
lsim(ssmodel,u,t);

clear all;