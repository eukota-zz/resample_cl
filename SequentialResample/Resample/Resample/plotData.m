clear all
close all
clc

d = csvread('data.csv');

tIn = d(:,1);
sigIn = d(:,2);
tOut = d(:,3);
sigOut = d(:,4);

figure
plot(tIn,sigIn,'o-',tOut,sigOut)